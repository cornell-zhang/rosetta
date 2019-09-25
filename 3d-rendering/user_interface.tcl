# define the top module
set top_module rendering

# Add design and testbench files
add_files rendering.cpp
add_files -tb 3d_rendering_host.cpp
add_files -tb check_result.cpp

# insert optimization directives here if necessary

# define simulation arguments
set sim_args " "
