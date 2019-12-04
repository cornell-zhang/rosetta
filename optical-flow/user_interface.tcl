# define the top module
set top_module optical_flow

# Add design and testbench files
add_files optical_flow.cpp -cflags "-I./imageLib"
add_files -tb optical_flow_host.cpp -cflags "-I./imageLib"
add_files -tb check_result.cpp -cflags "-I./imageLib"
add_files -tb utils.cpp -cflags "-I./imageLib"
add_files -tb ./imageLib/Convert.cpp -cflags "-I./imageLib"
add_files -tb ./imageLib/Convolve.cpp -cflags "-I./imageLib"
add_files -tb ./imageLib/flowIO.cpp -cflags "-I./imageLib"
add_files -tb ./imageLib/Image.cpp -cflags "-I./imageLib"
add_files -tb ./imageLib/ImageIO.cpp -cflags "-I./imageLib"
add_files -tb ./imageLib/RefCntMem.cpp -cflags "-I./imageLib"

# insert optimization directives here if necessary

# define simulation arguments
set sim_args "-p <absolute_path_to_dataset_folder> -o out.flo"
# example "-p <path_to_optical_flow>/datasets/tennis/ -o out.flo"
