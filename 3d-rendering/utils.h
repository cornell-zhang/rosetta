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
    std::string& path_to_data);

int read_input(std::string path_to_data, bit32** input);
