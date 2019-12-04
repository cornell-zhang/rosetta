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
#include <fstream>

#include "utils.h"

void print_usage(char* filename)
{
    printf("usage: %s <options>\n", filename);
    printf("  -f [kernel file]\n");
    printf("  -o [output file]\n");
}

void parse_command_line_args(
    int argc,
    char** argv,
    std::string& outFile, 
    std::string& image_path) 
{

  int c = 0;

  while ((c = getopt(argc, argv, "p:o:")) != -1) 
  {
    switch (c) 
    {
      case 'o':
        outFile = optarg;
        break;
      case 'p':
        image_path = optarg;
        break;
      default:
      {
        print_usage(argv[0]);
        exit(-1);
      }
    } // matching on arguments
  } // while args present
}

void read_input(std::string& image_path, unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH])
{
  std::ifstream infile;
  infile.open(image_path);
  if (infile.is_open())
  {
    for ( int i = 0; i < IMAGE_HEIGHT; i++)
    {
      for (int j = 0; j < IMAGE_WIDTH; j ++ )
      {
        unsigned int pix;
        infile >> std::hex >> pix;
        Data[i][j] = (unsigned int)pix;
      }
    }

    infile.close();
  }
  else
  {
    printf("Failed to open data file!\n");
    exit(-1);
  }
}

