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

#ifdef OCL
  // opencl harness headers
  #include "CLWorld.h"
  #include "CLKernel.h"
  #include "CLMemObj.h"
  // harness namespace
  using namespace rosetta;
#endif

#ifdef SDSOC
  // sdsoc headers
  #include "sds_lib.h"
  // hardware function declaration
  #include "../sdsoc/face_detect.h"
#endif

#ifdef SW
  # include "../sw/face_detect_sw.h"
#endif

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
  #ifdef OCL
    // parse command line arguments
    std::string kernelFile("");
    parse_sdaccel_command_line_args(argc, argv, kernelFile, outFile);
  #else
    parse_sdsoc_command_line_args(argc, argv, outFile);
  #endif 

  // for this benchmark, input data is included in array Data
  // these are outputs
  int result_x[RESULT_SIZE];
  int result_y[RESULT_SIZE];
  int result_w[RESULT_SIZE];
  int result_h[RESULT_SIZE];
  int res_size = 0;

  // timers
  struct timeval start, end;

  // opencl version host code
  #ifdef OCL  
    // create OpenCL world
    CLWorld facedetect_world = CLWorld(TARGET_DEVICE, CL_DEVICE_TYPE_ACCELERATOR);
  
    // add the bitstream file
    facedetect_world.addProgram(kernelFile);
  
    // create kernels
    // this kernel is written in C++
    CLKernel FaceDetect(facedetect_world.getContext(), facedetect_world.getProgram(), "face_detect", facedetect_world.getDevice());
  
    // create mem objects
    CLMemObj input_mem   ( (void*)Data,        sizeof(unsigned char), IMAGE_HEIGHT * IMAGE_WIDTH, CL_MEM_READ_ONLY);
    CLMemObj res_x_mem   ( (void*)result_x,    sizeof(int),           RESULT_SIZE,                CL_MEM_WRITE_ONLY);
    CLMemObj res_y_mem   ( (void*)result_y,    sizeof(int),           RESULT_SIZE,                CL_MEM_WRITE_ONLY);
    CLMemObj res_w_mem   ( (void*)result_w,    sizeof(int),           RESULT_SIZE,                CL_MEM_WRITE_ONLY);
    CLMemObj res_h_mem   ( (void*)result_h,    sizeof(int),           RESULT_SIZE,                CL_MEM_WRITE_ONLY);
    CLMemObj res_size_mem( (void*)(&res_size), sizeof(int),           1,                          CL_MEM_WRITE_ONLY);
  
    // start timer
    gettimeofday(&start, 0);
  
    // add them to the world
    // added in sequence, each of them can be referenced by an index
    facedetect_world.addMemObj(input_mem);
    facedetect_world.addMemObj(res_x_mem);
    facedetect_world.addMemObj(res_y_mem);
    facedetect_world.addMemObj(res_w_mem);
    facedetect_world.addMemObj(res_h_mem);
    facedetect_world.addMemObj(res_size_mem);
  
    // set work size
    int global_size[3] = {1, 1, 1};
    int local_size[3] = {1, 1, 1};
    FaceDetect.set_global(global_size);
    FaceDetect.set_local(local_size);
  
    // add them to the world
    facedetect_world.addKernel(FaceDetect);
  
    // set kernel arguments
    facedetect_world.setMemKernelArg(0, 0, 0);
    facedetect_world.setMemKernelArg(0, 1, 1);
    facedetect_world.setMemKernelArg(0, 2, 2);
    facedetect_world.setMemKernelArg(0, 3, 3);
    facedetect_world.setMemKernelArg(0, 4, 4);
    facedetect_world.setMemKernelArg(0, 5, 5);
  
    // run!
    facedetect_world.runKernels();
  
    // read the data back
    facedetect_world.readMemObj(1);
    facedetect_world.readMemObj(2);
    facedetect_world.readMemObj(3);
    facedetect_world.readMemObj(4);
    facedetect_world.readMemObj(5);
   
    // end timer
    gettimeofday(&end, 0);
  #endif

  // sdsoc host code
  #ifdef SDSOC
    // As the SDSoC generated data motion network does not support sending 320 X 240 images at once
    // We needed to send all the 240 rows using 240 iterations. The last invokation of detectFaces() does the actual face detection
    for ( int i = 0; i < IMAGE_HEIGHT-1; i ++ )
      face_detect(Data[i], result_x, result_y, result_w, result_h, &res_size);
  
    gettimeofday(&start, 0);
    face_detect(Data[IMAGE_HEIGHT-1], result_x, result_y, result_w, result_h, &res_size);
    gettimeofday(&end, 0);
  #endif

  // software version host code
  #ifdef SW
    // to be implemented
    gettimeofday(&start, 0);
    face_detect_sw(Data, result_x, result_y, result_w, result_h, &res_size);
    gettimeofday(&end, 0);
  #endif
 
  // check results
  printf("Checking results:\n");
  check_results(res_size, result_x, result_y, result_w, result_h, Data, outFile);
    
  // print time
  long long elapsed = (end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec;   
  printf("elapsed time: %lld us\n", elapsed);

  // cleanup
  #ifdef OCL
    facedetect_world.releaseWorld();
  #endif

  return EXIT_SUCCESS;

}
