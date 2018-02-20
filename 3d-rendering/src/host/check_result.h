/*===============================================================*/
/*                                                               */
/*                        check_result.h                         */
/*                                                               */
/*      Compare result and expected label to get error rate      */
/*                                                               */
/*===============================================================*/

#ifndef SW
void check_results(bit32* output);
#else
void check_results(bit8 output[MAX_X][MAX_Y]);
#endif
