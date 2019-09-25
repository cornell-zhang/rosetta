/*===============================================================*/
/*                                                               */
/*                        typedefs.h                             */
/*                                                               */
/*        Defines types and constants for host function          */
/*                                                               */
/*===============================================================*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__
//#include "ap_fixed.h"
const int MAX_HEIGHT = 436;
const int MAX_WIDTH = 1024;

// basic typedefs
#include "ap_fixed.h"
typedef ap_fixed<18,9> input_t;
typedef ap_fixed<32,10> pixel_t;
typedef ap_fixed<32,10> outer_pixel_t;
typedef ap_fixed<64,20> calc_pixel_t;
typedef ap_fixed<32,10> vel_pixel_t;

typedef struct{
	pixel_t x;
	pixel_t y;
	pixel_t z;
}gradient_t;

typedef struct{
    outer_pixel_t val[6];
}outer_t; 

typedef struct{
    outer_pixel_t val[6];
}tensor_t;

typedef struct{
    vel_pixel_t x;
    vel_pixel_t y;
}velocity_t;

#include "ap_int.h"
// for data packing
typedef ap_uint<64> frames_t;

#endif
