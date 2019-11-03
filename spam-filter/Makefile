# Set kernel name
KERNEL_NAME = SgdLR

# Set host source and headers
HOST_SRC_CPP = ./src/host/spam_filter.cpp ./src/host/utils.cpp ./src/host/check_result.cpp
HOST_SRC_H   = ./src/host/utils.h ./src/host/check_result.h ./src/host/typedefs.h 
DATA         = ./data/*.dat

# Set host code include paths
HOST_INC = -I$(XILINX_VIVADO)/include
HOST_LIB = -L$(XILINX_VIVADO)/lib

# Set kernel file
OCL_KERNEL_SRC = ./src/ocl/sgd.cpp
OCL_KERNEL_H = ./src/host/typedefs.h ./src/ocl/lut.h
SDSOC_KERNEL_SRC = ./src/sdsoc/sgd.cpp
SDSOC_KERNEL_H = ./src/host/typedefs.h ./src/sdsoc/lut.h
SW_KERNEL_SRC = ./src/sw/sgd_sw.cpp
SW_KERNEL_H = ./src/host/typedefs.h

# Set opencl kernel arguments
OCL_KERNEL_ARGS = --max_memory_ports all --report system -I$(shell pwd)/src/

#-------------------------
# Leave the rest to harness
#-------------------------
include ../harness/harness.mk

