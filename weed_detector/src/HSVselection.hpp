#pragma once
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "C:/custom/opencv/modules/highgui/include/opencv2/highgui/highgui_c.h" // workaround for OpenCV4.0.0

using namespace std;
using namespace cv;

Mat HSVSelection(Mat& img_input, const char* window_name);