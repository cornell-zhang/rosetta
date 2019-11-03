# Set kernel name
KERNEL_NAME = face_detect

# Set host source and headers
HOST_SRC_CPP = ./src/host/face_detect_host.cpp ./src/host/utils.cpp ./src/host/check_result.cpp ./src/host/image.cpp
HOST_SRC_H   = ./src/host/utils.h ./src/host/check_result.h ./src/host/typedefs.h ./src/host/image0_320_240.h \
               ./src/host/image.h
DATA         = 

# Set host code include paths
HOST_INC = -I$(XILINX_VIVADO)/include
HOST_LIB = -L$(XILINX_VIVADO)/lib

# Set kernel file
OCL_KERNEL_SRC = ./src/ocl/face_detect.cpp
OCL_KERNEL_H = ./src/host/typedefs.h ./src/ocl/*.h
SDSOC_KERNEL_SRC = ./src/sdsoc/face_detect.cpp
SDSOC_KERNEL_H = ./src/host/typedefs.h ./src/sdsoc/*.h
SW_KERNEL_SRC = ./src/sw/face_detect_sw.cpp
SW_KERNEL_H = ./src/host/typedefs.h ./src/sw/*.h

# Set opencl kernel arguments
OCL_KERNEL_ARGS = --max_memory_ports all --report_level 2 -I$(shell pwd)/src/

#-------------------------
# Leave the rest to harness
#-------------------------
include ../harness/harness.mk

