# define the top module
set top_module face_detect

# Add design and testbench files
add_files face_detect.cpp
add_files -tb face_detect_host.cpp
add_files -tb check_result.cpp
add_files -tb image.cpp
add_files -tb utils.cpp

# insert optimization directives here if necessary

# define simulation arguments
set sim_args "-o out.pgm"
