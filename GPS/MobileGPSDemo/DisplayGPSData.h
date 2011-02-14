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
#include "..\MobileGPSLib\GPSController.h"
#include <sms.h>



// *************************************************************************
// Class CDisplayGPSData 
//
// Inheritance:
//      None
//
// Purpose:
//      To display the GPS data to the user.  
//
//      This classs is used by CGPSSink to display the GPS data
// **************************************************************************
class CDisplayGPSData
{
public:
    HRESULT DisplayDeviceInfo(__in GPS_DEVICE gps_Device);
	HRESULT IsValidPosition(__in GPS_POSITION gps_Position);
	void Initialize(LPTSTR phoneNumber);
	bool DidSMSSend();
	void SendError();
	bool AnyGPSLocationFound();
	void DisplayInformation();

private:
	void SendSMSMessage();
	void SendSMS(BOOL , BOOL , LPCTSTR , LPCTSTR , LPCTSTR );
	void ShowGPSMessage();
	void SendSMSError();
	void ShowGPSError();

private:
    GPS_DEVICE m_GPS_Device;
	GPS_POSITION best_GPS_Position;
	
	int number_of_GPS_Positions;
	bool offlineMode;
	bool gpsLock;
	LPTSTR number;
};