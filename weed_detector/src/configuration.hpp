#pragma once
#include <stdint.h>

typedef struct weed_detector_param_s
{
	struct
	{
		struct {
			uint8_t upper;
			uint8_t lower;
		}H;
		struct {
			uint8_t upper;
			uint8_t lower;
		}S;
		struct {
			uint8_t upper;
			uint8_t lower;
		}V;
	}HSV;
}weed_detector_param_t, *weed_detector_param_p;


void getConfiguration();

//------------------------
extern weed_detector_param_t wd_param;
