/*===============================================================*/
/*                                                               */
/*                        spam_filter.cpp                        */
/*                                                               */
/*      Main host function for the Spam Filter application.      */
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
  #include "../sdsoc/sgd.h"
#endif

#ifdef SW
  # include "../sw/sgd_sw.h"
#endif

// other headers
#include "utils.h"
#include "typedefs.h"
#include "check_result.h"

int main(int argc, char *argv[]) 
{
  setbuf(stdout, NULL);

  printf("Spam Filter Application\n");

  // parse command line arguments
  std::string path_to_data("");
  // sdaccel version and sdsoc/sw version have different command line options
  #ifdef OCL
    std::string kernelFile("");
    parse_sdaccel_command_line_args(argc, argv, kernelFile, path_to_data);
  #else
    parse_sdsoc_command_line_args(argc, argv, path_to_data);
  #endif

  // allocate space
  // for software verification
  DataType*    data_points  = new DataType[DATA_SET_SIZE];
  LabelType*   labels       = new LabelType  [NUM_SAMPLES];
  FeatureType* param_vector = new FeatureType[NUM_FEATURES];

  // read in dataset
  std::string str_points_filepath = path_to_data + "/shuffledfeats.dat";
  std::string str_labels_filepath = path_to_data + "/shuffledlabels.dat";

  FILE* data_file;
  FILE* label_file;

  data_file = fopen(str_points_filepath.c_str(), "r");
  if (!data_file)
  {
    printf("Failed to open data file %s!\n", str_points_filepath.c_str());
    return EXIT_FAILURE;
  }
  for (int i = 0; i < DATA_SET_SIZE; i ++ )
  {
    float tmp;
    fscanf(data_file, "%f", &tmp);
    data_points[i] = tmp;
  }
  fclose(data_file);

  label_file = fopen(str_labels_filepath.c_str(), "r");
  if (!label_file)
  {
    printf("Failed to open label file %s!\n", str_labels_filepath.c_str());
    return EXIT_FAILURE;
  }
  for (int i = 0; i < NUM_SAMPLES; i ++ )
  {
    int tmp;
    fscanf(label_file, "%d", &tmp);
    labels[i] = tmp;
  }
  fclose(label_file);

  // reset parameter vector
  for (size_t i = 0; i < NUM_FEATURES; i++)
    param_vector[i] = 0;

  // timers
  struct timeval start, end;

  // opencl version host code
  #ifdef OCL
    // arrays for the accelerator
    VectorDataType* data_points_for_accel = new VectorDataType[NUM_TRAINING * NUM_FEATURES / D_VECTOR_SIZE];
    VectorLabelType* labels_for_accel = new VectorLabelType[NUM_TRAINING / L_VECTOR_SIZE];
    VectorFeatureType* param_for_accel = new VectorFeatureType[NUM_FEATURES / F_VECTOR_SIZE];
    
    // reorganize data for the accelerator
    // data points
    for (int i = 0; i < NUM_TRAINING * NUM_FEATURES / D_VECTOR_SIZE; i ++ )
      for (int j = 0; j < D_VECTOR_SIZE; j ++ )
        data_points_for_accel[i].range((j+1)*DTYPE_TWIDTH-1, j*DTYPE_TWIDTH) = data_points[i*D_VECTOR_SIZE+j].range(DTYPE_TWIDTH-1, 0);
  
    // labels
    for (int i = 0; i < NUM_TRAINING / L_VECTOR_SIZE; i ++ )
      for (int j = 0; j < L_VECTOR_SIZE; j ++ )
        labels_for_accel[i].range((j+1)*LTYPE_WIDTH-1, j*LTYPE_WIDTH) = labels[i*L_VECTOR_SIZE+j].range(LTYPE_WIDTH-1, 0);
  
    // parameter vector
    for (int i = 0; i < NUM_FEATURES / F_VECTOR_SIZE; i ++ )
      for (int j = 0; j < F_VECTOR_SIZE; j ++ )
        param_for_accel[i].range((j+1)*FTYPE_TWIDTH-1, j*FTYPE_TWIDTH) = param_vector[i*F_VECTOR_SIZE+j].range(FTYPE_TWIDTH-1, 0);
  
  
    // create OpenCL world
    CLWorld spam_filter_world = CLWorld(TARGET_DEVICE, CL_DEVICE_TYPE_ACCELERATOR);
  
    // add the bitstream file
    spam_filter_world.addProgram(kernelFile);
  
    // create kernels
    CLKernel SgdLR(spam_filter_world.getContext(), spam_filter_world.getProgram(), "SgdLR", spam_filter_world.getDevice());
  
    // create mem objects
    // use 4500 as training, 500 as test
    CLMemObj data_mem ( (void*)data_points_for_accel,  
                        sizeof(VectorDataType),
                        NUM_TRAINING * NUM_FEATURES / D_VECTOR_SIZE,
                        CL_MEM_READ_ONLY);
    CLMemObj label_mem( (void*)labels_for_accel,
                        sizeof(VectorLabelType),
                        NUM_TRAINING / L_VECTOR_SIZE,
                        CL_MEM_READ_ONLY);
    CLMemObj param_mem( (void*)param_for_accel,
                        sizeof(VectorFeatureType),
                        NUM_FEATURES / F_VECTOR_SIZE, 
                        CL_MEM_READ_WRITE);
  
    // start timer
    gettimeofday(&start, 0);
  
    // add them to the world
    // added in sequence, each of them can be referenced by an index
    spam_filter_world.addMemObj(data_mem);
    spam_filter_world.addMemObj(label_mem);
    spam_filter_world.addMemObj(param_mem);
  
    // set work size
    int global_size[3] = {1, 1, 1};
    int local_size[3] = {1, 1, 1};
    SgdLR.set_global(global_size);
    SgdLR.set_local(local_size);
  
    // add them to the world
    // same with the index here
    spam_filter_world.addKernel(SgdLR);
  
    // set kernel arguments
    spam_filter_world.setMemKernelArg(0, 0, 0);
    spam_filter_world.setMemKernelArg(0, 1, 1);
    spam_filter_world.setMemKernelArg(0, 2, 2);
  
    // run!
    spam_filter_world.runKernels();
  
    // read the param vector back
    spam_filter_world.readMemObj(2);
  
    // end timer
    gettimeofday(&end, 0);

    // reorganize the parameter vector
    for (int i = 0; i < NUM_FEATURES / F_VECTOR_SIZE; i ++ )
      for (int j = 0; j < F_VECTOR_SIZE; j ++ )
        param_vector[i*F_VECTOR_SIZE+j].range(FTYPE_TWIDTH-1, 0) = param_for_accel[i].range((j+1)*FTYPE_TWIDTH-1, j*FTYPE_TWIDTH);
  #endif    

  // sdsoc version host code
  #ifdef SDSOC
    // allocate space for accelerator
    VectorDataType* data_points_for_accel = (VectorDataType*)sds_alloc_non_cacheable(sizeof(VectorDataType) * NUM_TRAINING * NUM_FEATURES / D_VECTOR_SIZE);
    VectorLabelType* labels_for_accel = (VectorLabelType*)sds_alloc(sizeof(VectorLabelType) * NUM_TRAINING / L_VECTOR_SIZE);
    VectorFeatureType* param_for_accel = (VectorFeatureType*)sds_alloc(sizeof(VectorFeatureType) * NUM_FEATURES / F_VECTOR_SIZE);
 
    // reorganize data for the accelerator
    // data points
    for (int i = 0; i < NUM_TRAINING * NUM_FEATURES / D_VECTOR_SIZE; i ++ )
      for (int j = 0; j < D_VECTOR_SIZE; j ++ )
        data_points_for_accel[i].range((j+1)*DTYPE_TWIDTH-1, j*DTYPE_TWIDTH) = data_points[i*D_VECTOR_SIZE+j].range(DTYPE_TWIDTH-1, 0);
  
    // labels
    for (int i = 0; i < NUM_TRAINING / L_VECTOR_SIZE; i ++ )
      for (int j = 0; j < L_VECTOR_SIZE; j ++ )
        labels_for_accel[i].range((j+1)*LTYPE_WIDTH-1, j*LTYPE_WIDTH) = labels[i*L_VECTOR_SIZE+j].range(LTYPE_WIDTH-1, 0);
  
    // parameter vector
    for (int i = 0; i < NUM_FEATURES / F_VECTOR_SIZE; i ++ )
      for (int j = 0; j < F_VECTOR_SIZE; j ++ )
        param_for_accel[i].range((j+1)*FTYPE_TWIDTH-1, j*FTYPE_TWIDTH) = param_vector[i*F_VECTOR_SIZE+j].range(FTYPE_TWIDTH-1, 0);
  
    // run the accelerator
    gettimeofday(&start, NULL);
    for (int epoch = 0; epoch < NUM_EPOCHS; epoch ++ )
    {
      printf("epoch %d...\n", epoch);
      SgdLR(data_points_for_accel, labels_for_accel, param_for_accel, (epoch == 0), (epoch == 4));
    }
    gettimeofday(&end, NULL);
  
    // reorganize the parameter vector
    for (int i = 0; i < NUM_FEATURES / F_VECTOR_SIZE; i ++ )
      for (int j = 0; j < F_VECTOR_SIZE; j ++ )
        param_vector[i*F_VECTOR_SIZE+j].range(FTYPE_TWIDTH-1, 0) = param_for_accel[i].range((j+1)*FTYPE_TWIDTH-1, j*FTYPE_TWIDTH);
  #endif

  // sw version host code
  #ifdef SW
    gettimeofday(&start, NULL);
    SgdLR_sw(data_points, labels, param_vector);
    gettimeofday(&end, NULL);
  #endif 

  // check results
  printf("Checking results:\n");
  check_results( param_vector, data_points, labels );
    
  // print time
  long long elapsed = (end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec;   
  printf("elapsed time: %lld us\n", elapsed);

  // cleanup
  #ifdef OCL
    spam_filter_world.releaseWorld();
    delete []data_points_for_accel;
    delete []labels_for_accel;
    delete []param_for_accel;
  #endif

  #ifdef SDSOC
    sds_free(data_points_for_accel);
    sds_free(labels_for_accel);
    sds_free(param_for_accel);
  #endif

  delete []data_points;
  delete []labels;
  delete []param_vector;

  return EXIT_SUCCESS;
}
