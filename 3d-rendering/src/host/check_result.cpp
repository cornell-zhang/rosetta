/*===============================================================*/
/*                                                               */
/*                       check_result.cpp                        */
/*                                                               */
/*      Software evaluation of training and test error rate      */
/*                                                               */
/*===============================================================*/

#include <cstdio>
#include "typedefs.h"
#include <fstream>
#include <iostream>

#ifndef SW
void check_results(bit32* output)
#else
void check_results(bit8 output[MAX_X][MAX_Y])
#endif
{
  #ifndef SW
    bit8 frame_buffer_print[MAX_X][MAX_Y];
  
    // read result from the 32-bit output buffer
    for (int i = 0, j = 0, n = 0; n < NUM_FB; n ++ )
    {
      bit32 temp = output[n];
      frame_buffer_print[i][j++] = temp(7,0);
      frame_buffer_print[i][j++] = temp(15,8);
      frame_buffer_print[i][j++] = temp(23,16);
      frame_buffer_print[i][j++] = temp(31,24);
      if(j == MAX_Y)
      {
        i++;
        j = 0;
      }
    }
  #endif

  // print result
  std::ofstream ofile;
  ofile.open("outputs.txt");
  if (ofile.is_open())
  {
    ofile << "Image After Rendering: \n";
    for (int j = MAX_X - 1; j >= 0; j -- )
    {
      for (int i = 0; i < MAX_Y; i ++ )
      {
        int pix;
        #ifndef SW
          pix = frame_buffer_print[i][j].to_int();
        #else
          pix = output[i][j];
        #endif
        if (pix)
          ofile << "1";
        else
          ofile << "0";
      }
      ofile << std::endl;
    }
  }
  else
  {
    std::cout << "Failed to create output file!" << std::endl;
  }
}
