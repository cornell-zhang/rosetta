# Digit Recognition

## Description
This design classifies hand-written digits using the K-Nearest-Neighbor (KNN) algorithm. 
The hardware accelerator compares the test instance with all training instances, and votes between the K nearest neighbors to get a final classification result. 
The images of training and test instances are binarized so we can exploit bit-level parallelism on the FPGA. 
The `196data` directory contains a binarized, downsampled subset of the MNIST database. 

## Major Optimizations
This application only uses the basic loop unrolling, pipelining and array partitioning optimizations to improve performance. 
However, comparing with the software baseline, the code goes through significant restructuring to enable these optimization opportunities. 
A separate array is created to hold the results of the concurrent comparisons. 

## Usage
### SDAccel
To execute or simulate the design, do

`./Digitrec_host.exe -f <path_to_xclbin_file>`

### SDSoC
To run design on the board, do

`./DigitRec.elf`

### Software
To run software model, do

`./DigitRec_sw.exe`

## Known Issues
1. In `src/ocl/digitrec.cl`, the last `async_work_group_copy` can not be properly pipelined. 
2. The `knn_vote` function must be manually inlined for the tool to correctly pipeline `TRAINING_LOOP` and not try to pipeline `TEST_LOOP`. 
