#include "configuration.hpp"
#include <stdio.h>
#include <windows.h>


#define CAM_TYPE_16MPX //CAM_TYPE_ANALOG


LPCWSTR config_path = L"C:\\custom\\projects\\weed_detector\\src\\config.ini";


weed_detector_param_t wd_param;

void getConfiguration()
{
#ifdef CAM_TYPE_ANALOG
	LPCWSTR section = L"HSV_ANALOG";

	wd_param.HSV.H.upper = GetPrivateProfileIntW(section, L"H_UPPER", 0, config_path);
	wd_param.HSV.H.lower = GetPrivateProfileIntW(section, L"H_LOWER", 0, config_path);

	wd_param.HSV.S.upper = GetPrivateProfileIntW(section, L"S_UPPER", 0, config_path);
	wd_param.HSV.S.lower = GetPrivateProfileIntW(section, L"S_LOWER", 0, config_path);

	wd_param.HSV.V.upper = GetPrivateProfileIntW(section, L"V_UPPER", 0, config_path);
	wd_param.HSV.V.lower = GetPrivateProfileIntW(section, L"V_LOWER", 0, config_path);
#endif CAM_TYPE_ANALOG

#ifdef CAM_TYPE_16MPX
	LPCWSTR section = L"HSV_16MPX";

	wd_param.HSV.H.upper = GetPrivateProfileIntW(section, L"H_UPPER", 0, config_path);
	wd_param.HSV.H.lower = GetPrivateProfileIntW(section, L"H_LOWER", 0, config_path);

	wd_param.HSV.S.upper = GetPrivateProfileIntW(section, L"S_UPPER", 0, config_path);
	wd_param.HSV.S.lower = GetPrivateProfileIntW(section, L"S_LOWER", 0, config_path);

	wd_param.HSV.V.upper = GetPrivateProfileIntW(section, L"V_UPPER", 0, config_path);
	wd_param.HSV.V.lower = GetPrivateProfileIntW(section, L"V_LOWER", 0, config_path);
#endif CAM_TYPE_16MPX

	printf("configuration loaded...");
}
