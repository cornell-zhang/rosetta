/*===============================================================*/
/*                                                               */
/*                          digitrec.h                           */
/*                                                               */
/*             Hardware function for digit recognition           */
/*                                                               */
/*===============================================================*/

#include "typedefs.h"

void DigitRec(WholeDigitType global_training_set[NUM_TRAINING / 2], WholeDigitType global_test_set[NUM_TEST], LabelType global_results[NUM_TEST], int run);

