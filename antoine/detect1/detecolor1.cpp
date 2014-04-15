#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main (int argc, char **argv) {

	cout << "Début\n";

	VideoCapture video(0);
    
	if (! video.isOpened()) {
		cout << "Problème source\n";
		return -1;
	}
 
	Mat frame, frame2;
	bool continuer = true;

	namedWindow("base", CV_WINDOW_NORMAL);
	namedWindow("transformed", CV_WINDOW_NORMAL);
	namedWindow("panel", CV_WINDOW_AUTOSIZE);
	
	resizeWindow("base", 500, 375);
	resizeWindow("transformed", 500, 375);
	
	moveWindow("base", 0, 0);
	moveWindow("transformed", 500, 550);
	moveWindow("panel", 0, 550);
	
	int hTol = 54,
	    sTol = 92;
	    
    createTrackbar("hTolerance", "panel", &hTol, 100);
	createTrackbar("sTolerance", "panel", &sTol, 100);
	
	while (continuer) {
		video >> frame;
		if (frame.empty()) {
			cout << "Problème frame\n";
		}
		else {
			cvtColor(frame, frame2, CV_RGB2HSV);
			inRange(
			    frame2, 
                Scalar(175-hTol, 226-sTol, 0), 
                Scalar(175+hTol, 226+sTol, 255), 
                frame2
            );
          
			imshow("base", frame);
			imshow("transformed", frame2);
		}
		if (waitKey(30) != -1) { continuer = false; }
	}

	destroyAllWindows();
	video.release();
	frame.release();
	frame2.release();
	
	cout << "Fin\n";

	return 0;
}
