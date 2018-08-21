#include <iostream>
#include <sstream>
#include <windows.h>
//
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc.hpp>
#include "opencv2/videoio.hpp"
#include "HSVselection.hpp"
#include "Serial.hpp"


using namespace std;
using namespace cv;


//#define HSV_DEBUG
//#define VIDEO_FILE_IN


const char* camera_live = "LIVE";
const char* solenoid_closed = "CLOSE SOLENOID";
const char* solenoid_opened = "OPEN SOLENOID";
bool gl_grass_decected = false;
bool gl_grass_last_state = false;
int counterFound = 0;


void printVideoOverlay_CLOSE(Mat& dst_output)
{
	rectangle(dst_output, Point(0, 0), Point(640, 100), Scalar(0, 255, 0), FILLED, LINE_8);
	putText(dst_output, solenoid_closed, Point(10, 50), 3, 2, Scalar(0, 0, 0));
	imshow(camera_live, dst_output);
}

void printVideoOverlay_OPEN(Mat& dst_output)
{
	rectangle(dst_output, Point(0, 0), Point(640, 100), Scalar(0, 0, 255), FILLED, LINE_8);
	putText(dst_output, solenoid_opened, Point(10, 50), 3, 2, Scalar(0, 0, 0));
	imshow(camera_live, dst_output);
}

void videoProcessing(Mat& camera_input, Mat& original_img)
{

	camera_input.copyTo(original_img);
	// grass
	// H: 25 - 90
	// S: 0  - 255
	// V: 50 - 255

	cvtColor(camera_input, camera_input, COLOR_BGR2HSV); //Convert the captured frame // 0.011 - 0.022 sec

	inRange(camera_input, Scalar(25, 0, 50), Scalar(90, 255, 255), camera_input); // 0.011 - 0.021 sec
	//imshow("camera_ranged", camera_input);  // debug: HSV colorspace image


	//morphological operations
	erode(camera_input, camera_input, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));  // 0.005 - 0.006 sec
	dilate(camera_input, camera_input, getStructuringElement(MORPH_RECT, Size(10, 10))); // 0.004 -0.0064 sec
	dilate(camera_input, camera_input, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));

	imshow("camera_morph", camera_input);  // debug: morph image  // 0.002 sec


	vector < vector <Point>> contours;
	findContours(camera_input, contours, RETR_LIST, CHAIN_APPROX_NONE, Point(0, 0));  // 0.003 - 0.010 sec
	Mat drawing = Mat::zeros(camera_input.size(), CV_8UC3);

	//for (size_t i = 0; i < contours.size(); i++)  // 0.015 - 0.021 sec
	//{
	//	Scalar color = Scalar(121, 113, 96);
	//	drawContours(drawing, contours, i, color, -1);
	//	imshow("contour", drawing);  // debug: show contours
	//}

	gl_grass_decected = false;
	// Find areas
	vector <double> areas;
	for (size_t i = 0; i < contours.size(); i++)  // 0.0002  - 0.0011 sec
	{
		areas.push_back(contourArea(contours[i], false));
		//printf("--------------contour #%d area:%f\n", i, areas[i]);
		if (areas[i] >= 40000)
		{
			if (counterFound > 3)
			{
				gl_grass_decected = true;
			}
			//printf("grass found at #%d, area:%f\n", i, areas[i]); // debug: show all areas
			//printf("FOUND! Opening solenoid...");
		}
	}

	if (counterFound > 3)
	{
		if ((gl_grass_decected == true) && (gl_grass_last_state == false))
		{
			printf("[FOUND] Opening solenoid...\n");
			SerialPortWrite((char*)"[FOUND] Opening solenoid...\n", sizeof("[FOUND] Opening solenoid...\n"));
			gl_grass_last_state = true;
		}
		else if ((gl_grass_decected == false) && (gl_grass_last_state == true))
		{
			printf("[NOT FOUND] Close solenoid\n");
			SerialPortWrite((char*)"[NOT FOUND] Close solenoid\n", sizeof("[NOT FOUND] Close solenoid\n"));
			gl_grass_last_state = false;
		}
		counterFound = 0;
	}

	counterFound++;

	if (gl_grass_last_state)
	{
		printVideoOverlay_OPEN(original_img);
	}
	else
	{
		printVideoOverlay_CLOSE(original_img);
	}
}

void processMapir()
{

}


int main(int argc, char* argv[])
{
#ifdef VIDEO_FILE_IN
	// Open video file
	VideoCapture video("D:/Projects/video_recognition/Louis_grass/Louis_grass/Debug/2018_0811_072230_022.MP4");

	double fps = video.get(CV_CAP_PROP_FPS);

	cout << "Frames per second using video.get(CV_CAP_PROP_FPS) : " << fps << endl;
	system("pause"); //wait for any key press

	video.release();
	return 0;
#else

	SerialPortInit();
	SerialPortWrite((char*)"TEST_UART", sizeof("TEST_UART"));
	//while (1)
	//{
	//	SerialPortRead();
	//};
	Mat camera_input, camera_input_orig;
	VideoCapture cap;

	//----------------------
	int64 e1, e2; // testing clock counts for operations
	double time;
	//----------------------

	cap.open(0);

	if (!cap.isOpened())
	{
		cerr << "ERROR! Unable to open camera\n";
		system("pause"); //wait for any key press
		return -1;
	}

	cout << "Start grabbing" << endl;

	for (;;)
	{
		cap.read(camera_input);
		if (camera_input.empty())
		{
			cerr << "ERROR! blank frame grabbed\n";
			system("pause"); //wait for any key press
			return -1;
		}

		//imshow(camera_live, camera_input); // debug: original capture
#ifdef HSV_DEBUG
		const char* HSV_window_name = "SelectionInHSV";
		Mat cam_original;
		camera_input.copyTo(cam_original);
		imshow(HSV_window_name, HSVSelection(cam_original, HSV_window_name));
#endif HSV_DEBUG

		e1 = getTickCount();
		videoProcessing(camera_input, camera_input_orig);
		e2 = getTickCount();
		time = (e2 - e1) / getTickFrequency();

		// exit
		if (waitKey(5) >= 0)
		{
			break;
		}
	}
#endif
}
