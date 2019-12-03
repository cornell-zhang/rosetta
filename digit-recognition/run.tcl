#=============================================================================
# run_base.tcl 
#=============================================================================
# @brief: A Tcl script for synthesizing the baseline digit recongnition design.

# Open/reset the project
open_project accel.prj -reset
open_solution "solution1"

source user_interface.tcl

# Top function of the design is "dut"
set_top ${top_module}

# Use Zynq device
# zc-706
set_part {xcku060-ffva1156-2-e}
# Target clock period is 10ns
create_clock -period 4

config_rtl -reset state

############################################
# Simulate the C++ design
csim_design -argv ${sim_args}
# Synthesize the design
csynth_design
# Co-simulate the design
cosim_design -rtl verilog -setup -argv ${sim_args}
exit
