#include "HSVselection.hpp"

Mat HSVSelection(Mat& img_input, const char* window_name)
{
	Mat camera_HSV;
	Mat camera_HSV_th;

	static int iLowH = 0;
	static int iHighH = 255;

	static int iLowS = 0;
	static int iHighS = 255;

	static int iLowV = 0;
	static int iHighV = 255;

	namedWindow(window_name, CV_WINDOW_AUTOSIZE); //create a window

	//Create trackbars in window
	cvCreateTrackbar("Low_H", window_name, &iLowH, 255); //Hue (0 - 255)
	cvCreateTrackbar("High_H", window_name, &iHighH, 255);

	cvCreateTrackbar("Low_S", window_name, &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("High_S", window_name, &iHighS, 255);

	cvCreateTrackbar("Low_V", window_name, &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("High_V", window_name, &iHighV, 255);

	cvtColor(img_input, camera_HSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	//imshow("camera_HSV", camera_HSV);  // debug: HSV colorspace image

	inRange(camera_HSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), camera_HSV_th); //Threshold the image
	//inRange(camera_HSV, Scalar(0, 0, 0), Scalar(255, 255, 90), camera_HSV_th); // fixed threshold of the image

	//imshow("camera_HSV_th", camera_HSV_th);  // debug: thersholded HSV colorspace image

	return camera_HSV_th;
}