# Spam Filtering

## Description
This design trains a logistic regression model to classify emails as either "spam" or "ham". 
We use stochastic gradient descent (SGD) in our accelerator to train the model. 
The `dataset` directory contains our test dataset with 5000 emails. 

## Usage
### SDAccel
To execute or simulate the design, do

`./SgdLR_host.exe -f <path_to_xclbin_file> -p <path_to_dataset>`

### SDSoC
To run design on the board, do

`./SgdLR.elf -p <path_to_dataset>`

### SW
To run software version, do

`./SgdLR_sw -p <path_to_dataset>`

## Known Issues
1. In `ocl/sgd.cpp`, macros defined in `host/typedefs.h` can not be used in array partition pragmas. 
2. SDSoC can not share the same code with SDAccel. During execution the program would deadlock with dataflow. As a result, each invocation of the hardware function computes one epoch in the SDSoC version. 
