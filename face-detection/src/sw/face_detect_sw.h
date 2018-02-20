/*===============================================================*/
/*                                                               */
/*                       face_detect_sw.h                        */
/*                                                               */
/*             Software version for face detection.              */
/*                                                               */
/*===============================================================*/


#ifndef __FACE_DETECT_SW_H__
#define __FACE_DETECT_SW_H__

#include "../host/typedefs.h"

void face_detect_sw(unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH], 
                    int _result_x[RESULT_SIZE], 
                    int _result_y[RESULT_SIZE], 
                    int _result_w[RESULT_SIZE], 
                    int _result_h[RESULT_SIZE], 
                    int *result_size);

#endif
