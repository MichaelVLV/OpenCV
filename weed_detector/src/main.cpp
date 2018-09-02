#include <iostream>
#include <sstream>
#include <stdio.h>
#include <windows.h>
//
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc.hpp>
#include "opencv2/videoio.hpp"
#include "HSVselection.hpp"
#include "Serial.hpp"
#include "configuration.hpp"


using namespace std;
using namespace cv;


#define CAM_NUM   0
#define SERIAL_ENABLED // enable serial output
//#define HSV_DEBUG      // creates HSV colorspace field
//#define REC_ORIGINAL   // recordes original capture from camera
//#define REC_PROCESSED  // recordes processed capture
//#define VIDEO_FILE_IN  // processing recorded video (not implemented)
#//define SHOW_OVERLAY     // shows camera stream with overlays


#if defined(REC_ORIGINAL) & defined(REC_PROCESSED)
#error "only one stream can be recorded"
#endif

typedef struct recorder_param_s
{
	bool isColor;
	VideoWriter writer;
	int codec;  // select desired codec (must be available at runtime)
	double fps; // framerate of the created video stream
	char* filename; // name of the output video file with path
}recorder_param_t, *recorder_param_p;

typedef struct cam_param_s
{
	double height;
	double width;
	double fps;
	union {
		char c[4];
		double i;
	}codec;

}cam_param_t, *cam_param_p;


const char* camera_live     = "LIVE";
const char* solenoid_closed = "CLOSE SOLENOID";
const char* solenoid_opened = "OPEN SOLENOID";

const char serial_text_found[]     = "[OPEN]\r";
const char serial_text_not_found[] = "[CLOSE]\r";

const char* rec_orig_path   = "C:/custom/record_orig.avi";
const char* rec_proc_path   = "C:/custom/record_proc.avi";
//---------------------------------------------------------------
const float gl_FPS = 10.0; // fps for recording (moved to ext config)
//---------------------------------------------------------------
bool gl_grass_decected   = false;
bool gl_grass_last_state = false;
int counterFound = 0;


void printVideoOverlay_CLOSE(Mat& dst_output)
{
	if (isSettings_OVERLAY() == true) 
	{
		rectangle(dst_output, Point(0, 0), Point(640, 100), Scalar(0, 255, 0), FILLED, LINE_8);
		putText(dst_output, solenoid_closed, Point(10, 50), 3, 2, Scalar(0, 0, 0));
		imshow(camera_live, dst_output);
	}
}

void printVideoOverlay_OPEN(Mat& dst_output)
{
	if (isSettings_OVERLAY() == true)
	{
		rectangle(dst_output, Point(0, 0), Point(640, 100), Scalar(0, 0, 255), FILLED, LINE_8);
		putText(dst_output, solenoid_opened, Point(10, 50), 3, 2, Scalar(0, 0, 0));
		imshow(camera_live, dst_output);
	}
}


Mat videoProcessing(Mat& camera_input, Mat& original_img)
{
	static int64 e1, e2; // testing clock counts for operations
	static double time;
	//----------------------
	e1 = getTickCount();

	camera_input.copyTo(original_img);
	// grass
	// H: 25 - 90
	// S: 0  - 255
	// V: 50 - 255
	
	cvtColor(camera_input, camera_input, COLOR_BGR2HSV); //Convert the captured frame // 0.011 - 0.022 sec

	Scalar range_upper_set = Scalar (setSettings_Hu() , setSettings_Su() , setSettings_Vu() );
	Scalar range_lower_set = Scalar (setSettings_Hl() , setSettings_Sl() , setSettings_Vl() );
	//printf("upper: %d, %d, %d \n", setSettings_Hu(), setSettings_Su(), setSettings_Vu()); //debug
	//printf("lower: %d, %d, %d \n", setSettings_Hl(), setSettings_Sl(), setSettings_Vl()); //debug
	//inRange(camera_input, Scalar(25, 0, 50), Scalar(90, 255, 255), camera_input); // 0.011 - 0.021 sec // for 16Mpix camera
	//inRange(camera_input, Scalar(50, 85, 175), Scalar(160, 255, 255), camera_input); // 0.011 - 0.021 sec // for analog toradex camera
	inRange(camera_input, range_lower_set, range_upper_set, camera_input); // 0.011 - 0.021 sec


	if (isSettings_RANGED_DBG() == true)
	{
		imshow("camera_ranged", camera_input);  // debug: HSV colorspace image
	}


	//morphological operations
	erode(camera_input, camera_input, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));  // 0.005 - 0.006 sec
	dilate(camera_input, camera_input, getStructuringElement(MORPH_RECT, Size(10, 10))); // 0.004 -0.0064 sec
	dilate(camera_input, camera_input, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));

	if (isSettings_MORPH_DBG() == true)
	{
		imshow("camera_morph", camera_input);  // debug: morph image  // 0.002 sec
	}

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
		//if (areas[i] >= 40000) // for 16Mpix camera
		//if (areas[i] >= 10000) // for toradex analog camera
		
		if (areas[i] >= setSettings_AREA() )
		{
			if (counterFound > setSettings_Averaging() )
			{
				gl_grass_decected = true;
			}
			//printf("grass found at #%d, area:%f\n", i, areas[i]); // debug: show all areas
			//printf("FOUND! Opening solenoid...");
		}
	}

	if (counterFound > setSettings_Averaging() )
	{
		if ((gl_grass_decected == true) && (gl_grass_last_state == false))
		{
			printf(serial_text_found);
#ifdef SERIAL_ENABLED
			SerialPortWrite((char*)serial_text_found, sizeof(serial_text_found));
#endif SERIAL_ENABLED
			gl_grass_last_state = true;
		}
		else if ((gl_grass_decected == false) && (gl_grass_last_state == true))
		{
			printf(serial_text_not_found);
#ifdef SERIAL_ENABLED
			SerialPortWrite((char*)serial_text_not_found, sizeof(serial_text_not_found));
#endif SERIAL_ENABLED
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

	e2 = getTickCount();
	time = (e2 - e1) / getTickFrequency();

	return original_img;
}

void processMapir()
{

}

recorder_param_t recp;
void videoRecorder(Mat& source, const char* filename)
{
	static bool isRecorderInited = false;

	if(isRecorderInited == false)
	{ 
	    recp.isColor = (source.type() == CV_8UC3);
		recp.codec = CV_FOURCC('M', 'J', 'P', 'G');  
		recp.fps = setSettings_FPS();//25.0;                          
		//string filename = "D:/Projects/video_recognition/VideoRecog/live_stream_record.avi";             
		recp.writer.open(filename, recp.codec, recp.fps, source.size(), recp.isColor);

		if (!recp.writer.isOpened()) {
			cerr << "Could not open the output video file for write\n";
			system("pause"); //wait for any key press
			return;
		}

		isRecorderInited = true;
	}

	cout << "Writing videofile: " << filename << endl;
	recp.writer.write(source);
}

cam_param_t cp;
void getCameraParams(VideoCapture& cap)
{
	cp.fps = cap.get(CAP_PROP_FPS);
	cp.width = cap.get(CAP_PROP_FRAME_WIDTH);
	cp.height = cap.get(CAP_PROP_FRAME_HEIGHT);
	cp.codec.i = cap.get(CAP_PROP_FOURCC);
}

void setCameraParams(VideoCapture& cap)
{
	cap.set(CAP_PROP_FRAME_WIDTH, 240);
	cap.set(CAP_PROP_FRAME_HEIGHT, 320);
	cap.set(CAP_PROP_FPS, 25);
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

#ifdef SERIAL_ENABLED
	SerialPortInit();
	SerialPortWrite((char*)"TEST_UART", sizeof("TEST_UART"));
	//while (1)
	//{
	//	SerialPortRead();
	//};
#endif SERIAL_ENABLED

	Mat camera_input, camera_input_orig;
	//Mat processed;
	VideoCapture cap;

	getConfiguration();
	//TODO: use loaded configuration

	cap.open(CAM_NUM);

	if (!cap.isOpened())
	{
		cerr << "ERROR! Unable to open camera\n";
		system("pause"); //wait for any key press
		return -1;
	}

	getCameraParams(cap);

	//setCameraParams(cap);

	cout << "Start grabbing" << endl;

	for (;;)
	{
		//e1 = getTickCount();
		cap.read(camera_input);
		if (camera_input.empty())
		{
			cerr << "ERROR! blank frame grabbed\n";
			system("pause"); //wait for any key press
			return -1;
		}
		//imshow(camera_live, camera_input); // debug: original capture

		if (setSettings_RECORDING() == E_REC_ORIG)
		{
			recp.filename = (char*)rec_orig_path;
			videoRecorder(camera_input, (char*)recp.filename);
		}
		
		if (isSettings_HSV_DEBUG() == true)
		{
			const char* HSV_window_name = "SelectionInHSV";
			Mat cam_original;
			camera_input.copyTo(cam_original);
			imshow(HSV_window_name, HSVSelection(cam_original, HSV_window_name));
		}

		Mat processed = videoProcessing(camera_input, camera_input_orig);

		if (setSettings_RECORDING() == E_REC_PROC)
		{
			recp.filename = (char*)rec_proc_path;
			videoRecorder(processed, (char*)recp.filename);
		}

		//e2 = getTickCount();
		//time = (e2 - e1) / getTickFrequency();

		// exit
		if (waitKey(5) >= 0)
		{
			break;
		}
	}
#endif
}
