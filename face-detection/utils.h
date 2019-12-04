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
    std::string& outFile,
    std::string& image_path);

void read_input(std::string& image_path, unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH]);
