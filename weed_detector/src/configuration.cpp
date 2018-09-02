#include "configuration.hpp"
#include <iostream>
#include <stdio.h>
#include <windows.h>



//#define CAM_TYPE_16MPX //CAM_TYPE_ANALOG

bool is_settings_loaded = false;

LPCWSTR config_path = L"C:\\custom\\projects\\weed_detector\\src\\config.ini";


weed_detector_param_t wd_param;

void getGeneralConfiguration()
{
	LPCWSTR section = L"GENERAL_SETTINGS";

	wd_param.cam_type = GetPrivateProfileIntW(section, L"CAM_TYPE", 0, config_path);
	wd_param.fps = GetPrivateProfileIntW(section, L"FPS", 0, config_path);
	wd_param.averaging = GetPrivateProfileIntW(section, L"AVERAGING", 0, config_path);
	wd_param.hsv_debug = GetPrivateProfileIntW(section, L"HSV_DEBUG", 0, config_path);
	wd_param.ranged_dbg = GetPrivateProfileIntW(section, L"RANGED_DBG", 0, config_path);
	wd_param.morph_dbg = GetPrivateProfileIntW(section, L"MORPH_DBG", 0, config_path);
	wd_param.overlay   = GetPrivateProfileIntW(section, L"OVERLAY", 0, config_path);
	wd_param.recording = GetPrivateProfileIntW(section, L"RECORDING", 0, config_path);
	wd_param.area = GetPrivateProfileIntW(section, L"AREA", 0, config_path);
}

void getConfiguration()
{
	getGeneralConfiguration();

	if (wd_param.cam_type == E_CAM_AMALOG)
	{
		LPCWSTR section = L"HSV_ANALOG";

		wd_param.HSV.H.upper = GetPrivateProfileIntW(section, L"H_UPPER", 0, config_path);
		wd_param.HSV.H.lower = GetPrivateProfileIntW(section, L"H_LOWER", 0, config_path);

		wd_param.HSV.S.upper = GetPrivateProfileIntW(section, L"S_UPPER", 0, config_path);
		wd_param.HSV.S.lower = GetPrivateProfileIntW(section, L"S_LOWER", 0, config_path);

		wd_param.HSV.V.upper = GetPrivateProfileIntW(section, L"V_UPPER", 0, config_path);
		wd_param.HSV.V.lower = GetPrivateProfileIntW(section, L"V_LOWER", 0, config_path);
	}

	if (wd_param.cam_type == E_CAM_16MPX)
	{
		LPCWSTR section = L"HSV_16MPX";

		wd_param.HSV.H.upper = GetPrivateProfileIntW(section, L"H_UPPER", 0, config_path);
		wd_param.HSV.H.lower = GetPrivateProfileIntW(section, L"H_LOWER", 0, config_path);

		wd_param.HSV.S.upper = GetPrivateProfileIntW(section, L"S_UPPER", 0, config_path);
		wd_param.HSV.S.lower = GetPrivateProfileIntW(section, L"S_LOWER", 0, config_path);

		wd_param.HSV.V.upper = GetPrivateProfileIntW(section, L"V_UPPER", 0, config_path);
		wd_param.HSV.V.lower = GetPrivateProfileIntW(section, L"V_LOWER", 0, config_path);
	}

	printf("configuration loaded...\nCONFIG\n");
	printConfiguration();

	is_settings_loaded = true;
}

void printConfiguration(void)
{
	printf("CAM_TYPE: %d\n" , wd_param.cam_type);
	std::cout << "FPS:" << wd_param.fps << std::endl;
	printf("AVERAGING: %d\n", wd_param.averaging);
	//std::cout << "DETECTOR:" << wd_param.detector << std::endl; // to do
	std::cout << "HSV_DEBUG:" << wd_param.hsv_debug << std::endl;
	std::cout << "RANGED_DBG:" << wd_param.ranged_dbg<< std::endl;
	std::cout << "MORPH_DBG:" << wd_param.morph_dbg << std::endl;
	std::cout << "OVERLAY:" << wd_param.overlay << std::endl;
	printf("RECORDING: %d\n", wd_param.recording);
	std::cout << "AREA:" << wd_param.area << std::endl;
}

double setSettings_FPS(void)
{
	if (is_settings_loaded == true)
	{
		return wd_param.fps;
	}
	else
	{
		return 10.0;
	}

}

uint8_t setSettings_Averaging(void)
{
	if (is_settings_loaded == true)
	{
		return wd_param.averaging;
	}
	else
	{
		return 3;
	}
}

bool isSettings_HSV_DEBUG(void)
{
	return wd_param.hsv_debug;
}

bool isSettings_RANGED_DBG(void)
{
	return wd_param.ranged_dbg;
}

bool isSettings_MORPH_DBG(void)
{
	return wd_param.morph_dbg;
}

bool isSettings_OVERLAY(void)
{
	return wd_param.overlay;
}

uint8_t setSettings_RECORDING(void)
{
	return wd_param.recording;
}

uint32_t setSettings_AREA(void)
{
	return wd_param.area;
}

uint8_t setSettings_Hu(void)
{
	return wd_param.HSV.H.upper;
}

uint8_t setSettings_Su(void)
{
	return wd_param.HSV.S.upper;
}

uint8_t setSettings_Vu(void)
{
	return wd_param.HSV.V.upper;
}

uint8_t setSettings_Hl(void)
{
	return wd_param.HSV.H.lower;
}

uint8_t setSettings_Sl(void)
{
	return wd_param.HSV.S.lower;
}

uint8_t setSettings_Vl(void)
{
	return wd_param.HSV.V.lower;
}

