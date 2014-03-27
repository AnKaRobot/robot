#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

void saveValues(int state, void*);

int main(int argc, char *argv[]) {

	char c;

	VideoCapture cap(0);
	if (!cap.isOpened()) {
		printf("*** problème caméra ***\n");		
		return -1;
	}
	namedWindow("base", WINDOW_AUTOSIZE);
	namedWindow("trackbars", WINDOW_NORMAL);
	namedWindow("ajust", WINDOW_AUTOSIZE);	

	Mat frame;
	
	int erodeVal = 0,
		smoothVal = 0,
		rMinVal = 0, 
		gMinVal = 0, 
		bMinVal = 159,
		rMaxVal = 104,
		gMaxVal = 255,
		bMaxVal = 255;

	createTrackbar("erode", "base", &erodeVal, 30);
	createTrackbar("smooth", "base", &smoothVal, 100);

	createTrackbar("rMin", "trackbars", &rMinVal, 255);
	createTrackbar("gMin", "trackbars", &gMinVal, 255);
	createTrackbar("bMin", "trackbars", &bMinVal, 255);

	createTrackbar("rMax", "trackbars", &rMaxVal, 255);
	createTrackbar("gMax", "trackbars", &gMaxVal, 255);
	createTrackbar("bMax", "trackbars", &bMaxVal, 255);

	//createButton("save", saveValues, NULL, CV_PUSH_BUTTON);

	Mat element, frameCalc, frame3;

	vector <vector <Point> > contours;
	vector <Vec4i> hierarchy;


	while (1) {
		cap >> frame;
		if (frame.empty() ) {
			printf("*** problème frame ***\n");
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
			inRange(frame, Scalar(rMinVal, gMinVal, bMinVal), Scalar(rMaxVal, gMaxVal, bMaxVal), frameCalc);
			frame3 = frameCalc.clone();
			findContours(frame3, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

			for (int index = 0; index >= 0; index = hierarchy[index][0]) {
				Moments moment = moments( (Mat) contours[index] );
				double area = moment.m00;
				
				if (area > 5*5 && area < 100 * 100) {
				int x = moment.m10/area;
				int y = moment.m01/area;
				circle(frame, Point(x, y), 15, Scalar(0, 0, 255), 2);
				}
			}
			imshow("ajust", frameCalc);
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
void saveValues(int state, void*) {
	int erodeVal = getTrackbarPos("erode", "base"),
		smoothVal = getTrackbarPos("smooth", "base"),
		rMin = getTrackbarPos("rMin", "trackbars"),
		gMin = getTrackbarPos("gMin", "trackbars"),		
		bMin = getTrackbarPos("bMin", "trackbars"),
		rMax = getTrackbarPos("rMax", "trackbars"),
		gMax = getTrackbarPos("gMax", "trackbars"),
		bMax = getTrackbarPos("bMax", "trackbars");

			
}*/
