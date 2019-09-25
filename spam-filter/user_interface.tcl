# define the top module
set top_module SgdLR

# Add design and testbench files
add_files sgd.cpp
add_files -tb spam_filter.cpp
add_files -tb check_result.cpp
add_files -tb utils.cpp

# insert optimization directives here if necessary

# define simulation arguments
set sim_args "-p <absolute_path_to_dataset>"
