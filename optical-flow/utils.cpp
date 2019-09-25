/*===============================================================*/
/*                                                               */
/*                          utils.cpp                            */
/*                                                               */
/*                       Utility functions                       */
/*                                                               */
/*===============================================================*/

#include <string>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#include "utils.h"

void print_usage(char* filename)
{
    printf("usage: %s <options>\n", filename);
    printf("  -f [kernel file]\n");
    printf("  -p [path to data]\n");
    printf("  -o [path to output]\n");
}

void parse_sdaccel_command_line_args(
    int argc,
    char** argv,
    std::string& kernelFile,
    std::string& dataPath,
    std::string& outFile  ) 
{

  int c = 0;

  while ((c = getopt(argc, argv, "f:p:o:")) != -1) 
  {
    switch (c) 
    {
      case 'f':
        kernelFile = optarg;
        break;
      case 'p':
        dataPath = optarg;
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
    std::string& dataPath,
    std::string& outFile  ) 
{

  int c = 0;

  while ((c = getopt(argc, argv, "p:o:")) != -1) 
  {
    switch (c) 
    {
      case 'p':
        dataPath = optarg;
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
