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
#include "GPSSink.h"
// **************************************************************************
// Function Name: CGPSSink:SetGPSDeviceInfo
// 
// Purpose: Receive callbacks from CGPSController
//
// Arguments:
//    GPS_DEVICE gps_Device
//
// Return Values:
//    HRESULT: S_OK
//
// Side effects:  
//    None
// 
// Description:  
//    This function is called by CGPSController on the first time CGPSSink 
//    is registered to receive callbacks.  Then, it is called whenever
//    there is a hardware/GPS Intermediate Driver level changes. This function
//    then relies on CDisplayGPSData to show the data to the user.
//    Note that the return value is ignored by CGPSController and that it 
//    is safe to rely on the compiler to perform a bitwise 
//    copy on GPS_DEVICE.
// **************************************************************************
HRESULT CGPSSink::SetGPSDeviceInfo(GPS_DEVICE gps_Device)
{
    m_DisplayGPSInfo.DisplayDeviceInfo(gps_Device);
    return S_OK;
}

// **************************************************************************
// Function Name: CGPSSink:SetGPSPosition
// 
// Purpose: Receive callbacks from CGPSController
//
// Arguments:
//    GPS_POSITION gps_Position
//
// Return Values:
//    HRESULT: S_OK
//
// Side effects:  
//    None
// 
// Description:  
//    This function is called by CGPSController whenever the Intermediate Driver
//    had outsanding location data. it then relies on CDisplayGPSData to show 
//    the data to the user.
//    Note that the return value is ignored by CGPSController and that it 
//    is safe to rely on the compiler to perform a bitwise 
//    copy on GPS_POSITION.
// **************************************************************************
HRESULT CGPSSink::SetGPSPosition(GPS_POSITION gps_Position)
{
    m_DisplayGPSInfo.IsValidPosition(gps_Position);
    return S_OK;
}
