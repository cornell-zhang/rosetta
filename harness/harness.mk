# ======================================== Check Xilinx SDX Environment Settings ================================================== #
ifndef XILINX_SDX
  $(error Environment variable XILINX_SDX is required and should point to SDx install area)
endif

# =============================================== Tools Used in Rosetta =========================================================== #

# sdaccel tools
OCL_CXX   = xcpp
XOCC      = xocc

# sdsoc tools
SDSXX     = sds++

# default sw compiler
SW_CXX = g++

# ============================================= SDAccel Platform and Target Settings ============================================== #

# Set Default OpenCL device and platform
USR_PLATFORM = n
OCL_DEVICE = xilinx:adm-pcie-7v3:1ddr:3.0
OCL_PLATFORM = one_of_default_platforms

# Check if the user specified opencl platform
ifneq ($(OCL_PLATFORM), one_of_default_platforms)
  USR_PLATFORM=y
endif

# Check OCL_TARGET value
OCL_TARGET  = sw_emu
ifeq ($(OCL_TARGET),sw_emu)
else ifeq ($(OCL_TARGET),hw_emu)
else ifeq ($(OCL_TARGET),hw)
else
  $(error "OCL_TARGET does not support the $(OCL_TARGET) value. Supported values are: sw_emu, hw_emu, hw")
endif

# Check opencl kernel file type
OCL_KERNEL_TYPE = ocl

ifeq ($(suffix $(OCL_KERNEL_SRC)),.cl)
  OCL_KERNEL_TYPE=ocl
else
  OCL_KERNEL_TYPE=c
endif

# OpenCL runtime Libraries
OPENCL_INC = $(XILINX_XRT)/include/
OPENCL_LIB = $(XILINX_XRT)/lib/

# opencl harness files
OCL_HARNESS_DIR     = ../harness/ocl_src
OCL_HARNESS_SRC_CPP = $(OCL_HARNESS_DIR)/CLKernel.cpp $(OCL_HARNESS_DIR)/CLMemObj.cpp $(OCL_HARNESS_DIR)/CLWorld.cpp
OCL_HARNESS_SRC_H   = $(OCL_HARNESS_DIR)/CLKernel.h   $(OCL_HARNESS_DIR)/CLMemObj.h   $(OCL_HARNESS_DIR)/CLWorld.h

# host compilation flags
OCL_HOST_FLAGS = -DOCL -g -lxilinxopencl -I$(OPENCL_INC) $(HOST_INC) -L$(OPENCL_LIB) $(HOST_LIB) -I$(OCL_HARNESS_DIR)

# xclbin compilation flags
XCLBIN_FLAGS = -s -t $(OCL_TARGET) -g

ifneq ($(KERNEL_TYPE),ocl)
  XCLBIN_FLAGS += --kernel $(KERNEL_NAME)
endif

ifeq ($(USR_PLATFORM),n)
  XCLBIN_FLAGS += --xdevice $(OCL_DEVICE)
else
  XCLBIN_FLAGS += --platform $(OCL_PLATFORM)
endif

XCLBIN_FLAGS += $(OCL_KERNEL_ARGS)
#XCLBIN_FLAGS += -I$(BENCHMARK_DIR)

# host exe
OCL_HOST_EXE        = $(KERNEL_NAME)_host.exe

# Kernel XCLBIN file
XCLBIN        = $(KERNEL_NAME).$(OCL_TARGET).xclbin
# Kernel XO file
XO            = $(KERNEL_NAME).$(OCL_TARGET).xo
# =============================================== SDSoC Platform and Target Settings ============================================== #

# platform
SDSOC_PLATFORM = zc706

# executable
SDSOC_EXE = $(KERNEL_NAME).elf

# sds++ flags
SDSFLAGS = -sds-pf $(SDSOC_PLATFORM) -sds-hw $(KERNEL_NAME) $(SDSOC_KERNEL_SRC) -sds-end -clkid 3  \
           -poll-mode 1 -verbose
SDSCFLAGS += -DSDSOC -Wall -O3 -c
SDSCFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
SDSLFLAGS = -O3 

# objects
ALL_SDSOC_SRC = $(HOST_SRC_CPP) $(SDSOC_KERNEL_SRC)
OBJECTS := $(ALL_SDSOC_SRC:.cpp=.o)
DEPS := $(OBJECTS:.o=.d)

# =============================================== Pure Software Compilation Settings ============================================== #

# compiler flags
SW_FLAGS = -DSW -O3

# sw executable
SW_EXE = $(KERNEL_NAME)_sw.exe

# ========================================================= Rules ================================================================= #

# we will have 4 top-level rules: ocl, sdsoc, sw and clean
# default to sw

.PHONY: all ocl sdsoc sw clean emu_setup

all: sw

# ocl rules
ocl: $(OCL_HOST_EXE) $(XCLBIN)

# ocl secondary rule: host executable
$(OCL_HOST_EXE): $(HOST_SRC_CPP) $(HOST_SRC_H) $(OCL_HARNESS_SRC_CPP) $(OCL_HARNESS_SRC_H) $(DATA)
	$(OCL_CXX) $(OCL_HOST_FLAGS) -o $@ $(HOST_SRC_CPP) $(OCL_HARNESS_SRC_CPP) 

# ocl secondary rule: xclbin 
$(XCLBIN): $(XO)
	$(XOCC) -l $(XCLBIN_FLAGS) -o $@ $(XO)

# ocl secondary rule: the .xo file
$(XO): $(OCL_KERNEL_SRC) $(OCL_KERNEL_H)
	$(XOCC) -c $(XCLBIN_FLAGS) $(OCL_KERNEL_SRC) -o $@ 

# emulation setup
emu_setup: 
	emconfigutil --platform $(OCL_PLATFORM) 
	echo "Please set the environment variable XCL_EMULATION_MODE to either sw_emu or hw_emu!"

# sdsoc rules
sdsoc: $(SDSOC_EXE)

$(SDSOC_EXE): $(OBJECTS)
	$(SDSXX) $(SDSFLAGS) $(SDSLFLAGS) ${OBJECTS} -o $@

-include $(DEPS)

%.o: %.cpp
	$(SDSXX) $(SDSFLAGS) $(SDSCFLAGS) $< -o $@


# software rules
sw: $(HOST_SRC_CPP) $(HOST_SRC_H) $(SW_KENREL_SRC) $(SW_KERNEL_H) $(DATA)
	$(SW_CXX) $(SW_FLAGS) -o $(SW_EXE) $(HOST_SRC_CPP) $(SW_KERNEL_SRC)

# cleanup
clean:
	rm -rf *.exe *.elf *.xclbin *.xo *.log *.bit *.rpt _xocc* _sds sd_card _x .run .Xil *.json *.info
	rm -rf ./src/host/*.d
	rm -rf ./src/sdsoc/*.o
	rm -rf ./src/sdsoc/*.d
	rm -rf ./src/host/*.o
