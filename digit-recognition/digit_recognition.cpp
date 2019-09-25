/*===============================================================*/
/*                                                               */
/*                    digit_recognition.cpp                      */
/*                                                               */
/*   Main host function for the Digit Recognition application.   */
/*                                                               */
/*===============================================================*/

// standard C/C++ headers
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <string>
#include <time.h>
#include <sys/time.h>

// hardware function declaration
#include "digitrec.h"

// other headers
#include "typedefs.h"
#include "check_result.h"

// data
#include "training_data.h"
#include "testing_data.h"

int main(int argc, char ** argv) 
{
  printf("Digit Recognition Application\n");

  // for this benchmark, data is already included in arrays:
  //   training_data: contains 18000 training samples, with 1800 samples for each digit class
  //   testing_data:  contains 2000 test samples
  //   expected:      contains labels for the test samples

  // timers
  struct timeval start, end;

  // allocate space for hardware function
  WholeDigitType* training_in0 = (WholeDigitType*)malloc(sizeof(WholeDigitType) * NUM_TRAINING / 2);
  WholeDigitType* training_in1 = (WholeDigitType*)malloc(sizeof(WholeDigitType) * NUM_TRAINING / 2);
  WholeDigitType* test_in      = (WholeDigitType*)malloc(sizeof(WholeDigitType) * NUM_TEST);

  // pack the data into a wide datatype
  for (int i = 0; i < NUM_TRAINING / 2; i ++ )
  {
    training_in0[i].range(63 , 0  ) = training_data[i*DIGIT_WIDTH+0];
    training_in0[i].range(127, 64 ) = training_data[i*DIGIT_WIDTH+1];
    training_in0[i].range(191, 128) = training_data[i*DIGIT_WIDTH+2];
    training_in0[i].range(255, 192) = training_data[i*DIGIT_WIDTH+3];
  }
  for (int i = 0; i < NUM_TRAINING / 2; i ++ )
  {
    training_in1[i].range(63 , 0  ) = training_data[(NUM_TRAINING / 2 + i)*DIGIT_WIDTH+0];
    training_in1[i].range(127, 64 ) = training_data[(NUM_TRAINING / 2 + i)*DIGIT_WIDTH+1];
    training_in1[i].range(191, 128) = training_data[(NUM_TRAINING / 2 + i)*DIGIT_WIDTH+2];
    training_in1[i].range(255, 192) = training_data[(NUM_TRAINING / 2 + i)*DIGIT_WIDTH+3];
  }

  for (int i = 0; i < NUM_TEST; i ++ )
  {
    test_in[i].range(63 , 0  ) = testing_data[i*DIGIT_WIDTH+0];
    test_in[i].range(127, 64 ) = testing_data[i*DIGIT_WIDTH+1];
    test_in[i].range(191, 128) = testing_data[i*DIGIT_WIDTH+2];
    test_in[i].range(255, 192) = testing_data[i*DIGIT_WIDTH+3];
  }

  // create space for result
  LabelType* result = (LabelType*)malloc(sizeof(LabelType) * NUM_TEST);

  // run the hardware function
  // first call: transfer data
  DigitRec(training_in0, test_in, result, 0);

  // second call: execute
  gettimeofday(&start, NULL);
  DigitRec(training_in1, test_in, result, 1);
  gettimeofday(&end, NULL);

  // check results
  printf("Checking results:\n");
  check_results( result, expected, NUM_TEST );
    
  // print time
  long long elapsed = (end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec;   
  printf("elapsed time: %lld us\n", elapsed);

  // cleanup
  free(training_in0);
  free(training_in1);
  free(test_in);
  free(result);

  return EXIT_SUCCESS;
}
