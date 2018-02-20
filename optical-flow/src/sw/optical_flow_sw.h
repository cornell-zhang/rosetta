/*===============================================================*/
/*                                                               */
/*                      optical_flow_sw.h                        */
/*                                                               */
/*              Software version for optical flow.               */
/*                                                               */
/*===============================================================*/

#ifndef __OPTICAL_FLOW_H__
#define __OPTICAL_FLOW_H__

#include "../host/typedefs.h"

// convolution filters
const int GRAD_WEIGHTS[] =  {1,-8,0,8,-1};
const pixel_t GRAD_FILTER[] = {0.0755, 0.133, 0.1869, 0.2903, 0.1869, 0.133, 0.0755};
const pixel_t TENSOR_FILTER[] = {0.3243, 0.3513, 0.3243};

// top-level function
void optical_flow_sw(pixel_t frame0[MAX_HEIGHT][MAX_WIDTH],
                     pixel_t frame1[MAX_HEIGHT][MAX_WIDTH],
                     pixel_t frame2[MAX_HEIGHT][MAX_WIDTH],
                     pixel_t frame3[MAX_HEIGHT][MAX_WIDTH],
                     pixel_t frame4[MAX_HEIGHT][MAX_WIDTH],
                     velocity_t outputs[MAX_HEIGHT][MAX_WIDTH]);
#endif
