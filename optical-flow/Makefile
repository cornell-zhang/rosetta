# Set kernel name
KERNEL_NAME = optical_flow

# Set host source and headers
HOST_SRC_CPP = ./src/host/optical_flow_host.cpp ./src/host/utils.cpp ./src/host/check_result.cpp ./imageLib/Convert.cpp \
               ./imageLib/Convolve.cpp ./imageLib/flowIO.cpp ./imageLib/Image.cpp ./imageLib/ImageIO.cpp ./imageLib/RefCntMem.cpp
HOST_SRC_H   = ./src/host/utils.h ./src/host/check_result.h ./src/host/typedefs.h ./imageLib/*.h
DATA         = 

# Set host code include paths
HOST_INC = -I$(XILINX_VIVADO)/include -I./imageLib
HOST_LIB =  -L$(XILINX_VIVADO)/lib

# Set kernel file
OCL_KERNEL_SRC = ./src/ocl/optical_flow.cpp
OCL_KERNEL_H = ./src/host/typedefs.h
SDSOC_KERNEL_SRC = ./src/sdsoc/optical_flow.cpp
SDSOC_KERNEL_H = ./src/host/typedefs.h
SW_KERNEL_SRC = ./src/sw/optical_flow_sw.cpp
SW_KERNEL_H = ./src/host/typedefs.h ./src/sw/optical_flow_sw.h

# Set opencl kernel arguments
OCL_KERNEL_ARGS = --max_memory_ports all --report system -I$(shell pwd)/src/

#-------------------------
# Leave the rest to harness
#-------------------------
include ../harness/harness.mk

