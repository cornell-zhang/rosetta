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
    printf("  -p [path to training data]\n");
    printf("  -t [path to test data]\n");
    printf("  -l [path to label data]\n");
}

void parse_command_line_args(
    int argc,
    char** argv,
    std::string& path_to_training_data,
    std::string& path_to_test_data,
    std::string& path_to_label_data) 
{

  int c = 0;

  while ((c = getopt(argc, argv, "l:t:p:")) != -1) 
  {
    switch (c) 
    {
      case 'p':
        path_to_training_data = optarg;
        break;
      case 't':
        path_to_test_data = optarg;
        break;
      case 'l':
        path_to_label_data = optarg;
        break;
      default:
      {
        print_usage(argv[0]);
        exit(-1);
      }
    } // matching on arguments
  } // while args present
}

int read_data(std::string path_to_data, WholeDigitType* data, bool training)
{
  std::ifstream infile;
  infile.open(path_to_data);
  if (infile.is_open())
  {
    int num_samples;
    infile >> num_samples;
    printf("%d samples\n", num_samples);
    int class_samples = num_samples / PAR_FACTOR;

    // pack input data for better performance
    if (training) 
    {
      for (int c = 0; c < PAR_FACTOR; c ++ )
      {
        for ( int i = 0; i < class_samples; i++)
        {
          DigitType d0, d1, d2, d3;
          infile >> std::hex >> d0 >> d1 >> d2 >> d3;
          data[c * MAX_NUM_TRAINING / PAR_FACTOR + i].range(63 , 0  ) = d0;
          data[c * MAX_NUM_TRAINING / PAR_FACTOR + i].range(127, 64 ) = d1;
          data[c * MAX_NUM_TRAINING / PAR_FACTOR + i].range(191, 128) = d2;
          data[c * MAX_NUM_TRAINING / PAR_FACTOR + i].range(255, 192) = d3;
        }
      }
    }
    else
    {
      for ( int i = 0; i < num_samples; i++)
      {
        DigitType d0, d1, d2, d3;
        infile >> std::hex >> d0 >> d1 >> d2 >> d3;
        data[i].range(63 , 0  ) = d0;
        data[i].range(127, 64 ) = d1;
        data[i].range(191, 128) = d2;
        data[i].range(255, 192) = d3;
      }
    }
    infile.close();
    return num_samples;
  }
  else
  {
    printf("Failed to open data file!\n");
    exit(-1);
  }

  return -1;
}

int read_labels(std::string path_to_labels, LabelType* labels)
{
  std::ifstream infile;
  infile.open(path_to_labels);
  if (infile.is_open())
  {
    int num_samples;
    infile >> num_samples;
    printf("%d labels\n", num_samples);

    // pack input data for better performance
    for ( int i = 0; i < num_samples; i++)
    {
      unsigned int l;
      infile >> l;
      labels[i] = (LabelType)l;
    }

    infile.close();
    return num_samples;
  }
  else
  {
    printf("Failed to open data file!\n");
    exit(-1);
  }

  return -1;
}

