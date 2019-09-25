/*===============================================================*/
/*                                                               */
/*                        check_result.h                         */
/*                                                               */
/*      Compare result and expected label to get error rate      */
/*                                                               */
/*===============================================================*/

void check_results(int &result_size, 
                   int result_x[RESULT_SIZE], 
                   int result_y[RESULT_SIZE], 
                   int result_w[RESULT_SIZE],  
                   int result_h[RESULT_SIZE],
                   unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH],
                   std::string outFile);

