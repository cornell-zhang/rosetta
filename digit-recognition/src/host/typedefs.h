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
const int NUM_TRAINING  = 18000;
const int CLASS_SIZE    = 1800;
const int NUM_TEST      = 2000;
const int DIGIT_WIDTH   = 4;

// typedefs
typedef unsigned long long DigitType;
typedef unsigned char      LabelType;

#ifdef OCL
  #include <string>
  // target device
  // change here to map to a different device
  const std::string TARGET_DEVICE = "xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0";
#endif

#ifdef SDSOC
  #include "ap_int.h"
  // sdsoc wide vector type
  typedef ap_uint<256>  WholeDigitType;
#endif

// parameters
#define K_CONST 3
#define PAR_FACTOR 40

#endif
