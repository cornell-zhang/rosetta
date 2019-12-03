/*===============================================================*/
/*                                                               */
/*                          digitrec.h                           */
/*                                                               */
/*             Hardware function for digit recognition           */
/*                                                               */
/*===============================================================*/

#include "typedefs.h"
void DigitRec(WholeDigitType global_training_set[MAX_NUM_TRAINING], 
              WholeDigitType global_test_set[MAX_NUM_TEST], 
              LabelType global_results[MAX_NUM_TEST], int num_training, int num_test); 
