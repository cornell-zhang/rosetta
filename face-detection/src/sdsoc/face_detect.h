/*===============================================================*/
/*                                                               */
/*                       face_detect.h                           */
/*                                                               */
/*     Hardware function for the Face Detection application.     */
/*                                                               */
/*===============================================================*/


#include "../host/typedefs.h"

void face_detect( unsigned char Data[IMAGE_WIDTH], 
                  int _result_x[RESULT_SIZE], 
                  int _result_y[RESULT_SIZE], 
                  int _result_w[RESULT_SIZE], 
                  int _result_h[RESULT_SIZE], 
                  int *result_size);

