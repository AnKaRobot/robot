#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char *argv[]) {

	char c;

	VideoCapture cap(0);
	if (!cap.isOpened()) {
		return -1;
	}
	namedWindow("base", 1);
	Mat frame;
	
	int erodeVal = 0,
		smoothVal = 0;

	createTrackbar("erode", "base", &erodeVal, 30);
	createTrackbar("smooth", "base", &smoothVal, 100);

	Mat element;

	while (1) {
		cap >> frame;
		if (frame.empty() ) {
			printf("***problème frame***\n");
		}
		else {
			if (erodeVal) {
				element = getStructuringElement(MORPH_ELLIPSE, Size(erodeVal, erodeVal));
				erode(frame, frame, element);
			}
			if (smoothVal) {
				blur(frame, frame, Size(smoothVal, smoothVal));
			}
			//cvtColor(frame, frame, COLOR_RGB2HSV);
			imshow("base", frame);
		}
		c = (char) cvWaitKey(30);
		if (c == 27) {
			break;
		}
	}
	return 0;
}
/*
void barreAction(int, void*) {
	int barreValue = getTrackbarPos("barre", "base");
}*/
