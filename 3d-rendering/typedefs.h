/*===============================================================*/
/*                                                               */
/*                          typedefs.h                           */
/*                                                               */
/*                     Typedefs for the host                     */
/*                                                               */
/*===============================================================*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

// resolution 256x256
const int MAX_X = 256;
const int MAX_Y = 256;

// number of values in frame buffer: 32 bits
const int NUM_FB = MAX_X * MAX_Y / 4;
// dataset information 
const int NUM_3D_TRI = 3192;

// hls header
#include "ap_int.h"
// specialized datatypes
typedef ap_uint<1> bit1;
typedef ap_uint<2> bit2;
typedef ap_uint<8> bit8;
typedef ap_uint<16> bit16;
typedef ap_uint<32> bit32;

// struct: 3D triangle
typedef struct
{
  bit8   x0;
  bit8   y0;
  bit8   z0;
  bit8   x1;
  bit8   y1;
  bit8   z1;
  bit8   x2;
  bit8   y2;
  bit8   z2;
} Triangle_3D;

// struct: 2D triangle
typedef struct
{
  bit8   x0;
  bit8   y0;
  bit8   x1;
  bit8   y1;
  bit8   x2;
  bit8   y2;
  bit8   z;
} Triangle_2D;

// struct: candidate pixels
typedef struct
{
  bit8   x;
  bit8   y;
  bit8   z;
  bit8   color;
} CandidatePixel;

// struct: colored pixel
typedef struct
{
  bit8   x;
  bit8   y;
  bit8   color;
} Pixel;

// dataflow switch
#define ENABLE_DATAFLOW
#endif
