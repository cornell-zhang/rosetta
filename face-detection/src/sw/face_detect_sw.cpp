/*===============================================================*/
/*                                                               */
/*                      face_detect-sw.cpp                       */
/*                                                               */
/*             Software version for face detection.              */
/*                                                               */
/*===============================================================*/

#include "face_detect_sw.h"

// function declarations
void integralImages( int height, int width, unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH], int Sum[IMAGE_HEIGHT][IMAGE_WIDTH], int Sqsum[IMAGE_HEIGHT][IMAGE_WIDTH]);

void imageScaler        ( int src_height,
			  int src_width,
                          unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH],
                          int dest_height,
			  int dest_width,
                          unsigned char IMG1_data[IMAGE_HEIGHT][IMAGE_WIDTH] 
                        );

void processImage       ( float factor,
                          int sum_row,
                          int sum_col,
                          int *AllCandidates_x,
                          int *AllCandidates_y,
                          int *AllCandidates_w,
                          int *AllCandidates_h,
                          int *AllCandidates_size,
                          unsigned char IMG1_data[IMAGE_HEIGHT][IMAGE_WIDTH],
                          MySize winSize
                        );


int cascadeClassifier  ( int SUM1_data[IMAGE_HEIGHT][IMAGE_WIDTH],
                         int SQSUM1_data[IMAGE_HEIGHT][IMAGE_WIDTH],
                         MyPoint pt
                       ); 

int weakClassifier      ( int stddev,
                          int coord[12],
                          int haar_counter,
                          int w_id                      
                        );

void groupRectangles    ( MyRect *rectList, 
                          int *rectList_size, 
                          int groupThreshold, 
                          float eps
                        );

unsigned int int_sqrt   ( unsigned int  value
                        );

inline  int  myRound ( float value )
{
  return (int)(value + (value >= 0 ? 0.5 : -0.5));
}

// top-level function
void face_detect_sw

( 
  unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH],  
  int result_x[RESULT_SIZE],         
  int result_y[RESULT_SIZE],
  int result_w[RESULT_SIZE],
  int result_h[RESULT_SIZE],
  int *result_size
)

{
   int i, j;

  *result_size = 0;
     
  float  scaleFactor = 1.2;
  unsigned char IMG1_data[IMAGE_HEIGHT][IMAGE_WIDTH];  

  float factor;
  int height,width;

  MySize winSize0;
  winSize0.width = 24;
  winSize0.height= 24;

  factor = scaleFactor;

  // Loop for image pyramid formation

  L1: 
  while ( IMAGE_WIDTH/factor > WINDOW_SIZE && IMAGE_HEIGHT/factor > WINDOW_SIZE )
  {

    /* size of the window scaled up */
    MySize winSize = { myRound(winSize0.width*factor), myRound(winSize0.height*factor) };

    /* size of the image scaled down */
    MySize sz = { (IMAGE_WIDTH/factor), (IMAGE_HEIGHT/factor) };
    
    height = sz.height;
    width  = sz.width;

    imageScaler     ( IMAGE_HEIGHT,
		      IMAGE_WIDTH,	
                      Data, 
                      height,
		      width,
                      IMG1_data
                    ); 
   
  
    processImage       ( factor, 
                         height, 
                         width,
                         result_x,
                         result_y,
                         result_w,
                         result_h,
                         result_size,
                         IMG1_data,
                         winSize
                       ); 
    factor *= scaleFactor;
  }

}

void processImage

( float factor,
  int sum_row,
  int sum_col, 
  int *AllCandidates_x,
  int *AllCandidates_y,
  int *AllCandidates_w,
  int *AllCandidates_h,
  int *AllCandidates_size, 
  unsigned char IMG1_data[IMAGE_HEIGHT][IMAGE_WIDTH], 
  MySize winSize
)
{
  MyPoint p;
  int result;
  int step;
 
  int u,v;
  int x,y,i,j,k;

  int SUM1_data[IMAGE_HEIGHT][IMAGE_WIDTH], SQSUM1_data[IMAGE_HEIGHT][IMAGE_WIDTH];

  integralImages( sum_row, sum_col ,IMG1_data, SUM1_data, SQSUM1_data );

  Pixely: for( y = 0; y < sum_row - WINDOW_SIZE + 1; y++ ){
    Pixelx: for ( x = 0; x < sum_col - WINDOW_SIZE + 1; x++ ){

      p.x = x;
      p.y = y;
      
      result = cascadeClassifier ( SUM1_data,
    			           SQSUM1_data,
    		                   p
                                 );

     if ( result > 0 ) {
       MyRect r = {myRound(p.x*factor), myRound(p.y*factor), winSize.width, winSize.height};
       AllCandidates_x[*AllCandidates_size]=r.x;
       AllCandidates_y[*AllCandidates_size]=r.y;
       AllCandidates_w[*AllCandidates_size]=r.width;
       AllCandidates_h[*AllCandidates_size]=r.height;
      *AllCandidates_size=*AllCandidates_size+1;
     }      
    }   
  } 
}

int cascadeClassifier 

( int SUM1_data[IMAGE_HEIGHT][IMAGE_WIDTH],
  int SQSUM1_data[IMAGE_HEIGHT][IMAGE_WIDTH],
  MyPoint pt
) 

{

  int i, j, k;

  int mean;
  int stddev = 0;
  int haar_counter = 0;
  int w_index = 0;
  int r_index = 0;
  int stage_sum=0;

  #include "haar_dataRcc_with_partitioning.h"

  static int coord[12];

  MyRect equRect;

  equRect.x = equRect.y = 0;
  equRect.width = WINDOW_SIZE;
  equRect.height = WINDOW_SIZE;

  stddev =                    SQSUM1_data[pt.y][pt.x]   
                           -  SQSUM1_data[pt.y][pt.x+WINDOW_SIZE-1]
                           -  SQSUM1_data[pt.y+WINDOW_SIZE-1][pt.x] 
                           +  SQSUM1_data[pt.y+WINDOW_SIZE-1][pt.x+WINDOW_SIZE-1];

  mean =                      SUM1_data[pt.y][pt.x]   
                           -  SUM1_data[pt.y][pt.x+WINDOW_SIZE-1]
                           -  SUM1_data[pt.y+WINDOW_SIZE-1][pt.x] 
                           +  SUM1_data[pt.y+WINDOW_SIZE-1][pt.x+WINDOW_SIZE-1];

  stddev = (stddev*(WINDOW_SIZE-1)*(WINDOW_SIZE-1));
  stddev =  stddev - mean*mean; 
 
  if( stddev > 0 )
  {
    stddev = int_sqrt(stddev);  
  }
  else{
    stddev = 1;
  }

  MyRect tr0,tr1,tr2;
  
  int r_id;
  int w_id;
  int s;

  Stages: for ( i = 0; i < 25; i++ ) {
    Filters: for ( j = 0; j < stages_array[i] ; j++ ){

      if ( j == 0 ) {
        stage_sum = 0; s=0;
      } 
   
      r_id = r_index;
      w_id = w_index;

      tr0.x = pt.x + rectangles_array0[haar_counter];
      tr0.width = rectangles_array2[haar_counter];
      tr0.y = pt.y + rectangles_array1[haar_counter];
      tr0.height = rectangles_array3[haar_counter];
   
      tr1.x = pt.x + rectangles_array4[haar_counter];
      tr1.width = rectangles_array6[haar_counter];
      tr1.y = pt.y + rectangles_array5[haar_counter];
      tr1.height = rectangles_array7[haar_counter];
    
      tr2.x =     rectangles_array8[haar_counter];
      tr2.width = rectangles_array10[haar_counter];
      tr2.y =     rectangles_array9[haar_counter];
      tr2.height = rectangles_array11[haar_counter];

      coord[0] = SUM1_data[tr0.y][tr0.x];
      coord[1] = SUM1_data[tr0.y][tr0.x+tr0.width];
      coord[2] = SUM1_data[tr0.y+tr0.height][tr0.x];
      coord[3] = SUM1_data[tr0.y+tr0.height][tr0.x+tr0.width];
     
      coord[4] = SUM1_data[tr1.y][tr1.x];
      coord[5] = SUM1_data[tr1.y][tr1.x+tr1.width];
      coord[6] = SUM1_data[tr1.y+tr1.height][tr1.x];
      coord[7] = SUM1_data[tr1.y+tr1.height][tr1.x+tr1.width];

      if (!(tr2.x ==0 && tr2.width==0 && tr2.y==0 && tr2.height==0 ) && tr2.width!=0 && tr2.height!=0)
      {
        coord[8] = SUM1_data[pt.y + tr2.y][pt.x + tr2.x];
        coord[9] = SUM1_data[pt.y + tr2.y][pt.x + tr2.x+tr2.width];
        coord[10] = SUM1_data[pt.y + tr2.y+tr2.height][pt.x + tr2.x];
        coord[11] = SUM1_data[pt.y + tr2.y+tr2.height][pt.x + tr2.x+tr2.width];     
      }
      else
      {
        coord[8] = 0;
        coord[9] = 0;
        coord[10] = 0;
        coord[11] = 0; 
      }
      
      s = weakClassifier      ( stddev, 
                                coord,
                                haar_counter,
                                w_id				
                              );
        
      stage_sum = stage_sum + s;
      haar_counter = haar_counter+1;
      w_index = w_index+3;
      r_index = r_index+12;
    } /* end of j loop */

    if( stage_sum < 0.4*stages_thresh_array[i] ){
       return -i;
    }    
  } /* end of i loop */

 return 1;
}
 
int weakClassifier
(
  int stddev,
  int coord[12],  
  int haar_counter,
  int w_id 
)
{                                                                                             
  #include "haar_dataEWC_with_partitioning.h"
  int t = tree_thresh_array[haar_counter] * stddev; 
  
  int sum0 =0;
  int sum1 =0;
  int sum2 =0;
  int final_sum =0;
  int return_value;
	             
  sum0 = (coord[0] - coord[1] - coord[2] + coord[3]) * weights_array0[haar_counter];//[w_id] area of 1st filter block (rectangle) multiplied by its weigh
  sum1 = (coord[4] - coord[5] - coord[6] + coord[7]) * weights_array1[haar_counter];//[w_id+1];    
  sum2 = (coord[8] - coord[9] - coord[10] + coord[11]) * weights_array2[haar_counter];//[w_id+2]; 
  final_sum = sum0+sum1+sum2;
  
  if(final_sum >= t) 
  {
    return_value =  alpha2_array[haar_counter];  
  }
  else   
  {
    return_value =  alpha1_array[haar_counter];
  }
  
  return return_value ; 

}

void integralImages( int height, int width, unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH], int Sum[IMAGE_HEIGHT][IMAGE_WIDTH], int Sqsum[IMAGE_HEIGHT][IMAGE_WIDTH])
{
  int x, y, s, sq, t, tq;
  unsigned char it;

  for( y = 0; y < height; y++)
    {
      s = 0;
      sq = 0;
      /* loop over the number of columns */
      for( x = 0; x < width; x ++)
	{
	  it = Data[y][x];
	  /* sum of the current row (integer)*/
	  s += it; 
	  sq += it*it;

	  t = s;
	  tq = sq;
	  if (y != 0)
	    {
	      t += Sum[(y-1)][x];
	      tq += Sqsum[(y-1)][x];
	    }
	  Sum[y][x]=t;
	  Sqsum[y][x]=tq;
	}
    }
}


/***********************************************************
 * This function downsample an image using nearest neighbor
 * It is used to build the image pyramid
 **********************************************************/
void imageScaler
(
  int src_height,
  int src_width,
  unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH], 
  int dest_height,
  int dest_width,
  unsigned char IMG1_data[IMAGE_HEIGHT][IMAGE_WIDTH]
)
{
  int y;
  int j;
  int x;
  int i;
  unsigned char* t;
  unsigned char* p;
  int w1 = src_width;
  int h1 = src_height;
  int w2 = dest_width;
  int h2 = dest_height;

  int rat = 0;

  int x_ratio = (int)((w1<<16)/w2) +1; 
  int y_ratio = (int)((h1<<16)/h2) +1;


  nearestNeighborL1:  
  for ( i = 0 ; i < IMAGE_HEIGHT ; i++ )
  { 
      nearestNeighborL1_1:  
      for (j=0;j < IMAGE_WIDTH ;j++)
      {
        if ( j < w2 && i < h2 ) 
        {
          IMG1_data[i][j] =  Data[(i*y_ratio)>>16][(j*x_ratio)>>16];
        }
      }
  }
}

unsigned int int_sqrt 
( 
  unsigned int value
)
{
  int i;
  unsigned int a = 0, b = 0, c = 0;



  for ( i = 0 ; i < (32 >> 1) ; i++ )
  {
    c<<= 2;   
    #define UPPERBITS(value) (value>>30)
    c += UPPERBITS(value);
    #undef UPPERBITS
    value <<= 2;
    a <<= 1;
    b = (a<<1) | 1;
    if ( c >= b )
    {
      c -= b;
      a++;
    }
  }
  return a;
}


int max
(
  int a,
   int b
)
{
 if ( a > b )
   return a;
 else 
   return b;
}

int min
(
  int a,
  int b
)
{
  if ( a < b )
    return a;
  else 
    return b;
}

