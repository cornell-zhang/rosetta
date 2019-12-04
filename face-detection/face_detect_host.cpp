/*===============================================================*/
/*                                                               */
/*                       face_detect.cpp                         */
/*                                                               */
/*     Main host function for the Face Detection application.    */
/*                                                               */
/*===============================================================*/

// standard C/C++ headers
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <string>
#include <time.h>
#include <sys/time.h>
#include "face_detect.h"
// other headers
#include "utils.h"
#include "typedefs.h"
#include "check_result.h"

int main(int argc, char ** argv) 
{
  printf("Face Detection Application\n");

  std::string image_path = "";
  std::string outFile = "";
  parse_command_line_args(argc, argv, outFile, image_path);

  static unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH];
  read_input(image_path, Data);

  // outputs
  int result_x[RESULT_SIZE];
  int result_y[RESULT_SIZE];
  int result_w[RESULT_SIZE];
  int result_h[RESULT_SIZE];
  int res_size = 0;

  // timers
  struct timeval start, end;

  gettimeofday(&start, 0);
  face_detect(Data, result_x, result_y, result_w, result_h, &res_size);
  gettimeofday(&end, 0);

  // check results
  printf("Checking results:\n");
  check_results(res_size, result_x, result_y, result_w, result_h, Data, outFile);
    
  // print time
  long long elapsed = (end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec;   
  printf("elapsed time: %lld us\n", elapsed);

  return EXIT_SUCCESS;

}
