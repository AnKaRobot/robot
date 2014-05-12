#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, const char** argv )
{
       Mat img = imread("../normalization-build/MyPic.JPG", CV_LOAD_IMAGE_COLOR); 

       if (img.empty()) 
       {
            cout << "Image cannot be loaded..!!" << endl;
            return -1;
       }

       vector<Mat> channels; 
       Mat img_hist_equalized;

       cvtColor(img, img_hist_equalized, CV_BGR2YCrCb); 

       split(img_hist_equalized,channels); 

       imwrite("Y_channel.jpg",channels[0]);
       imwrite("Cr_channel.jpg",channels[1]);
       imwrite("Cb_channel.jpg",channels[2]);

	equalizeHist(channels[0], channels[0]);

   merge(channels,img_hist_equalized); 

      cvtColor(img_hist_equalized, img_hist_equalized, CV_YCrCb2BGR); 

       
       namedWindow("Original Image", CV_WINDOW_NORMAL);
       namedWindow("Histogram Equalized", CV_WINDOW_NORMAL);

      
       imshow("Original Image", img);
       imshow("Histogram Equalized", img_hist_equalized);

       waitKey(0); 

       destroyAllWindows(); 

       return 0;
}
