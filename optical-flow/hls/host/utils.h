/*===============================================================*/
/*                                                               */
/*                           utils.h                             */
/*                                                               */
/*                       Utility functions                       */
/*                                                               */
/*===============================================================*/

void print_usage(char* filename);

void parse_sdaccel_command_line_args(
    int argc,
    char** argv,
    std::string& kernelFile,
    std::string& dataPath,
    std::string& outFile);

void parse_sdsoc_command_line_args(
    int argc,
    char** argv,
    std::string& dataPath,
    std::string& outFile  ); 
