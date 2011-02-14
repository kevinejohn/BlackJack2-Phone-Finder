//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//

#include "stdafx.h"
#include "GPSController.h"

HANDLE CGPSController::s_hGPS_Device = NULL;
HANDLE CGPSController::s_hNewLocationData = NULL;
HANDLE CGPSController::s_hDeviceStateChange = NULL;
HANDLE CGPSController::s_hExitThread = NULL;
IGPSSink *CGPSController::s_pGPSSink = NULL;

#define MAX_WAIT    5000
#define MAX_AGE     3000
#define GPS_CONTROLLER_EVENT_COUNT 3

// **************************************************************************
// Function Name: CGPSController:CGPSController
// 
// Purpose: Initialize the non static member variables of CGPSController
//
// Arguments:
//    None
//
// Return Values:
//    None
//
// Side effects:  
//    None
// 
// Description:  
//    See purpose
// **************************************************************************
CGPSController::CGPSController() : m_hThread(NULL), m_dwThreadID(0)
{

}

// **************************************************************************
// Function Name: CGPSController:GPSThreadProc
// 
// Purpose: Provide an Asynchronous mechanism to query for GPS data
//
// Arguments:
//    __opt LPVOID lpParameter: This parameter is not used.  It is required to 
//                        enable GPSThreadProc to be started as a thread
//
// Return Values:
//    None: it always return 0.
//
// Side effects:  
//    The thread got to be instructed to unload before CGPSController object
//    is deleted.
// 
// Description:  
//    GPSThreadProc asynchronously receive notifcations events from the GPS
//    intermediate driver whenever the GPS hardware status changes or new
//    location information is available
// **************************************************************************
DWORD WINAPI CGPSController::GPSThreadProc(__opt LPVOID lpParameter)
{
	bool smsSent = false;

    DWORD dwRet = 0;
    GPS_POSITION gps_Position = {0};
    GPS_DEVICE gps_Device = {0};
    HANDLE gpsHandles[GPS_CONTROLLER_EVENT_COUNT] = {s_hNewLocationData, 
        s_hDeviceStateChange,
        s_hExitThread
        };

    gps_Position.dwSize = sizeof(gps_Position);
    gps_Position.dwVersion = GPS_VERSION_1;

    gps_Device.dwVersion = GPS_VERSION_1;
    gps_Device.dwSize = sizeof(gps_Device);

    do
    {
        dwRet = WaitForMultipleObjects(GPS_CONTROLLER_EVENT_COUNT,
            gpsHandles,
            FALSE,
            INFINITE);
        if (dwRet == WAIT_OBJECT_0)
        {
            dwRet = GPSGetPosition(s_hGPS_Device,
                &gps_Position,
                MAX_AGE,
                0);
            if (ERROR_SUCCESS != dwRet)
            {
                continue;
            }
            else
            {
                s_pGPSSink->SetGPSPosition(gps_Position);
            }
        }
        else if (dwRet == WAIT_OBJECT_0 + 1)
        {
            dwRet = GPSGetDeviceState(&gps_Device);			
            if (ERROR_SUCCESS != dwRet)
            {
                continue;
            }
            else
            {
                s_pGPSSink->SetGPSDeviceInfo(gps_Device);
            }			
        }
        else if (dwRet == WAIT_OBJECT_0 + 2)
        {
            break;
        }
        else
        {
            ASSERT(0);
        }
    } while (1);


    return 0;
}



// **************************************************************************
// Function Name: CGPSController:UnloadThread
// 
// Purpose: Instructs GPSThreadProc to unload and wait for the thread to exit
//
// Arguments:
//    None
//
// Return Values:
//    HRESULT: S_OK on success 
//             An appropriate error value on failure
//
// Side effects:  
//    It unloads GPSThreadProc
// 
// Description:  
//    GPSThreadProc asynchronously receive notifcations events from the GPS
//    intermediate driver whenever the GPS hardware status changes or new
//    location information is available
// **************************************************************************
HRESULT CGPSController::UnloadThread()
{
    HRESULT hr = E_FAIL;
    BOOL bRet = FALSE;
    DWORD dwRet = 0;

    // Instruct the thread to unload
    // There is not much one can do if set event failed
    // Ignore the failure and continue unloading
    bRet = SetEvent(s_hExitThread);
    if (!bRet)
    {
        dwRet = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRet);
        goto Exit;
    }

    dwRet = WaitForSingleObject(m_hThread, MAX_WAIT);
    if(WAIT_OBJECT_0 != dwRet)
    {
        dwRet = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRet);
        goto Exit;
    }

    hr = S_OK;

Exit:
    if (s_hNewLocationData)
    {
        CloseHandle(s_hNewLocationData);
        s_hNewLocationData = NULL;
    }

    if (s_hDeviceStateChange)
    {
        CloseHandle(s_hDeviceStateChange);
        s_hDeviceStateChange = NULL;
    }

    if (s_hExitThread)
    {
        CloseHandle(s_hExitThread);
        s_hExitThread = NULL;
    }

    if (m_hThread)
    {
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    return hr;
}

// **************************************************************************
// Function Name: CGPSController:LoadThread
// 
// Purpose: Initialize the variables to be used by GPSThreadProc then 
//          create the thread
//
// Arguments:
//    None
//
// Return Values:
//    HRESULT: S_OK on success
//             An appropriate error value on failure
//              
//
// Side effects:  
//    It loads the GPSThreadProc
// 
// Description:  
//    LoadThread intializes 3 events before creating GPSThreadProc.
//    s_hNewLocationData    :   This event is set by the GPS Intermdiate 
//                              driver whenever new location information is
//                              available
//    s_hDeviceStateChange  :   This event is set by the GPS Intermediate
//                              driver whenever the GPS hardware state 
//                              changes
//    s_hExitThread         :   Used by CGPSController to instruct 
//                              GPSThreadProc to unload
// **************************************************************************
HRESULT CGPSController::LoadThread()
{
    HRESULT hr = E_FAIL;
    DWORD dwRet = 0;

    s_hNewLocationData = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!s_hNewLocationData)
    {
        dwRet = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRet);
        goto Exit;
    }

    s_hDeviceStateChange = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!s_hDeviceStateChange)
    {
        dwRet = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRet);
        goto Exit;
    }

    s_hExitThread = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!s_hExitThread)
    {
        dwRet = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRet);
        goto Exit;
    }

    m_hThread = CreateThread(NULL, NULL, GPSThreadProc, NULL, NULL, &m_dwThreadID);
    if (!m_hThread)
    {
        dwRet = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRet);
        goto Exit;
    }

    hr = S_OK;

Exit:
    if (FAILED(hr))
    {
        if (s_hNewLocationData)
        {
            CloseHandle(s_hNewLocationData);
            s_hNewLocationData = NULL;
        }

        if (s_hDeviceStateChange)
        {
            CloseHandle(s_hDeviceStateChange);
            s_hDeviceStateChange = NULL;
        }

        if (s_hExitThread)
        {
            CloseHandle(s_hExitThread);
            s_hExitThread = NULL;
        }

        // tmphThread is NULL & the thread was not created
    }


    return hr;
}

// **************************************************************************
// Function Name: CGPSController:InitDevice
// 
// Purpose: Setup CGPSContoller internal state then initialize the GPS 
//          Intermediate driver
//
// Arguments:
//    __in IGPSSink *pGPSSink : Call back interface
//
// Return Values:
//    HRESULT: S_OK on success
//             An appropriate error value on failure
//              
//
// Side effects:  
//    If InitDevice succeeds, then UninitDevice is to be called before 
//    detroying the CGPSController object
// 
// Description:
//    See purpose
//    
// **************************************************************************
HRESULT CGPSController::InitDevice(__in IGPSSink *pGPSSink)
{
    HRESULT hr = E_FAIL;
    DWORD	dwRet = 0;

    if (NULL == pGPSSink)
    {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (s_hGPS_Device)
    {
        // InitDevice is to be called only once
        // This is best protected by implemeneting
        // an instantitor function and having the construtor set to become
        // private.
        hr = E_INVALIDARG;
        goto Exit;
    }

    s_pGPSSink = pGPSSink;

    hr = LoadThread();
    if (FAILED(hr))
    {
       goto Exit;
    }

    s_hGPS_Device = GPSOpenDevice(s_hNewLocationData,
        s_hDeviceStateChange,
        NULL,
        NULL);
    if (!s_hGPS_Device)
    {
        dwRet = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRet);
        goto Exit;
    }

    hr = S_OK;

Exit:
    if (FAILED(hr))
    {
        UninitDevice();
    }
    return hr;
}

// **************************************************************************
// Function Name: CGPSController:UninitDevice
// 
// Purpose: clear CGPSContoller internal state and uninitialize the GPS 
//          Intermediate driver handle
//
// Arguments:
//    None
//
// Return Values:
//    HRESULT: S_OK on success
//             E_INVALIDARG if the Intermediate Driver is not initialized
//             Otherwise, an appropriate error value on failure
//              
//
// Side effects:  
//    Unload the thread and release all resources owned by CGPSController
// 
// Description:
//    See purpose
//    
// **************************************************************************
HRESULT CGPSController::UninitDevice()
{
    HRESULT hr = E_FAIL;
    DWORD dwRet = 0;

    if (!s_hGPS_Device)
    {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Attempt to unload the thread
    // There is nothing much that one can do if it fails
    hr = UnloadThread();

    // Attempt to unload the thread
    // there is nothing much that one can do if it fails
    dwRet = GPSCloseDevice(s_hGPS_Device);
    s_hGPS_Device = NULL;

    // return the origin of the failure if any
    if (FAILED(hr))
    {
       goto Exit;
    }
    else if (ERROR_SUCCESS != dwRet)
    {
        dwRet = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRet);
        goto Exit;
    }

    hr = S_OK;

Exit:
    return hr;
}