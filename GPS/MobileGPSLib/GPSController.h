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

#pragma once 
#include <GPSApi.h>

// *************************************************************************
// Class IGPSSink 
//
// Inheritance:
//      None
//
// Purpose:
//      To define an interface that is to be implemented by client objects
//      that want to receive call back from CGPSController
//
//      - SetGPSPosition will be called whenever new location information is
//      available
//      - SetGPSDeviceInfo will be called on two instances
//          1. Whenever the GPS hardware status 
//          2. When the CGPSController class is initialized for the first time
//
// **************************************************************************
interface IGPSSink
{
    virtual HRESULT SetGPSPosition(GPS_POSITION gps_Position) = 0;
    virtual HRESULT SetGPSDeviceInfo(GPS_DEVICE gps_Device) = 0;
};


// *************************************************************************
// Class CGPSController
//
// Inheritance:
//      None
//
// Purpose:
//      To provide an abstraction of the steps required asynchronously recieve
//      GPS Data whenever it is available
//
//      This class performs a call back on an IGPSSink interface using
//      a specialized thread.  As a result, the object implementing IGPSSink 
//      got to be allocated from the heap.
//
// **************************************************************************
class CGPSController
{
public:
    CGPSController();
    HRESULT InitDevice(__in IGPSSink *pGPSSink);
    HRESULT UninitDevice();

private:
    static DWORD WINAPI GPSThreadProc(__opt LPVOID lpParameter);
    HRESULT LoadThread();
    HRESULT UnloadThread();

private:
    // Data available to the worker thread
    static HANDLE s_hGPS_Device;
    static HANDLE s_hNewLocationData;
    static HANDLE s_hDeviceStateChange;
    static HANDLE s_hExitThread;
    // Pointer to the call back interface
    static IGPSSink *s_pGPSSink;

    // Thread Info
    HANDLE m_hThread;
    DWORD m_dwThreadID;

};