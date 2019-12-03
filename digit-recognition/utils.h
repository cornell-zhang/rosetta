/*===============================================================*/
/*                                                               */
/*                           utils.h                             */
/*                                                               */
/*                       Utility functions                       */
/*                                                               */
/*===============================================================*/

#include "typedefs.h"

void print_usage(char* filename);
void parse_command_line_args(
    int argc,
    char** argv,
    std::string& path_to_training_data,
    std::string& path_to_test_data,
    std::string& path_to_label_data); 

int read_data(std::string path_to_data, WholeDigitType* data, bool training = true);
int read_labels(std::string path_to_labels, LabelType* labels);
