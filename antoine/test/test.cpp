#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main (int argc, char **argv) {

	cout << "Début\n";

	VideoCapture video("/home/robuntu/test/vid1.mp4");
    //VideoCapture video(0);
    
	if (! video.isOpened()) {
		cout << "Problème source\n";
		return -1;
	}

	Size s = Size(
		(int) video.get(CV_CAP_PROP_FRAME_WIDTH),
		(int) video.get(CV_CAP_PROP_FRAME_HEIGHT));

	VideoWriter output;
	output.open("output.avi", CV_FOURCC('M', 'J', 'P', 'G'), 20, s, true);
	
	if (! output.isOpened()) {
		cout << "Problème output\n";
		return -1;
	}
 
	Mat frame;
	bool continuer = true;
    int compteurErreurs = 0;
    
	namedWindow("test", CV_WINDOW_AUTOSIZE);

	while (continuer) {
		video >> frame;
		if (frame.empty()) {
			cout << "Problème frame\n";
			compteurErreurs ++;
			if (compteurErreurs >= 5) { continuer = false; }
		}
		else { 
			imshow("test", frame); 
			output << frame;
			compteurErreurs = 0;
		}
		if (waitKey(30) != -1) { continuer = false; }
	}

	destroyAllWindows();
	video.release();
	output.release();
	frame.release();
	
	cout << "Fin\n";

	return 0;
}
