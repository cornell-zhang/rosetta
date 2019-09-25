# Rosetta: A Realistic High-level Synthesis Benchmark Suite for Software Programmable FPGAs

## Publication
-------------------------------------------------------------------------------------------
If you use Rosetta in your research, please cite [our FPGA'18 paper][1]:
```
  @article{zhou-rosetta-fpga2018,
    title   = "{Rosetta: A Realistic High-Level Synthesis Benchmark Suite for
                Software-Programmable FPGAs}",
    author  = {Yuan Zhou and Udit Gupta and Steve Dai and Ritchie Zhao and 
               Nitish Srivastava and Hanchen Jin and Joseph Featherston and
               Yi-Hsiang Lai and Gai Liu and Gustavo Angarita Velasquez and
               Wenping Wang and Zhiru Zhang},
    journal = {Int'l Symp. on Field-Programmable Gate Arrays (FPGA)},
    month   = {Feb},
    year    = {2018},
  }
```
[1]: http://www.csl.cornell.edu/~zhiruz/pdfs/rosetta-fpga2018.pdf

## Introduction 
-------------------------------------------------------------------------------------------
Rosetta is a set of realistic benchmarks for software programmable FPGAs. 
It contains six fully-developed applications from machine learning and image/video processing domains, where each benchmark consists multiple compute kernels that expose diverse sources of parallelism. 
These applications are developed under realistic design constraints, and are optimized at both kernel-level and application-level with the advanced features of HLS tools to meet these constraints. 
As a result, Rosetta is not only a practical benchmark suite for the HLS community, but also a design tutorial on how to build application-specific FPGA accelerators with state-of-the-art HLS tools and optimizations. 
We will continue to include more applications and optimize existing benchmarks. 

This branch contains the versions of the applications that are easy to run with Vivado HLS. For 
more results on actual hardware please refer to the master branch for details. 

## Applications
-------------------------------------------------------------------------------------------
1. 3D rendering;
2. Digit recognition;
3. Spam filtering;
4. Optical flow;
5. Binarized neural network, adopted from [our open-source BNN implementation][2];
6. Face detection, adopted from [our open-source Haar face detection implementation][3].

[2]: https://github.com/cornell-zhang/bnn-fpga
[3]: https://github.com/cornell-zhang/facedetect-fpga

## Code Structure
-------------------------------------------------------------------------------------------
In this branch, each directory contains all design source files, datasets, as well as the `.tcl`
files used for Vivado HLS. There are two `.tcl` files: `user_interface.tcl` allows the user to 
specify the source and testbench files, apply additional applications, and specify the command line
arguments for simulation. `run.tcl` is the main file that Vivado HLS uses. 
 
## Usage
-------------------------------------------------------------------------------------------
To run C software simulation, high-level synthesis, and C-RTL co-simulation, please go into the 
application directory of interest, and run 
```
vivado_hls -f run.tcl
```
*For BNN, please first go into* `cpp/minizip`*, run* `make`*, go into* `cpp/accel`*,then run*
```
vivado_hls -f hls.tcl
```

*Notice that for Spam Filtering and Optical Flow, please modify the simulation arguments in*
`user_interface.tcl` *and change the path to the dataset in the last line. *


