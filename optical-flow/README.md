# Optical Flow

## Description
This design computes the movement of each pixel in five continuous image frames. 
The hardware function constructs a streaming dataflow pipeline between different stages of the algorithm. 
The `dataset` directory contains our test input and the reference output flow file. 
The `imageLib` directory contains a small image library from Microsoft Research for image file I/O. 

## Major Optimizations
Our optical flow design uses three major optimization techniques:
1. Data packing. The hardware function kernel takes in five consecutive image frames. 
The corresponding pixels in the five frames are packed into a 64-bit integer for fast off-chip data transfer. 
2. Image line buffer and window buffer. The kernels in optical flow have sliding window access patterns. 
As a result, line buffer and window buffer are introduced to exploit data reuse and reduce accesses to the next-level memory. 
3. Streaming dataflow optimization. 
After using line buffer and window buffer, the kernels in optical flow have perfectly balanced throughput, and access data in strict sequential order. 
As a result, execution of all stages can be perfectly overlapped to form a very deep image processing pipeline. 

## Usage
### SDAccel
To execute or simulate the design, do

`./optical_flow_host.exe -f <path_to_xclbin_file> -p <path_to_dataset> -o <path_to_output_file>`

### SDSoC
To run design on the board, do

`./optical_flow.elf -p <path_to_dataset> -o <path_to_output_file>`

### SW
To run software version, do

`./optical_flow_sw.exe -p <path_to_dataset> -o <path_to_output_file>`

### Note
`<path_to_output_file>` must end with the extension `.flo`. Example: `output.flo`, `out.flo`. 

## Known Issues
1. In function `gradient_xy_calc`, if we use the hls built-in line buffer, the tool can not fully pipeline the loop `GRAD_XY_INNER`. 

## Current Benchmark

### On ZC706
| Benchmark | #LUTs | #FFs | #BRAMs | #DSPs | Runtime (ms) | Throughput |
|:---------:|:-----:|:----:|:-----:|:------:|:------------:|:----------:|
|Floating_point|42878|61078|54|454|24.3|41.2 frames/s|
|Fixed_point|27869|47860|72|354|19.7|50.8 images/s|
