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
#include <fstream>

#include "utils.h"

void print_usage(char* filename)
{
    printf("usage: %s <options>\n", filename);
    printf("  -p [path to data]\n");
}

void parse_command_line_args(
    int argc,
    char** argv,
    std::string& path_to_data) 
{

  int c = 0;

  while ((c = getopt(argc, argv, "p:")) != -1) 
  {
    switch (c) 
    {
      case 'p':
        path_to_data = optarg;
        break;
      default:
      {
        print_usage(argv[0]);
        exit(-1);
      }
    } // matching on arguments
  } // while args present
}

int read_input(std::string path_to_data, bit32** input)
{
  std::ifstream infile;
  infile.open(path_to_data);
  if (infile.is_open())
  {
    int num_3d_tri;
    infile >> num_3d_tri;
    printf("%d triangles\n", num_3d_tri);
    *input = (bit32*)calloc(3 * MAX_NUM_TRIS, sizeof(bit32));

    // pack input data for better performance
    for ( int i = 0; i < num_3d_tri; i++)
    {
      unsigned int x0, y0, z0, x1, y1, z1, x2, y2, z2;
      infile >> x0 >> y0 >> z0 >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
      //printf("%d %d %d %d %d %d %d %d\n", x0, y0, z0, x1, y1, z1, x2, y2, z2);
      
      (*input)[3*i](7,0)     = x0;
      (*input)[3*i](15,8)    = y0;
      (*input)[3*i](23,16)   = z0;
      (*input)[3*i](31,24)   = x1;
      (*input)[3*i+1](7,0)   = y1;
      (*input)[3*i+1](15,8)  = z1;
      (*input)[3*i+1](23,16) = x2;
      (*input)[3*i+1](31,24) = y2;
      (*input)[3*i+2](7,0)   = z2;
      (*input)[3*i+2](31,8)  = 0;
    }

    infile.close();
    return num_3d_tri;
  }
  else
  {
    printf("Failed to open data file!\n");
    exit(-1);
  }

  return -1;
}


