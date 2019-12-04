# define the top module
set top_module rendering

# Add design and testbench files
add_files rendering.cpp
add_files -tb 3d_rendering_host.cpp
add_files -tb check_result.cpp
add_files -tb utils.cpp

# insert optimization directives here if necessary

# define simulation arguments
set sim_args "-p <absolute path to data>"
# example: "-p <your_absolute_path_to_3d_rendering>/data/input_data1"
