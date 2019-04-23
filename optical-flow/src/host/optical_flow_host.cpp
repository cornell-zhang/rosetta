/*===============================================================*/
/*                                                               */
/*                    optical_flow_host.cpp                      */
/*                                                               */
/*      Main host function for the Optical Flow application.     */
/*                                                               */
/*===============================================================*/

// standard C/C++ headers
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <string>
#include <time.h>
#include <sys/time.h>

// other headers
#include "utils.h"
#include "typedefs.h"
#include "check_result.h"

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
  //#include "sds_lib.h"
  // hardware function declaration
  #include "../sdsoc/optical_flow.h"
#endif
#ifdef SW
  # include "../sw/optical_flow_sw.h"
#endif



int main(int argc, char ** argv) 
{
  printf("Optical Flow Application\n");

  // parse command line arguments
  std::string dataPath("");
  std::string outFile("");
  #ifdef OCL
    // parse command line arguments for opencl version
    std::string kernelFile("");
    parse_sdaccel_command_line_args(argc, argv, kernelFile, dataPath, outFile);
  #else
    // for sw and sdsoc versions
    parse_sdsoc_command_line_args(argc, argv, dataPath, outFile);
  #endif

  // create actual file names according to the datapath
  std::string frame_files[5];
  std::string reference_file;
  frame_files[0] = dataPath + "/frame1.ppm";
  frame_files[1] = dataPath + "/frame2.ppm";
  frame_files[2] = dataPath + "/frame3.ppm";
  frame_files[3] = dataPath + "/frame4.ppm";
  frame_files[4] = dataPath + "/frame5.ppm";
  reference_file = dataPath + "/ref.flo";

  // read in images and convert to grayscale
  printf("Reading input files ... \n");

  CByteImage imgs[5];
  for (int i = 0; i < 5; i++) 
  {
    CByteImage tmpImg;
    ReadImage(tmpImg, frame_files[i].c_str());
    imgs[i] = ConvertToGray(tmpImg);
  }

  // read in reference flow file
  printf("Reading reference output flow... \n");

  CFloatImage refFlow;
  ReadFlowFile(refFlow, reference_file.c_str());

  // timers
  struct timeval start, end;

  // opencl version host code
  #ifdef OCL
    // arrays for compute
    // inputs
    frames_t*  frames = new frames_t[MAX_HEIGHT * MAX_WIDTH];
    // output
    velocity_t* outputs = new velocity_t[MAX_HEIGHT * MAX_WIDTH];
   
    // pack the values
    for (int i = 0; i < MAX_HEIGHT; i++) 
    {
      for (int j = 0; j < MAX_WIDTH; j++) 
      {
        frames[i*MAX_WIDTH+j](7 ,  0) = imgs[0].Pixel(j, i, 0);
        frames[i*MAX_WIDTH+j](15,  8) = imgs[1].Pixel(j, i, 0);
        frames[i*MAX_WIDTH+j](23, 16) = imgs[2].Pixel(j, i, 0);
        frames[i*MAX_WIDTH+j](31, 24) = imgs[3].Pixel(j, i, 0);
        frames[i*MAX_WIDTH+j](39, 32) = imgs[4].Pixel(j, i, 0);
        frames[i*MAX_WIDTH+j](63, 40) = 0;  
      }
    }
  
    // create OpenCL world
    CLWorld oflow_world = CLWorld(TARGET_DEVICE, CL_DEVICE_TYPE_ACCELERATOR);
  
    // add the bitstream file
    oflow_world.addProgram(kernelFile);
  
    // create kernels
    // this kernel is written in C++
    CLKernel OpticalFlow(oflow_world.getContext(), oflow_world.getProgram(), "optical_flow", oflow_world.getDevice());
  
    // create mem objects
    CLMemObj frames_mem ( (void*)frames,  sizeof(frames_t),   MAX_HEIGHT * MAX_WIDTH, CL_MEM_READ_ONLY, 0);
    CLMemObj outputs_mem( (void*)outputs, sizeof(velocity_t), MAX_HEIGHT * MAX_WIDTH, CL_MEM_WRITE_ONLY, 0);
  
    // start timer
    gettimeofday(&start, 0);
  
    // add them to the world
    // added in sequence, each of them can be referenced by an index
    oflow_world.addMemObj(frames_mem);
    oflow_world.addMemObj(outputs_mem);
  
    // set work size
    int global_size[3] = {1, 1, 1};
    int local_size[3] = {1, 1, 1};
    OpticalFlow.set_global(global_size);
    OpticalFlow.set_local(local_size);
  
    // add them to the world
    oflow_world.addKernel(OpticalFlow);
  
    // set kernel arguments
    oflow_world.setMemKernelArg(0, 0, 0);
    oflow_world.setMemKernelArg(0, 1, 1);
   
    // run!
    oflow_world.runKernels();
  
    // read the data back
    oflow_world.readMemObj(1);
  
    // end timer
    gettimeofday(&end, 0);
  #endif

  // sdsoc version host code
  #ifdef SDSOC
    // input and output buffers
    static frames_t frames[MAX_HEIGHT][MAX_WIDTH];
    static velocity_t outputs[MAX_HEIGHT][MAX_WIDTH];

    // pack the values
    for (int i = 0; i < MAX_HEIGHT; i++) 
    {
      for (int j = 0; j < MAX_WIDTH; j++) 
      {
        frames[i][j](7 ,  0) = imgs[0].Pixel(j, i, 0);
        frames[i][j](15,  8) = imgs[1].Pixel(j, i, 0);
        frames[i][j](23, 16) = imgs[2].Pixel(j, i, 0);
        frames[i][j](31, 24) = imgs[3].Pixel(j, i, 0);
        frames[i][j](39, 32) = imgs[4].Pixel(j, i, 0);
        frames[i][j](63, 40) = 0;  
      }
    }
    printf("Start!\n");

    // run
    gettimeofday(&start, NULL);
    optical_flow(frames, outputs);
    gettimeofday(&end, NULL);
  #endif

  // sw version host code
  #ifdef SW
    static pixel_t frames[5][MAX_HEIGHT][MAX_WIDTH];
    static velocity_t outputs[MAX_HEIGHT][MAX_WIDTH];

    gettimeofday(&start, NULL);
    // use native C datatype arrays
    for (int f = 0; f < 5; f ++ )
      for (int i = 0; i < MAX_HEIGHT; i ++ )
        for (int j = 0; j < MAX_WIDTH; j ++ )
          frames[f][i][j] = imgs[f].Pixel(j, i, 0) / 255.0f;

    // run
    optical_flow_sw(frames[0], frames[1], frames[2], frames[3], frames[4], outputs);
    gettimeofday(&end, NULL);
  #endif

  // check results
  printf("Checking results:\n");

  check_results(outputs, refFlow, outFile);

  // print time
  long long elapsed = (end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec;   
  printf("elapsed time: %lld us\n", elapsed);

  // cleanup
  #ifdef OCL
    oflow_world.releaseWorld();
    delete []frames;
    delete []outputs; 
  #endif

  return EXIT_SUCCESS;

}
