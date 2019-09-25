/*===============================================================*/
/*                                                               */
/*                          utils.cpp                            */
/*                                                               */
/*                       Utility functions                       */
/*                                                               */
/*===============================================================*/

#include <string>
#include <cstdlib>
#include <cstdio>
#include <getopt.h>

#include "utils.h"

void print_usage(char* filename)
{
    printf("usage: %s <options>\n", filename);
    printf("  -f [kernel file]\n");
    printf("  -o [output file]\n");
}

void parse_sdaccel_command_line_args(
    int argc,
    char** argv,
    std::string& kernelFile,
    std::string& outFile) 
{

  int c = 0;

  while ((c = getopt(argc, argv, "f:o:")) != -1) 
  {
    switch (c) 
    {
      case 'f':
        kernelFile = optarg;
        break;
      case 'o':
        outFile = optarg;
        break;
      default:
      {
        print_usage(argv[0]);
        exit(-1);
      }
    } // matching on arguments
  } // while args present
}

void parse_sdsoc_command_line_args(
    int argc,
    char** argv,
    std::string& outFile) 
{

  int c = 0;

  while ((c = getopt(argc, argv, "o:")) != -1) 
  {
    switch (c) 
    {
      case 'o':
        outFile = optarg;
        break;
      default:
      {
        print_usage(argv[0]);
        exit(-1);
      }
    } // matching on arguments
  } // while args present
}
