#pragma once
#include <stdint.h>
#include "opencv2/core.hpp"

typedef enum settings_cam_types_e
{
	E_CAM_AMALOG = 0,
	E_CAM_16MPX,
}settings_cam_types_t;

typedef enum settings_recording_e
{
	E_REC_OFF = 0,
	E_REC_ORIG,
	E_REC_PROC,
}settings_recording_t;

typedef struct weed_detector_param_s
{
	uint8_t cam_type;
	double fps;
	uint8_t averaging;
	bool hsv_debug;
	bool ranged_dbg;
	bool morph_dbg;
	bool overlay;
	uint8_t recording;
	uint32_t area;

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



//--------------------------------------------------------
void getConfiguration();
void printConfiguration(void);
double setSettings_FPS(void);
bool isSettings_HSV_DEBUG(void);
bool isSettings_RANGED_DBG(void);
bool isSettings_MORPH_DBG(void);
bool isSettings_OVERLAY(void);
uint8_t setSettings_RECORDING(void);
uint8_t setSettings_Averaging(void);
uint32_t setSettings_AREA(void);
uint8_t setSettings_Hu(void);
uint8_t setSettings_Su(void);
uint8_t setSettings_Vu(void);
uint8_t setSettings_Hl(void);
uint8_t setSettings_Sl(void);
uint8_t setSettings_Vl(void);
//--------------------------------------------------------
extern weed_detector_param_t wd_param;
