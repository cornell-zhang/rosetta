/*===============================================================*/
/*                                                               */
/*                        check_result.h                         */
/*                                                               */
/*      Compare result and expected label to get error rate      */
/*                                                               */
/*===============================================================*/


#ifdef OCL
void check_results(velocity_t output[MAX_HEIGHT * MAX_WIDTH], CFloatImage refFlow, std::string outFile);
#else
void check_results(velocity_t output[MAX_HEIGHT][MAX_WIDTH], CFloatImage refFlow, std::string outFile);
#endif
