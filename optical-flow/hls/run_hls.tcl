# Create a project
open_project -reset test_oflow

# Add design files
add_files sdsoc/optical_flow.cpp
add_files sdsoc/optical_flow.h

# Add test bench & files
add_files -tb host/optical_flow_host.cpp
add_files -tb host/check_result.cpp
add_files -tb host/utils.cpp
add_files -tb host/check_result.h
add_files -tb host/utils.h
add_files -tb host/typedefs.h
add_files -tb ../imageLib/Convert.cpp
#add_files -tb imageLib/Convert.h
add_files -tb ../imageLib/Convolve.cpp
add_files -tb imageLib/Convolve.h
#add_files -tb imageLib/Error.h
add_files -tb ../imageLib/Image.cpp
add_files -tb imageLib/Image.h
#
add_files -tb ../imageLib/ImageIO.cpp
add_files -tb imageLib/ImageIO.h
add_files -tb ../imageLib/RefCntMem.cpp
add_files -tb ../imageLib/RefCntMem.h
add_files -tb ../imageLib/flowIO.cpp
add_files -tb imageLib/flowIO.h
add_files -tb imageLib/imageLib.h

# Set the top-level function
set_top optical_flow

# ########################################################
# Create a solution
open_solution -reset solution1
# Define technology and clock rate
set_part  {zynq}
create_clock -period 7

csim_design -argv "-p /home/pbc48/rosetta_g/rosetta/optical-flow/datasets/sintel_alley -o out.flo"

exit


