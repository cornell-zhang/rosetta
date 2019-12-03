/*===============================================================*/
/*                                                               */
/*                        rendering.cpp                          */
/*                                                               */
/*                 C++ kernel for 3D Rendering                   */
/*                                                               */
/*===============================================================*/

#include <hls_stream.h>
#include <ap_int.h>

using namespace hls;

#define USE_DATAFLOW

// resolution 256x256
const int MAX_X = 256;
const int MAX_Y = 256;

// number of values in frame buffer: 32 bits
const int NUM_FB = MAX_X * MAX_Y / 4;
const int MAX_NUM_TRIS = 4000;

typedef ap_uint<1> bit1;
typedef ap_uint<2> bit2;
typedef ap_uint<8> bit8;
typedef ap_uint<16> bit16;
typedef ap_int<16> sbit16;
typedef ap_uint<24> bit24;
typedef ap_uint<32> bit32;
typedef ap_uint<40> bit40;
typedef ap_uint<56> bit56;
typedef ap_uint<72> bit72;

/*======================PROCESSING STAGES========================*/

// project a 3D triangle to a 2D triangle
void projection ( bit32* input, stream<bit56 > & Triangle_2D_fifo, bit2 angle, int num_3d_triangles )
{
#pragma FLASH EXEC_CYCLE
#pragma HLS INLINE off
	for (int i = 0; i < num_3d_triangles; i++)
	{
#pragma HLS pipeline II=1
		bit32 input_lo;
		input_lo  = input[i*3];
		bit32 input_mi;
		input_mi  = input[i*3+1];
		bit32 input_hi;
		input_hi  = input[i*3+2];

		bit56 Triangle_2D_temp;
		bit72 Triangle_3D_temp;

		Triangle_3D_temp(7,0) = input_lo( 7,  0);
		Triangle_3D_temp(15,8) = input_lo(15,  8);
		Triangle_3D_temp(23,16) = input_lo(23, 16);
		Triangle_3D_temp(31,24) = input_lo(31, 24);
		Triangle_3D_temp(39,32) = input_mi( 7,  0);
		Triangle_3D_temp(47,40) = input_mi(15,  8);
		Triangle_3D_temp(55,48) = input_mi(23, 16);
		Triangle_3D_temp(63,56) = input_mi(31, 24);
		Triangle_3D_temp(71,64) = input_hi( 7,  0);

		// Setting camera to (0,0,-1), the canvas at z=0 plane
		// The 3D model lies in z>0 space
		// The coordinate on canvas is proportional to the corresponding coordinate 
		// on space
		if(angle == 0)
		{
			Triangle_2D_temp(7,0) = Triangle_3D_temp(7,0);
			Triangle_2D_temp(15,8) = Triangle_3D_temp(15,8);
			Triangle_2D_temp(23,16) = Triangle_3D_temp(31,24);
			Triangle_2D_temp(31,24) = Triangle_3D_temp(39,32);
			Triangle_2D_temp(39,32) = Triangle_3D_temp(55,48);
			Triangle_2D_temp(47,40) = Triangle_3D_temp(63,56);
			Triangle_2D_temp(55,48)  = (Triangle_3D_temp(23,16) / 3) + (Triangle_3D_temp(47,40) / 3) + (Triangle_3D_temp(71,64) / 3);
		}
		else{ 
			if(angle == 1)
			{
				Triangle_2D_temp(7,0) = Triangle_3D_temp(7,0);
				Triangle_2D_temp(15,8) = Triangle_3D_temp(23,16);
				Triangle_2D_temp(23,16) = Triangle_3D_temp(31,24);
				Triangle_2D_temp(31,24) = Triangle_3D_temp(47,40);
				Triangle_2D_temp(39,32) = Triangle_3D_temp(55,48);
				Triangle_2D_temp(47,40) = Triangle_3D_temp(71,64);
				Triangle_2D_temp(55,48)  = (Triangle_3D_temp(15,8) / 3) + (Triangle_3D_temp(39,32) / 3) + (Triangle_3D_temp(63,56) / 3);
			}
			else{ 
				if(angle == 2)
				{
					Triangle_2D_temp(7,0) = Triangle_3D_temp(23,16);
					Triangle_2D_temp(15,8) = Triangle_3D_temp(15,8);
					Triangle_2D_temp(23,16) = Triangle_3D_temp(47,40);
					Triangle_2D_temp(31,24) = Triangle_3D_temp(39,32);
					Triangle_2D_temp(39,32) = Triangle_3D_temp(71,64);
					Triangle_2D_temp(47,40) = Triangle_3D_temp(63,56);
					Triangle_2D_temp(55,48)  = (Triangle_3D_temp(7,0) / 3) + (Triangle_3D_temp(31,24) / 3) + (Triangle_3D_temp(55,48) / 3);
				}
			}
		}
		Triangle_2D_fifo.write(Triangle_2D_temp);
	}
}



// calculate bounding box for a 2D triangle
void rasterization1 ( stream<bit56 > & Triangle_2D_fifo, stream<bit40 > & max_min_fifo, stream<bit56 > & triangle_2d_same, stream<bit16 > & max_index, int num_3d_triangles)
{
#pragma FLASH EXEC_CYCLE
#pragma HLS INLINE off
	bit8 max_min[5];

	for (int i = 0; i < num_3d_triangles; i++)
	{
#pragma HLS pipeline II=1
		bit56 Triangle_2D_temp;
		Triangle_2D_temp = Triangle_2D_fifo.read();

		// Determine whether three vertices of a trianlgLe
		// (x0,y0) (x1,y1) (x2,y2) are in clockwise order by Pineda algorithm
		// if so, return a number > 0
		// else if three points are in line, return a number == 0
		// else in counterclockwise order, return a number < 0
		bit8 x0;
		x0 =	Triangle_2D_temp(7,0);
		bit8 y0;
		y0 =	Triangle_2D_temp(15,8);
		bit8 x1;
		x1 =	Triangle_2D_temp(23,16);
		bit8 y1;
		y1 =	Triangle_2D_temp(31,24);
		bit8 x2;
		x2 =	Triangle_2D_temp(39,32);
		bit8 y2;
		y2 =	Triangle_2D_temp(47,40);
		bit8 z;
		z  =	Triangle_2D_temp(55,48);
		sbit16 cw1;
		cw1 = x2 - x0;
		sbit16 cw2;
		cw2 = y1 - y0;
		sbit16 cw3;
		cw3 = y2 - y0;
		sbit16 cw4;
		cw4 = x1 - x0;
		sbit16 cw;
		cw = cw1 * cw2 - cw3 * cw4;

		// clockwise the vertices of input 2d triangle
		bit16 max_index_temp;
		bit40 max_min_temp;
		bit56 triangle_2d_same_temp;

		if ( cw == 0 ){
			max_min_temp = 0;
			max_index_temp = 0;
			triangle_2d_same_temp = 0;
		}
		else{
			if ( cw < 0 ){
				// swap (x0, y0) (x1, y1) of a bit56

				bit8 tmp_x, tmp_y;

				tmp_x = Triangle_2D_temp(7,0);
				tmp_y = Triangle_2D_temp(15,8);

				Triangle_2D_temp(7,0) = Triangle_2D_temp(23,16);
				Triangle_2D_temp(15,8) = Triangle_2D_temp(31,24);

				Triangle_2D_temp(23,16) = tmp_x;
				Triangle_2D_temp(31,24) = tmp_y;
			}

			// copy the same 2D triangle
			triangle_2d_same_temp(7,0) = Triangle_2D_temp(7,0);
			triangle_2d_same_temp(15,8) = Triangle_2D_temp(15,8);
			triangle_2d_same_temp(23,16) = Triangle_2D_temp(23,16);
			triangle_2d_same_temp(31,24) = Triangle_2D_temp(31,24);
			triangle_2d_same_temp(39,32) = Triangle_2D_temp(39,32);
			triangle_2d_same_temp(47,40) = Triangle_2D_temp(47,40);
			triangle_2d_same_temp(55,48) = Triangle_2D_temp(55,48);

			// find the rectangle bounds of 2D triangles
			if (Triangle_2D_temp(7,0) < Triangle_2D_temp(23,16))
			{
				if (Triangle_2D_temp(7,0) < Triangle_2D_temp(39,32)){
					max_min[0] = Triangle_2D_temp(7,0);
				}
				else{
					max_min[0] = Triangle_2D_temp(39,32);
				}
			}
			else
			{
				if (Triangle_2D_temp(23,16) < Triangle_2D_temp(39,32)){
					max_min[0] = Triangle_2D_temp(23,16);
				}
				else{
					max_min[0] = Triangle_2D_temp(39,32);
				}
			}
			if (Triangle_2D_temp(7,0) > Triangle_2D_temp(23,16))
			{
				if (Triangle_2D_temp(7,0) > Triangle_2D_temp(39,32)){
					max_min[1] = Triangle_2D_temp(7,0);
				}
				else{
					max_min[1] = Triangle_2D_temp(39,32);
				}
			}
			else
			{
				if (Triangle_2D_temp(23,16) > Triangle_2D_temp(39,32)){
					max_min[1] = Triangle_2D_temp(23,16);
				}
				else{
					max_min[1] = Triangle_2D_temp(39,32);
				}
			}
			if (Triangle_2D_temp(15,8) < Triangle_2D_temp(31,24))
			{
				if (Triangle_2D_temp(15,8) < Triangle_2D_temp(47,40)){
					max_min[2] = Triangle_2D_temp(15,8);
				}
				else{
					max_min[2] = Triangle_2D_temp(47,40);
				}
			}
			else
			{
				if (Triangle_2D_temp(31,24) < Triangle_2D_temp(47,40)){
					max_min[2] = Triangle_2D_temp(31,24);
				}
				else{
					max_min[2] = Triangle_2D_temp(47,40);
				}
			}
			if (Triangle_2D_temp(15,8) > Triangle_2D_temp(31,24))
			{
				if (Triangle_2D_temp(15,8) > Triangle_2D_temp(47,40)){
					max_min[3] = Triangle_2D_temp(15,8);
				}
				else{
					max_min[3] = Triangle_2D_temp(47,40);
				}
			}
			else
			{
				if (Triangle_2D_temp(31,24) > Triangle_2D_temp(47,40)){
					max_min[3] = Triangle_2D_temp(31,24);
				}
				else{
					max_min[3] = Triangle_2D_temp(47,40);
				}
			}

			max_min[4] = max_min[1] - max_min[0];

			// calculate index for searching pixels
			max_index_temp = (max_min[1] - max_min[0]) * (max_min[3] - max_min[2]);

			max_min_temp(7,0) = max_min[0];
			max_min_temp(15,8) = max_min[1];
			max_min_temp(23,16) = max_min[2];
			max_min_temp(31,24) = max_min[3];
			max_min_temp(39,32) = max_min[4];
		}

		max_index.write(max_index_temp);
		max_min_fifo.write(max_min_temp);
		triangle_2d_same.write(triangle_2d_same_temp);
	}
}

// find pixels in the triangles from the bounding box
void rasterization2 ( stream<bit16 > & size, stream<bit40 > & max_min_fifo, stream<bit16 > & max_index, stream<bit56 > & triangle_2d_same, stream<bit32 > & fragment2, int num_3d_triangles )
{
#pragma FLASH EXEC_CYCLE
#pragma HLS INLINE off
	bit8 max_min[5];

	for (int i = 0; i < num_3d_triangles; i++)
	{
		// clockwise the vertices of input 2d triangle
		bit16 max_index_temp;
		max_index_temp = max_index.read();

		bit40 max_min_temp;
		max_min_temp = max_min_fifo.read();
		max_min[0] = max_min_temp(7,0);
		max_min[1] = max_min_temp(15,8);
		max_min[2] = max_min_temp(23,16);
		max_min[3] = max_min_temp(31,24);
		max_min[4] = max_min_temp(39,32);

		bit56 triangle_2d_same_temp;
		triangle_2d_same_temp = triangle_2d_same.read();

		bit8 color;
		color = 100;
		bit16 cnt;
		cnt = 0;

		bit16 k; 
		for ( k = 0; k < max_index_temp; k++ )
		{
#pragma HLS PIPELINE II=1
			bit8 x;
			x = max_min[0] + (k%max_min[4]);
			bit8 y;
			y = max_min[2] + (k/max_min[4]);

			// Given a bit24, determine whether it is inside the triangle
			// by Pineda algorithm
			// if so, return true
			// else, return false

		bit8 x0;
		x0 =	triangle_2d_same_temp(7,0);
		bit8 y0;
		y0 =	triangle_2d_same_temp(15,8);
		bit8 x1;
		x1 =	triangle_2d_same_temp(23,16);
		bit8 y1;
		y1 =	triangle_2d_same_temp(31,24);
		bit8 x2;
		x2 =	triangle_2d_same_temp(39,32);
		bit8 y2;
		y2 =	triangle_2d_same_temp(47,40);
		bit8 z;
		z  =	triangle_2d_same_temp(55,48);

		sbit16 pix0;
		pix0 = x - x0;
		sbit16 pix1;
		pix1 = x - x1;
		sbit16 pix2;
		pix2 = x - x2;
		sbit16 pix10;
		pix10 = x1 - x0;
		sbit16 pix21;
		pix21 = x2 - x1;
		sbit16 pix02;
		pix02 = x0 - x2;
		sbit16 piy0;
		piy0 = y - y0;
		sbit16 piy1;
		piy1 = y - y1;
		sbit16 piy2;
		piy2 = y - y2;
		sbit16 piy10;
		piy10 = y1 - y0;
		sbit16 piy21;
		piy21 = y2 - y1;
		sbit16 piy02;
		piy02 = y0 - y2;

  sbit16 pi0;
  pi0 = pix0 * piy10 - piy0 * pix10;
  sbit16 pi1;
  pi1 = pix1 * piy21 - piy1 * pix21;
  sbit16 pi2;
  pi2 = pix2 * piy02 - piy2 * pix02;

			if(pi0 >= 0 && pi1 >= 0 && pi2 >= 0)
			{
				bit32 fragment_temp;
				fragment_temp(7,0) = x;
				fragment_temp(15,8) = y;
				fragment_temp(23,16) = triangle_2d_same_temp(55,48);
				fragment_temp(31,24) = color;
				fragment2.write(fragment_temp);
				cnt++;
			}
		}

		size.write(cnt);
	}
}

// filter hidden pixels
void zculling ( stream<bit16 > & size_pixels, stream<bit32 > & fragments, stream<bit16 > & size, stream<bit24 > & pixels,
                int num_3d_triangles )
{
#pragma FLASH EXEC_CYCLE
#pragma HLS INLINE off
	// initilize the z-buffer in rendering first triangle for an image
	bit8 z_buffer[MAX_X*MAX_Y];

	for ( int i = 0; i < MAX_X; i++)
	{
		for ( int j = 0; j < MAX_Y; j++)
		{
#pragma HLS PIPELINE II=1
			z_buffer[i*MAX_Y+j] = 255;
		}
	}

	bit16 counter;
	for (counter = 0; counter < num_3d_triangles; counter++)
	{
		bit16 size_temp;
		size_temp = size.read();
		// bit24 counter
		bit16 pixel_cntr;
		pixel_cntr = 0;

		// update z-buffer and pixels
		bit16 n = 0;
		for ( n = 0; n < size_temp; n++ )
		{
#pragma HLS PIPELINE II=1
			bit32 fragments_temp;
			fragments_temp = fragments.read();
			int zbuf_addr0 = (int)(fragments_temp(7,0));
			int zbuf_addr1 = (int)(fragments_temp(15,8));
			if( fragments_temp(23,16) < z_buffer[zbuf_addr1*MAX_Y+zbuf_addr0] )
			{
				bit24 pixels_temp;
				pixels_temp(7,0)     = fragments_temp(7,0);
				pixels_temp(15,8)     = fragments_temp(15,8);
				pixels_temp(23,16) = fragments_temp(31,24);
				pixels.write(pixels_temp);
				pixel_cntr++;
				z_buffer[zbuf_addr1*MAX_Y+zbuf_addr0] = fragments_temp(23,16);
			}
		}

		size_pixels.write(pixel_cntr);
	}
}

// color the frame buffer
void coloringFB(stream<bit16 > & size_pixels, stream<bit24 > & pixels, bit8 frame_buffer[MAX_X*MAX_Y], 
                stream<bit1 > & done, int num_3d_triangles)
{
#pragma FLASH EXEC_CYCLE
#pragma HLS INLINE off

	// initilize the framebuffer for a new image
	COLORING_FB_INIT_ROW: for ( int i = 0; i < MAX_X; i++) {
		COLORING_FB_INIT_COL: for ( int j = 0; j < MAX_Y; j++){
#pragma HLS PIPELINE II=1
			frame_buffer[i*MAX_Y+j] = 0;
		}
	}

	bit16 counter;
	for (counter = 0; counter < num_3d_triangles; counter++)
	{
		bit16 size_pixels_temp;
		size_pixels_temp = size_pixels.read();

		// update the framebuffer
		bit16 i;
		COLORING_FB: for ( i = 0; i < size_pixels_temp; i++) {
#pragma HLS PIPELINE II=1
			bit24 pixels_temp;
			pixels_temp = pixels.read();
			int framebuf_addr0 = (int)(pixels_temp(15,8));
			int framebuf_addr1 = (int)(pixels_temp(7,0));

		     frame_buffer[ framebuf_addr1*MAX_Y + framebuf_addr0 ] = pixels_temp(23,16);
		}
	}
	
	done.write(frame_buffer[0]);
}

// stream out the frame buffer
void output_FB(bit8 frame_buffer[MAX_X*MAX_Y], bit32 output[NUM_FB], stream<bit1 > & done)
{
#pragma FLASH EXEC_CYCLE
  #pragma HLS INLINE OFF

  bit1 done_temp;
  done_temp = done.read();

  bit32 out_FB = 0;
  OUTPUT_FB_ROW: for ( int i = 0; i < MAX_X; i++)
  {
    OUTPUT_FB_COL: for ( int j = 0; j < MAX_Y; j = j + 4)
    {
#pragma HLS PIPELINE II=1
      out_FB( 7,  0) = frame_buffer[i*MAX_Y + j + 0];
      out_FB(15,  8) = frame_buffer[i*MAX_Y + j + 1];
      out_FB(23, 16) = frame_buffer[i*MAX_Y + j + 2];
      out_FB(31, 24) = frame_buffer[i*MAX_Y + j + 3];
      output[i * MAX_Y / 4 + j / 4] = out_FB;
    }
  }
}


/*========================TOP FUNCTION===========================*/
void rendering( bit32 input[3 * MAX_NUM_TRIS], bit32 output[NUM_FB], int num_3d_triangles)
{
#pragma FLASH PRINTCYCLE
#pragma FLASH PRINT_ALLVAR
  // local variables
  stream<bit56 > triangle_2ds;
#pragma HLS STREAM variable=triangle_2ds depth=4
  stream<bit56 > triangle_2ds_same;
#pragma HLS STREAM variable=triangle_2ds_same depth=4

  stream<bit16 > size_fragment;
#pragma HLS STREAM variable=size_fragment depth=4
  stream<bit32 > fragment;
#pragma HLS STREAM variable=fragment depth=500

  stream<bit16 > size_pixels;
#pragma HLS STREAM variable=size_pixels depth=4
  stream<bit24 > pixels;
#pragma HLS STREAM variable=pixels depth=500

  bit8 frame_buffer[MAX_X*MAX_Y];
  bit2 angle = 0;

  stream<bit40 > max_min;
#pragma HLS STREAM variable=max_min depth=4
  stream<bit16 > max_index;
#pragma HLS STREAM variable=max_index depth=4

  stream<bit1 > done;
  #pragma HLS STREAM variable=done depth=4

    #ifdef USE_DATAFLOW
      #pragma HLS dataflow
    #endif

    // five stages for processing each 3D triangle
    projection( input, triangle_2ds, angle, num_3d_triangles );
    rasterization1( triangle_2ds, max_min, triangle_2ds_same, max_index, num_3d_triangles);
    rasterization2( size_fragment, max_min, max_index, triangle_2ds_same, fragment, num_3d_triangles );
    zculling( size_pixels, fragment, size_fragment, pixels, num_3d_triangles);
    coloringFB ( size_pixels, pixels, frame_buffer, done, num_3d_triangles);

  // output values: frame buffer
  output_FB(frame_buffer,output, done);
}
