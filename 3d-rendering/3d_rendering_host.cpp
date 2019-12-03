/*===============================================================*/
/*                                                               */
/*                       3d_rendering.cpp                        */
/*                                                               */
/*      Main host function for the 3D Rendering application.     */
/*                                                               */
/*===============================================================*/

// standard C/C++ headers
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <string>
#include <time.h>
#include <sys/time.h>
#include "rendering.h"

// other headers
#include "typedefs.h"
#include "check_result.h"
#include "utils.h"

int main(int argc, char ** argv) 
{
  printf("3D Rendering Application\n");

  std::string path_to_data("");
  parse_command_line_args(argc, argv, path_to_data);

  bit32* input;
  int num_3d_triangles = read_input(path_to_data, &input);

  // timers
  struct timeval start, end;

  // create space for input and output
  bit32* output = (bit32*)malloc(NUM_FB * sizeof(bit32));

  // run hardware function and time it

  gettimeofday(&start, 0);
  rendering(input, output, num_3d_triangles);
  gettimeofday(&end, 0);


  // check results
  printf("Checking results:\n");
  check_results(output);
 
  // print time
  long long elapsed = (end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec;   
  printf("elapsed time: %lld us\n", elapsed);

  free(input);
  free(output);

  return EXIT_SUCCESS;

}
