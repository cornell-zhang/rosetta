# define the top module
set top_module DigitRec

# Add design and testbench files
add_files digitrec.cpp
add_files -tb digit_recognition.cpp
add_files -tb check_result.cpp
add_files -tb 196data/training_set_0.dat
add_files -tb 196data/training_set_1.dat
add_files -tb 196data/training_set_2.dat
add_files -tb 196data/training_set_3.dat
add_files -tb 196data/training_set_4.dat
add_files -tb 196data/training_set_5.dat
add_files -tb 196data/training_set_6.dat
add_files -tb 196data/training_set_7.dat
add_files -tb 196data/training_set_8.dat
add_files -tb 196data/training_set_9.dat
add_files -tb 196data/test_set.dat
add_files -tb 196data/expected.dat

# insert optimization directives here if necessary

# define simulation arguments
set sim_args " "
