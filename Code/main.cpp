#define _CRT_SECURE_NO_WARNINGS

#include "Utility.h"
#pragma comment(lib, "mpr.lib")


int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cout << "Invalid number of parameters!\n";
        return 1;
    }
    
    
    Utility ult;
    
    
    LPCWSTR ip = ult.ByteToWideChar((BYTE*)argv[1]);
    LPCWSTR usrname = ult.ByteToWideChar((BYTE*)argv[2]);
    LPCWSTR passwd = ult.ByteToWideChar((BYTE*)argv[3]);


    //remote name: \\\\ + ip + \\admin$
    wchar_t wRemoteName[MAX_PATH];
    swprintf_s(wRemoteName, sizeof(wRemoteName), L"\\\\%ls\\admin$", ip);
   

    // Define the network resource structure
    NETRESOURCE nr;
    nr.dwScope = RESOURCE_CONNECTED;
    nr.dwType = RESOURCETYPE_ANY;
    nr.lpLocalName = NULL;
    nr.lpRemoteName = (LPWSTR)wRemoteName;
    nr.lpProvider = NULL;
    nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;

    
    wchar_t wUsrname[MAX_PATH];
    swprintf_s(wUsrname, sizeof(wUsrname), L"%ls\\%ls", ip, usrname); 
   
    // Connect to the remote share using the current user credentials
    DWORD result = WNetAddConnection2(&nr, passwd, wUsrname, CONNECT_TEMPORARY);
    if (result == NO_ERROR)
    {
        ult.UninstallService(ip, L"svc");

        std::wcout << "Connected to " << ip << '\n';
        std::cout << "Input the shared folder to save file: ";
        std::wstring inputFolder;
        std::wcin >> inputFolder;
        LPCWSTR lpRemoteFolder = inputFolder.c_str();

        // path = "\\\\" + ip + "\\" + folder + "\\svc.exe";
        wchar_t wPathDest[MAX_PATH];
        swprintf_s(wPathDest, sizeof(wPathDest), L"\\\\%ls\\%ls\\svc.exe", ip, lpRemoteFolder);
        
        
        std::cout << "Input the service file path to copy: ";
        std::wstring inputSource;
        std::wcin >> inputSource;
        LPCWSTR lpLocalSource = inputSource.c_str(); // Local file

        // Copy the file using the CopyFile function
        BOOL success = CopyFile(lpLocalSource, wPathDest, FALSE);
        if (success)
        {
            std::wcout << "Copied service file to " << ip << '\n';
        }
        else
        {
            std::cout << "CopyFile failed with error: " << GetLastError() << '\n';
        }
        
        
        if (ult.InstallService(ip, wPathDest, L"svc") == FALSE)
        {
            std::cout << "Error in creating service: " << GetLastError() << '\n';
        }
        else
        {
            std::cout << "Create service successfully\n";
        }

        // Disconnect from the remote share
        result = WNetCancelConnection2(wRemoteName, 0, TRUE);
        if (result == NO_ERROR)
        {
            std::wcout << "Disconnected from " << ip << '\n';
        }
        else
        {
            std::cout << "WNetCancelConnection2 failed with error " << GetLastError() << '\n';
        }
    }
    else if(result == ERROR_LOGON_FAILURE)
    {
        std::cout << "Username or password is not valid: " << result << '\n';
    }
    else if (result == ERROR_ACCESS_DENIED)
    {
        std::cout << "User does not have admin privilege: " << result << '\n';
    }
    else if (result == ERROR_NO_NETWORK)
    {
        std::cout << "The network is unavailable: " << result << '\n';
    }
    else
    {
        std::cout << "Error in connecting to remote machine: " << result << '\n';
    }
    return 0;
}
