#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main (int argc, char **argv) {
    
	cout << "Début\n";

    string source("/home/robuntu/Videos/vid1.mp4");
	//int source = 0;
    VideoCapture video(source);
    
	if (! video.isOpened()) {
		cout << "Problème source\n";
		return -1;
	}
 
	bool continuer = true;
	int compteurErreurs = 0;
	bool neverEnd = true;

	namedWindow("base", CV_WINDOW_NORMAL);
	namedWindow("transformed", CV_WINDOW_NORMAL);
	namedWindow("panel", CV_WINDOW_AUTOSIZE);
	
	resizeWindow("base", 500, 375);
	resizeWindow("transformed", 500, 375);
	
	moveWindow("base", 0, 0);
	moveWindow("transformed", 500, 550);
	moveWindow("panel", 0, 500);
	
	int hTol = 54,
	    sTol = 92,
	    dSize = 0,
	    eSize = 0;
	    
	Mat frame, frame2, element;
	   
    createTrackbar("hTolerance", "panel", &hTol, 100);
	createTrackbar("sTolerance", "panel", &sTol, 100);
	createTrackbar("dilateSize", "panel", &dSize, 30);
	createTrackbar("erodeSize", "panel", &eSize, 30);
	
	while (continuer) {
		video >> frame;
		if (frame.empty()) {
			cout << "Problème frame\n";
			compteurErreurs ++;
			if (compteurErreurs > 5) {
			    if (neverEnd) {
			        video.open(source);
			    }
			    else {
			        cout << "Arrêt : 6 frames erronées consécutives\n";
			        continuer = false;
			    }
			}
		}
		else {
		    compteurErreurs = 0;
			cvtColor(frame, frame2, CV_RGB2HSV);
			inRange(
			    frame2, 
                Scalar(175-hTol, 226-sTol, 0), 
                Scalar(175+hTol, 226+sTol, 255), 
                frame2
            );
            
            
            if (eSize > 0) {
                element = getStructuringElement(MORPH_ELLIPSE, Size(eSize, eSize));    
                erode(frame2, frame2, element);
            }
            if (dSize > 0) {
                element = getStructuringElement(MORPH_ELLIPSE, Size(dSize, dSize));    
                dilate(frame2, frame2, element);
            }
            
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

