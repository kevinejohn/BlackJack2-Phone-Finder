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
#include "DisplayGPSData.h"


// **************************************************************************
// Function Name: CDisplayGPSData:DisplayDeviceInfo
// 
// Purpose: Update local GPS_DEVICE data structure and display the data to 
//          the user
//
// Arguments:
//    __in GPS_DEVICE gps_Device: standar GPS_DEVICE structure
//
// Return Values:
//    HRESULT: S_OK on success
//             An appropriate error value on failure
//
// Side effects:  
//    None
// 
// Description:  
//    See purpose.  Note that it is safe to rely on the compiler to perform 
//    a bitwise copy on gps_Device.
// **************************************************************************
HRESULT CDisplayGPSData::DisplayDeviceInfo(__in GPS_DEVICE gps_Device)
{
    HRESULT hr = E_FAIL;
    m_GPS_Device = gps_Device;

    //hr = ShowData();

    return hr;
}



void CDisplayGPSData::SendSMSMessage()
{
	TCHAR gpszMessage[160];

	_sntprintf(gpszMessage, sizeof(gpszMessage), TEXT("http://maps.google.com/maps?q=%2.6f,%2.6f Alt%2.2f Speed%3.1f Heading%3.2f H-Quality%3.1f V-Quality%3.1f Satel%d"), 
		best_GPS_Position.dblLatitude, 
		best_GPS_Position.dblLongitude, 
		best_GPS_Position.flAltitudeWRTSeaLevel, 
		best_GPS_Position.flSpeed, 
		best_GPS_Position.flHeading, 
		best_GPS_Position.flHorizontalDilutionOfPrecision,
		best_GPS_Position.flVerticalDilutionOfPrecision,
		best_GPS_Position.dwSatelliteCount);

	LPTSTR gpszServer = (LPTSTR)L"";
	LPTSTR gpszRecipient = number;
	SendSMS(false, true, gpszServer, gpszRecipient, gpszMessage);

	/*
	MessageBox(NULL,
					L"Message Sent", 
					L"Sent",
					MB_OK);
	*/	
}



// ***************************************************************************
// Function Name: SendSMS
// 
// Purpose: Send an SMS Message
//
// Arguments: none
//
// Return Values: none
//
// Description:
//	Called after everything has been set up, this function merely opens an
//	SMS_HANDLE and tries to send the SMS Message.
 
void CDisplayGPSData::SendSMS(BOOL bSendConfirmation, BOOL bUseDefaultSMSC, LPCTSTR lpszSMSC, LPCTSTR lpszRecipient, LPCTSTR lpszMessage)
{
	SMS_HANDLE smshHandle;
	SMS_ADDRESS smsaSource;
	SMS_ADDRESS smsaDestination;
	TEXT_PROVIDER_SPECIFIC_DATA tpsd;
	SMS_MESSAGE_ID smsmidMessageID;

	// try to open an SMS Handle
	if(FAILED(SmsOpen(SMS_MSGTYPE_TEXT, SMS_MODE_SEND, &smshHandle, NULL)))
	{
/*		MessageBox(NULL,
					(LPCTSTR)LoadString(ghInstance, IDS_ERROR_SMSOPEN, 0, 0), 
					(LPCTSTR)LoadString(ghInstance, IDS_CAPTION_ERROR, 0, 0),
					MB_OK | MB_ICONERROR);*/
		return;
	}

	// Create the source address
	if(!bUseDefaultSMSC)
	{
		smsaSource.smsatAddressType = SMSAT_INTERNATIONAL;
		_tcsncpy(smsaSource.ptsAddress, lpszSMSC, SMS_MAX_ADDRESS_LENGTH);
	}

	// Create the destination address
	if( lstrlen(lpszRecipient) < 11 ){
		smsaDestination.smsatAddressType = SMSAT_ABBREVIATED;
	}
	else{
		smsaDestination.smsatAddressType = SMSAT_INTERNATIONAL;
	}
	_tcsncpy(smsaDestination.ptsAddress, lpszRecipient, SMS_MAX_ADDRESS_LENGTH);

	// Set up provider specific data
    memset(&tpsd, 0, sizeof(tpsd));
	tpsd.dwMessageOptions = bSendConfirmation ? PS_MESSAGE_OPTION_STATUSREPORT : PS_MESSAGE_OPTION_NONE;
	tpsd.psMessageClass = PS_MESSAGE_CLASS1;
	tpsd.psReplaceOption = PSRO_NONE;
	tpsd.dwHeaderDataSize = 0;

	// Send the message, indicating success or failure
	if(SUCCEEDED(SmsSendMessage(smshHandle, ((bUseDefaultSMSC) ? NULL : &smsaSource), 
								 &smsaDestination, NULL, (PBYTE) lpszMessage, 
								 _tcslen(lpszMessage) * sizeof(TCHAR), (PBYTE) &tpsd, 
								 sizeof(TEXT_PROVIDER_SPECIFIC_DATA), SMSDE_OPTIMAL, 
								 SMS_OPTION_DELIVERY_NONE, &smsmidMessageID)))
	{
	/*	MessageBox(NULL,
					(LPCTSTR)LoadString(ghInstance, IDS_SMSSENT, 0, 0), 
					(LPCTSTR)LoadString(ghInstance, IDS_CAPTION_SUCCESS, 0, 0),
					MB_OK);*/
	}
	else
	{
	/*	MessageBox(NULL,
					(LPCTSTR)LoadString(ghInstance, IDS_ERROR_SMSSEND, 0, 0), 
					(LPCTSTR)LoadString(ghInstance, IDS_CAPTION_ERROR, 0, 0),
					MB_OK | MB_ICONERROR);
					*/
	}

	// clean up
	VERIFY(SUCCEEDED(SmsClose(smshHandle)));
}



HRESULT CDisplayGPSData::IsValidPosition(__in GPS_POSITION gps_Position)
{
	//GPS_POSITION m_GPS_Position = gps_Position;


	if((!gpsLock) &&
		(gps_Position.flHorizontalDilutionOfPrecision > 0) &&
		(gps_Position.flHorizontalDilutionOfPrecision < 50) &&
		((gps_Position.flHorizontalDilutionOfPrecision != best_GPS_Position.flHorizontalDilutionOfPrecision) ||
		 (gps_Position.dwSatelliteCount > best_GPS_Position.dwSatelliteCount))){


			//Initialize the first GPS Position
			//if(number_of_GPS_Positions == 0){
			//	best_GPS_Position = gps_Position;
			//}
			//Increment the number of GPS positions
			if(best_GPS_Position.dwSatelliteCount >= 3){
				number_of_GPS_Positions++;
			}

			if(best_GPS_Position.dwSatelliteCount < gps_Position.dwSatelliteCount){
			
				best_GPS_Position = gps_Position;
			}
			else if(best_GPS_Position.dwSatelliteCount == gps_Position.dwSatelliteCount){
				if(best_GPS_Position.flHorizontalDilutionOfPrecision > gps_Position.flHorizontalDilutionOfPrecision){
					best_GPS_Position = gps_Position;
				}
			}

			if(((best_GPS_Position.dwSatelliteCount >=3) && (best_GPS_Position.flHorizontalDilutionOfPrecision < 2)) || 
				(number_of_GPS_Positions >= 8)){

				DisplayInformation();
			}
	}

	return S_OK;
}



bool CDisplayGPSData::DidSMSSend()
{
	return gpsLock;
}



void CDisplayGPSData::Initialize(LPTSTR phoneNumber)
{
	if(phoneNumber == L""){
		offlineMode = true;
	}
	else
	{
		offlineMode = false;
	}
	best_GPS_Position.flHorizontalDilutionOfPrecision = 999;
	best_GPS_Position.dwSatelliteCount = 0;
	number_of_GPS_Positions = 0;
	gpsLock = false;
	number = phoneNumber;
}



void CDisplayGPSData::ShowGPSMessage()
{
	TCHAR gpszMessage[160];

	_sntprintf(gpszMessage, sizeof(gpszMessage), TEXT("Latitude %2.6f \nLongitude %2.6f \nAltitude %2.2f \nSpeed %3.1f \nHeading %3.2f \nHorizontalQuality %3.1f \nVerticalQuality %3.1f \nSatellites %d"), 
		best_GPS_Position.dblLatitude, 
		best_GPS_Position.dblLongitude, 
		best_GPS_Position.flAltitudeWRTSeaLevel, 
		best_GPS_Position.flSpeed, 
		best_GPS_Position.flHeading, 
		best_GPS_Position.flHorizontalDilutionOfPrecision,
		best_GPS_Position.flVerticalDilutionOfPrecision,
		best_GPS_Position.dwSatelliteCount);

	MessageBox(NULL,
					gpszMessage, 
					L"GPS Lock",
					MB_OK);
}



void CDisplayGPSData::SendError()
{
	if(offlineMode == false){
		SendSMSError();
	}
	else{
		ShowGPSError();
	}
}



void CDisplayGPSData::SendSMSError()
{
	TCHAR gpszMessage[160];

	_sntprintf(gpszMessage, sizeof(gpszMessage), TEXT("GPS timed out."));

	LPTSTR gpszServer = (LPTSTR)L"";
	LPTSTR gpszRecipient = number;
	SendSMS(false, true, gpszServer, gpszRecipient, gpszMessage);
}


// TODO: Show actual GPS error
void CDisplayGPSData::ShowGPSError()
{
	TCHAR gpszMessage[160];

	_sntprintf(gpszMessage, sizeof(gpszMessage), TEXT("GPS timed out."));

	/*
	MessageBox(NULL,
					gpszMessage, 
					L"GPS Error",
					MB_OK);
	*/
}



bool CDisplayGPSData::AnyGPSLocationFound()
{
	if(best_GPS_Position.dwSatelliteCount > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}



void CDisplayGPSData::DisplayInformation()
{
	if(offlineMode == false){
		SendSMSMessage();
	}
	else{
		ShowGPSMessage();
	}

	gpsLock = true;
}

