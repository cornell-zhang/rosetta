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

// data
#include "image0_320_240.h"

int main(int argc, char ** argv) 
{
  printf("Face Detection Application\n");

  std::string outFile("");
  parse_sdsoc_command_line_args(argc, argv, outFile);

  // for this benchmark, input data is included in array Data
  // these are outputs
  int result_x[RESULT_SIZE];
  int result_y[RESULT_SIZE];
  int result_w[RESULT_SIZE];
  int result_h[RESULT_SIZE];
  int res_size = 0;

  // timers
  struct timeval start, end;

  // As the SDSoC generated data motion network does not support sending 320 X 240 images at once
  // We needed to send all the 240 rows using 240 iterations. The last invokation of detectFaces() does the actual face detection
  for ( int i = 0; i < IMAGE_HEIGHT-1; i ++ )
    face_detect(Data[i], result_x, result_y, result_w, result_h, &res_size);
  
  gettimeofday(&start, 0);
  face_detect(Data[IMAGE_HEIGHT-1], result_x, result_y, result_w, result_h, &res_size);
  gettimeofday(&end, 0);

  // check results
  printf("Checking results:\n");
  check_results(res_size, result_x, result_y, result_w, result_h, Data, outFile);
    
  // print time
  long long elapsed = (end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec;   
  printf("elapsed time: %lld us\n", elapsed);

  return EXIT_SUCCESS;

}
