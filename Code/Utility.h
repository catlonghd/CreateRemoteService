#pragma once
#include <windows.h>
#include <iostream>
#include <string>

class Utility
{
public:
	LPCWSTR ByteToWideChar(BYTE* str);
	LPCWSTR ConcatWideChar(LPCWSTR str1, LPCWSTR str2);
    BOOL InstallService(LPCWSTR ip, LPCWSTR remoteFilePath, LPCWSTR serviceName);
    BOOL UninstallService(LPCWSTR ip, LPCWSTR serviceName);
};

