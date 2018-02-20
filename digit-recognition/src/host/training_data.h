/*===============================================================*/
/*                                                               */
/*                       training_data.h                         */
/*                                                               */
/*              Constant array for training instances.           */
/*                                                               */
/*===============================================================*/


#ifndef TRAINING_DATA_H
#define TRAINING_DATA_H


const DigitType training_data[NUM_TRAINING * DIGIT_WIDTH] = {
  #include "../../196data/training_set_0.dat" 
  #include "../../196data/training_set_1.dat" 
  #include "../../196data/training_set_2.dat" 
  #include "../../196data/training_set_3.dat" 
  #include "../../196data/training_set_4.dat" 
  #include "../../196data/training_set_5.dat" 
  #include "../../196data/training_set_6.dat" 
  #include "../../196data/training_set_7.dat" 
  #include "../../196data/training_set_8.dat" 
  #include "../../196data/training_set_9.dat"
};

#endif
