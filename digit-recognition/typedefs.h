/*===============================================================*/
/*                                                               */
/*                          typedefs.h                           */
/*                                                               */
/*           Constant definitions and typedefs for host.         */
/*                                                               */
/*===============================================================*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

// dataset information
const int MAX_NUM_TRAINING  = 18000;
const int MAX_NUM_TEST      = 2000;
const int DIGIT_WIDTH       = 4;

// typedefs
typedef unsigned long long DigitType;
typedef unsigned char      LabelType;

#include "ap_int.h"
// wide vector type
typedef ap_uint<256>  WholeDigitType;

// parameters
#define K_CONST 3
#define PAR_FACTOR 10

#endif
