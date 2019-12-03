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
#include "utils.h"

int main(int argc, char ** argv) 
{
  printf("Digit Recognition Application\n");

  std::string training_data_path = "";
  std::string test_data_path = "";
  std::string label_path = "";
  parse_command_line_args(argc, argv, training_data_path, test_data_path, label_path); 

  // timers
  struct timeval start, end;

  // allocate space for hardware function
  WholeDigitType* training_in = (WholeDigitType*)calloc(MAX_NUM_TRAINING, sizeof(WholeDigitType));
  WholeDigitType* test_in     = (WholeDigitType*)calloc(MAX_NUM_TEST, sizeof(WholeDigitType));
  // create space for result
  LabelType* result = (LabelType*)calloc(MAX_NUM_TEST, sizeof(LabelType));
  LabelType* expected = (LabelType*)calloc(MAX_NUM_TEST, sizeof(LabelType));
  printf("Reading training data... \n");
  int num_train = read_data(training_data_path, training_in);
  printf("Reading test data... \n");
  int num_test = read_data(test_data_path, test_in, false);
  printf("Reading labels... \n");
  int num_labels = read_labels(label_path, expected);

  // run the hardware function
  // first call: transfer data
  printf("Running... \n");
  gettimeofday(&start, NULL);
  DigitRec(training_in, test_in, result, num_train, num_test);
  gettimeofday(&end, NULL);

  // check results
  printf("Checking results:\n");
  check_results( result, expected, num_test );
    
  // print time
  long long elapsed = (end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec;   
  printf("elapsed time: %lld us\n", elapsed);

  // cleanup
  free(training_in);
  free(test_in);
  free(result);
  free(expected);

  return EXIT_SUCCESS;
}
