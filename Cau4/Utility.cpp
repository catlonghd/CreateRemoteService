#include "Utility.h"

LPCWSTR Utility::ByteToWideChar(BYTE* str)
{
    LPCCH narrowString = (LPCCH)str;
    int length = MultiByteToWideChar(CP_ACP, 0, narrowString, -1, nullptr, 0);
    wchar_t* wideString = new wchar_t[length];
    MultiByteToWideChar(CP_ACP, 0, narrowString, -1, wideString, length);

    return wideString;
}

LPCWSTR Utility::ConcatWideChar(LPCWSTR str1, LPCWSTR str2)
{
    size_t length = wcslen(str1) + wcslen(str2) + 1;
    wchar_t* result = new wchar_t[length];
    wcscpy_s(result, length, str1);
    wcscat_s(result, length, str2);

    return result;
}


BOOL Utility::InstallService(LPCWSTR ip, LPCWSTR remoteFilePath, LPCWSTR serviceName)
{
    

    // Connect to the remote machine using the OpenSCManagerW function
    SC_HANDLE scMangager = OpenSCManager(ip, NULL, SC_MANAGER_CREATE_SERVICE);
    if (scMangager == 0)
    {
        std::cout << "Error at OpenSCManager\n";
        return FALSE;
    }

        
    // Create the new service using the CreateServiceW function
    SC_HANDLE scService = CreateService(scMangager,
                                        serviceName,
                                        serviceName,
                                        STANDARD_RIGHTS_REQUIRED | SERVICE_START | SERVICE_STOP | SERVICE_PAUSE_CONTINUE | SERVICE_QUERY_STATUS,
                                        SERVICE_WIN32_OWN_PROCESS,
                                        SERVICE_AUTO_START,
                                        SERVICE_ERROR_NORMAL,
                                        remoteFilePath,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL);

    if (scService == NULL)
    {
        std::cout << "Error at CreateService!\n";
        CloseServiceHandle(scMangager);
        return FALSE;
    }

    // Start the new service using the OpenServiceW function
    SC_HANDLE scServiceHandle = OpenService(scMangager, serviceName, SERVICE_ALL_ACCESS);
    if (scServiceHandle == NULL)
    {
        CloseServiceHandle(scService);
        CloseServiceHandle(scMangager);
        return FALSE;
    }

    SERVICE_STATUS serviceStatus;
    if (StartService(scServiceHandle, 0, NULL) == FALSE)
    {
        CloseServiceHandle(scServiceHandle);
        CloseServiceHandle(scService);
        CloseServiceHandle(scMangager);
        return FALSE;
    } 

    // Check if the service was created successfully
    if (!QueryServiceStatus(scServiceHandle, &serviceStatus) != FALSE)
    {
        CloseServiceHandle(scServiceHandle);
        CloseServiceHandle(scService);
        CloseServiceHandle(scMangager);
        return FALSE;
    }

    // Disconnect from the remote machine
    CloseServiceHandle(scServiceHandle);
    CloseServiceHandle(scService);
    CloseServiceHandle(scMangager);

    return TRUE;

}

BOOL Utility::UninstallService(LPCWSTR ip, LPCWSTR serviceName)
{
   
    // Connect to the remote machine using the OpenSCManagerW function
    SC_HANDLE scMangager = OpenSCManager(ip, NULL, SC_MANAGER_CREATE_SERVICE);
    if (scMangager == 0)
    {
        std::cout << "Error at OpenSCManager\n";
        return FALSE;
    }

    SC_HANDLE scServiceHandle = OpenService(scMangager, serviceName, SERVICE_ALL_ACCESS);
    if (scServiceHandle == NULL)
    {
        CloseServiceHandle(scMangager);
        return TRUE;
    }
    
    SERVICE_STATUS svcStatus;
    ControlService(scServiceHandle, SERVICE_CONTROL_STOP, &svcStatus);

    if (!DeleteService(scServiceHandle))
    {
        CloseServiceHandle(scMangager);
        CloseServiceHandle(scServiceHandle);
        return FALSE;
    }

    CloseServiceHandle(scMangager);
    CloseServiceHandle(scServiceHandle);
    return 0;
}
