# define the top module
set top_module DigitRec

# Add design and testbench files
add_files digitrec.cpp
add_files -tb digit_recognition.cpp
add_files -tb check_result.cpp
add_files -tb utils.cpp

# insert optimization directives here if necessary

# define simulation arguments
set sim_args "-p <path_to_training_data> -t <path_to_test_data> -l <path_to_test_labels>"
# example "-p <path_to_digit_recognition/196data/all_training.dat> \
#          -t <path_to_digit_recognition>/196data/test_set.dat \
#          -l <path_to_digit_recognition>/196data/expected.dat"
