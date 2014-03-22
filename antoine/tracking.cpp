#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char *argv[]) {

	VideoCapture cap(0);
	if (!cap.isOpened()) {
		return -1;
	}
	namedWindow("edges", 1);
	while (1) {
		Mat frame;
		cap >> frame;
		imshow("edges", frame);
		if(waitKey(30) >= 0) { break; }
	}
	return 0;
}
