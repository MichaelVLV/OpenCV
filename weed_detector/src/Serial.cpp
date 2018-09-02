#include <windows.h>
#include <iostream>
#include "Serial.hpp"

HANDLE hSerial;

void SerialPortRead(void)
{
	DWORD iSize;
	char RxData;

	while (true)
	{
		ReadFile(hSerial, &RxData, 1, &iSize, 0);
		if (iSize > 0)
		{
			std::cout << RxData;
		}
	}
}


void SerialPortWrite(char *data, DWORD len)
{
	//char data[] = "test_string";
	//DWORD dwSize = sizeof(data);
	DWORD dwBytesWritten;
	BOOL iRet = WriteFile(hSerial, data, len, &dwBytesWritten, NULL);
	if (!iRet) std::cout << "Some error occured in send\n";
	//cout << dwSize << " Bytes in string. " << dwBytesWritten << " Bytes sended. " << std::endl;
}

void SerialPortInit(void)
{
	LPCTSTR sPortName = L"\\\\.\\COM4";

	hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			std::cout << "serial port does not exist.\n";
		}
		std::cout << "some other error occurred.\n";
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		std::cout << "getting state error\n";
	}
	dcbSerialParams.BaudRate = CBR_115200;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		std::cout << "error setting serial port state\n";
	}
}


