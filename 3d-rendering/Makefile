# Set kernel name
KERNEL_NAME = rendering

# Set host source and headers
HOST_SRC_CPP = ./src/host/3d_rendering_host.cpp ./src/host/utils.cpp ./src/host/check_result.cpp
HOST_SRC_H   = ./src/host/utils.h ./src/host/check_result.h ./src/host/typedefs.h ./src/host/input_data.h 
DATA         = 

# Set host code include paths
HOST_INC = -I$(XILINX_VIVADO)/include
HOST_LIB = -L$(XILINX_VIVADO)/lib

# Set kernel file
OCL_KERNEL_SRC = ./src/ocl/rendering.cpp
OCL_KERNEL_H = ./src/host/typedefs.h
SDSOC_KERNEL_SRC = ./src/sdsoc/rendering.cpp
SDSOC_KERNEL_H = ./src/host/typedefs.h
SW_KERNEL_SRC = ./src/sw/rendering_sw.cpp
SW_KERNEL_H = ./src/host/typedefs.h ./src/sw/rendering_sw.h

# Set opencl kernel arguments
OCL_KERNEL_ARGS = --max_memory_ports all --report_level 2 -I$(shell pwd)/src/

#-------------------------
# Leave the rest to harness
#-------------------------
include ../harness/harness.mk

