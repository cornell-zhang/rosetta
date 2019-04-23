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

For each Rosetta benchmark, we provide an unoptimized software version which does not use any HLS-specific optimization, and optimized versions targeting cloud and embedded FPGA platforms. 
Rosetta currently supports Xilinx SDx 2017.1, which combines the previous Xilinx SDAccel and Xilinx SDSoC development environments. 
SDAccel is used for cloud FPGA platforms, and SDSoC is used for embedded FPGA platforms. 
Our designs have been tested on the AWS f1.2xlarge instance and a local ZC706 evaluation kit. Major results are as follows. 
For more results please refer to [our FPGA'18 paper][1]. 

### Rosetta results on ZC706

| Benchmark | #LUTs | #FFs | #BRAMs | #DSPs | Runtime (ms) | Throughput |
|:---------:|:-----:|:----:|:-----:|:------:|:------------:|:----------:|
|3D Rendering|8893|12471|48|11|4.7|213 frames/s|
|Digit Recognition<sup>1</sup>|41238|26468|338|1|10.6|189k digits/s|
|Spam Filtering<sup>2</sup>|12678|22134|69|224|60.8|370k samples/s|
|Optical Flow|42878|61078|54|454|24.3|41.2 frames/s|
|BNN<sup>3</sup>|46899|46760|102|4|4995.2|200 images/s|
|Face Detection|62688|83804|121|79|33.0|30.3 frames/s|

1: K=3, `PAR_FACTOR`=40.

2: Five epochs, `PAR_FACTOR`=32, `VDWIDTH`=64.

3: Eight convolvers, 1000 test images.
 
### Rosetta results on AWS F1

| Benchmark | #LUTs | #FFs | #BRAMs | #DSPs | Runtime (ms) | Throughput | Performance-cost Ratio | 
|:---------:|:-----:|:----:|:-----:|:------:|:------------:|:----------:|:----------------------:|
|3D Rendering|6763|7916|36|11|4.4|227 frames/s|496k frames/$|
|Digit Recognition<sup>1</sup>|39971|33853|207|0|11.1|180k digits/s|393M digits/$|
|Spam Filtering<sup>2</sup>|7207|17434|90|224|25.1|728k samples/s|1.6G samples/$|
|Optical Flow|38094|63438|55|484|8.4|119 frames/s|260k frames/$|
|Face Detection|48217|54206|92|72|21.5|46.5 frames/s|101k frames/$|

1: K=3, `PAR_FACTOR`=40.

2: Five epochs, `PAR_FACTOR`=32, `VDWIDTH`=512.

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
The `harness` directory contains the wrapper code for OpenCL APIs, as well as the main makefile. 
The `src` directory contains the source code for CPU host function (`host`), software implementation (`sw`), sdsoc hardware function implementation (`sdsoc`), and sdaccel hardware function implementation (`ocl`).
Each benchmark has its own makefile specifying the paths to necessary source files. 
 
## Usage
-------------------------------------------------------------------------------------------
### BNN
The `BNN` folder is currently a copy of the original [BNN repo][2] by Zhao et.al. For instructions on how to simulate and compile the design please refer to the README file inside the folder. 

### SDAccel compilation steps:
1. Figure out your target platform. SDAccel only supports a limited number of platforms. 
The code for your target platform can be found from the SDAccel user guide, or any other materials provided by the platform vendor. 
SDAccel also supports using custom platforms which are not integrated yet. 
A platform specification file (usually has the extension `.xpfm`) is needed to describe the target platform. 
2. Go into any benchmark folder.
3. To compile for software emulation and get a quick latency estimate, do `make ocl OCL_TARGET=sw_emu`. 
The report `system_estimate.xtxt` shows latency and resource estimate after high-level synthesis. 
If only a software model is needed, comment out `--report estimate` from the local makefile. 
Compilation time will significantly decrease. 
4. To compile for hardware emulation, do `make ocl OCL_TARGET=hw_emu`.
5. To compile for bitstream and actually execute on the board, do `make ocl OCL_TARGET=hw`.
6. Target platform can be specified with the `OCL_DEVICE` variable. 
Default is Alpha Data 7v3 board. 
For example, to target the Alpha Data KU3 board and generate bitstream, do 
`make ocl OCL_TARGET=hw OCL_DEVICE=xilinx:adm-pcie-ku3:2ddr-xpr:4.0`. 
To use a custom platform, specify its path with the `OCL_PLATFORM` variable. 
For example, to generate bitstream for a custom platform, do 
`make ocl OCL_TARGET=hw OCL_PLATFORM=<path_to_custom_platform_xfpm_file>`. 
Also remember to change the target device string in `host/typedefs.h`. 
7. Please refer to SDAccel user guide for instructions on running simulations. 
For instructions on how to run the applications, please refer to the READMEs in the benchmark folders. 

### SDAccel on AWS
After finishing the required setup steps on AWS, follow above steps with following differences:
1. Use the option `OCL_PLATFORM=$AWS_PLATFORM`.
The environment variable `AWS_PLATFORM` specifies the location of the AWS platform file. 
2. In `host/typedefs.h` set `TARGET_DEVICE = "xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0"`.
3. When running the application, choose the `.awsxclbin` bitstream file instead of `.xclbin`.

### SDSoC compilation steps:
1. Go into any benchmark folder. 
2. Do `make sdsoc`.
3. The target platform is now hard-coded in the makefiles. All benchmarks currently target the ZC706 platform. 

### Software compilation steps:
1. Go into any benchmark folder. 
2. Do `make sw`. 

### Run the applications:
Please refer to the README files in the corresponding application folder for instructions. 
