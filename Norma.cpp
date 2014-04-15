
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main( int, char** argv )
{
  Mat src, dst;

  const char* source_window = "Source image";
  const char* equalized_window = "Equalized Image";

  src = imread( argv[1], 1 );

  if( !src.data )
    { cout<<"Usage: ./Histogram_Demo <path_to_image>"<<endl;
      return -1;
    }


  cvtColor( src, src, COLOR_BGR2GRAY );


  equalizeHist( src, dst );


  namedWindow( source_window, WINDOW_NORMAL );
  namedWindow( equalized_window, WINDOW_NORMAL );

  imshow( source_window, src );
  imshow( equalized_window, dst );


  waitKey(0);

  return 0;

}
