/*===============================================================*/
/*                                                               */
/*                       check_result.cpp                        */
/*                                                               */
/*      Software evaluation of training and test error rate      */
/*                                                               */
/*===============================================================*/

#include <cstdio>
#include <string>
#include <cmath>
#include <fstream>

#include "typedefs.h"
#include "../../imageLib/imageLib.h"

#ifdef OCL
void check_results(velocity_t output[MAX_HEIGHT * MAX_WIDTH], CFloatImage refFlow, std::string outFile)
#else
void check_results(velocity_t output[MAX_HEIGHT][MAX_WIDTH], CFloatImage refFlow, std::string outFile)
#endif
{
  // copy the output into the float image
  CFloatImage outFlow(MAX_WIDTH, MAX_HEIGHT, 2);
  for (int i = 0; i < MAX_HEIGHT; i++) 
  {
    for (int j = 0; j < MAX_WIDTH; j++) 
    {
      #ifdef OCL
        double out_x = output[i * MAX_WIDTH + j].x.to_double();
        double out_y = output[i * MAX_WIDTH + j].y.to_double();
      #endif
      #ifdef SDSOC
        double out_x = output[i][j].x.to_double();
        double out_y = output[i][j].y.to_double();
      #endif
      #ifdef SW
        double out_x = output[i][j].x;
        double out_y = output[i][j].y;
      #endif

      if (out_x * out_x + out_y * out_y > 25.0) 
      {
        outFlow.Pixel(j, i, 0) = 1e10;
        outFlow.Pixel(j, i, 1) = 1e10;
      } 
      else 
      {
        outFlow.Pixel(j, i, 0) = out_x;
        outFlow.Pixel(j, i, 1) = out_y;
      }
    }
  }

  WriteFlowFile(outFlow, outFile.c_str());

  double accum_error = 0;
  int num_pix = 0;
  for (int i = 0; i < MAX_HEIGHT; i++) 
  {
    for (int j = 0; j < MAX_WIDTH; j++) 
    {
      double out_x = outFlow.Pixel(j, i, 0);
      double out_y = outFlow.Pixel(j, i, 1);

      if (unknown_flow(out_x, out_y)) continue;

      double out_deg = atan2(-out_y, -out_x) * 180.0 / M_PI;
      double ref_x = refFlow.Pixel(j, i, 0);
      double ref_y = refFlow.Pixel(j, i, 1);
      double ref_deg = atan2(-ref_y, -ref_x) * 180.0 / M_PI;

      // Normalize error to [-180, 180]
      double error = out_deg - ref_deg;
      while (error < -180) error += 360;
      while (error > 180) error -= 360;

      accum_error += fabs(error);
      num_pix++;
    }
  }

  double avg_error = accum_error / num_pix;
  std::ofstream ofile;
  ofile.open("output.txt");
  if (ofile.is_open())
  {
    ofile << "Average error: " << avg_error << " degrees" << std::endl;
    ofile.close();
  }
  else
  {
    std::cout << "Failed to create output file!" << std::endl;
  }

}
