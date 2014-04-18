#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;

IplImage* imgTracking=0;

int lastX1 = -1;
int lastY1 = -1;

int lastX2 = -1;
int lastY2 = -1;

void detectionObject(IplImage* imgThresh){
CvSeq* contour;  //hold the pointer to a contour
CvSeq* result;     //hold sequence of points of a contour
CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

//finding all contours in the image
cvFindContours(imgThresh, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

//iterating through each contour
while(contour)
{
//obtain a sequence of points of the countour, pointed by the variable 'countour'
result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.02, 0);

//if there are 3 vertices  in the contour
if(result->total==3)
{

CvPoint *pt[3];
for(int i=0;i<3;i++)
{
pt[i] = (CvPoint*)cvGetSeqElem(result, i);
}
cvLine(img, *pt[0], *pt[1], cvScalar(255,0,0),4);//bleu
cvLine(img, *pt[1], *pt[2], cvScalar(255,0,0),4);
cvLine(img, *pt[2], *pt[0], cvScalar(255,0,0),4);

}
else if (result -> total = 4)
{
    CvPoint *pt[4];
    for(int i=0;i<4;i++)
    {
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
    }
    cvLine(img, *pt[0], *pt[1], cvScalar(0,255,0),4);//vert
    cvLine(img, *pt[1], *pt[2], cvScalar(0,255,0),4);
    cvLine(img, *pt[2], *pt[3], cvScalar(0,255,0),4);
    cvLine(img, *pt[3], *pt[0], cvScalar(0,255,0),4);
}
else if (result -> total = 5)
{
    CvPoint *pt[5];
    for(int i=0;i<5;i++)
    {
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
    }
    cvLine(img, *pt[0], *pt[1], cvScalar(0,255,0),4);//vert
    cvLine(img, *pt[1], *pt[2], cvScalar(0,255,0),4);
    cvLine(img, *pt[2], *pt[3], cvScalar(0,255,0),4);
    cvLine(img, *pt[3], *pt[0], cvScalar(0,255,0),4);
    cvLine(img, *pt[3], *pt[0], cvScalar(0,255,0),4);
}



cvReleaseMemStorage(&storage);
}


int main(){
    //load the video file to the memory
CvCapture *capture =     cvCaptureFromAVI("E:/Projects/Robot/IESL Robot/robot/a.avi");

    if(!capture){
        printf("Capture failure\n");
        return -1;
    }

    IplImage* frame=0;
    frame = cvQueryFrame(capture);
    if(!frame) return -1;

    //create a blank image and assigned to 'imgTracking' which has the same size of original video
    imgTracking=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U, 3);
    cvZero(imgTracking); //covert the image, 'imgTracking' to black

    cvNamedWindow("Video");

//iterate through each frames of the video
    while(true){

        frame = cvQueryFrame(capture);
        if(!frame) break;
        frame=cvCloneImage(frame);

//smooth the original image using Gaussian kernel
        cvSmooth(frame, frame, CV_GAUSSIAN,3,3);

//converting the original image into grayscale
IplImage* imgGrayScale = cvCreateImage(cvGetSize(frame), 8, 1);
cvCvtColor(frame,imgGrayScale,CV_BGR2GRAY);

       //thresholding the grayscale image to get better results
cvThreshold(imgGrayScale,imgGrayScale,100,255,CV_THRESH_BINARY_INV);

        //track the possition of the ball
        trackObject(imgGrayScale);

        // Add the tracking image and the frame
        cvAdd(frame, imgTracking, frame);

        cvShowImage("Video", frame);

        //Clean up used images
        cvReleaseImage(&imgGrayScale);
        cvReleaseImage(&frame);

        //Wait 10mS
        int c = cvWaitKey(10);
        //If 'ESC' is pressed, break the loop
        if((char)c==27 ) break;
    }

    cvDestroyAllWindows();
    cvReleaseImage(&imgTracking);
    cvReleaseCapture(&capture);

    return 0;
}
