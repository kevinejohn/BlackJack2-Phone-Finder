// Status.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sms.h> 
#include <Winbase.h>
#include <Regext.h>

void SendSMSMessage(int battery, int signalPercent, LPTSTR number);
void ShowPhoneStatus(int battery, int signalPercent);
void SendSMS(BOOL bSendConfirmation, BOOL bUseDefaultSMSC, LPCTSTR lpszSMSC, LPCTSTR lpszRecipient, LPCTSTR lpszMessage);
DWORD GetCellSignalPercent();
int GetBatteryPercent();


int _tmain(int argc, _TCHAR* argv[])
{
	int battery = GetBatteryPercent();
	int signalPercent = GetCellSignalPercent();

	if(argc == 2){
		SendSMSMessage(battery, signalPercent, argv[1]);
	}
	else{
		ShowPhoneStatus(battery, signalPercent);
	}


	return 0;
}


void SendSMSMessage(int battery, int signalPercent, LPTSTR number)
{

	TCHAR gpszMessage[160];

	if(battery == 255){
		_sntprintf(gpszMessage, sizeof(gpszMessage), TEXT("Battery life: UNKNOWN, Signal Strength: %d percent"), signalPercent );
	}
	else{		
		_sntprintf(gpszMessage, sizeof(gpszMessage), TEXT("Battery life: %d percent, Signal Strength: %d percent"), battery, signalPercent );
	}

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


void ShowPhoneStatus(int battery, int signalPercent)
{
	TCHAR gpszMessage[161];

	if(battery == 255){
		_sntprintf(gpszMessage, sizeof(gpszMessage), TEXT("Battery life: UNKNOWN \nSignal Strength: %d percent"), signalPercent  );
	}
	else{		
		_sntprintf(gpszMessage, sizeof(gpszMessage), TEXT("Battery life: %d percent \nSignal Strength: %d percent"), battery , signalPercent);
	}

	MessageBox(NULL,
		gpszMessage, 
		L"Phone Status",
		MB_OK);
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

void SendSMS(BOOL bSendConfirmation, BOOL bUseDefaultSMSC, LPCTSTR lpszSMSC, LPCTSTR lpszRecipient, LPCTSTR lpszMessage)
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


// Get cell signal percent from registry
DWORD GetCellSignalPercent()
{
	HKEY SN_SIGNALSTRENGTHRAW_ROOT = HKEY_LOCAL_MACHINE;
	LPCTSTR SN_SIGNALSTRENGTHRAW_PATH = L"\\System\\State\\Phone";
	LPCTSTR SN_SIGNALSTRENGTHRAW_VALUE = L"Signal Strength Raw";

    DWORD cCellSignalPercent = 0;
    HRESULT hr;

    hr = RegistryGetDWORD(SN_SIGNALSTRENGTHRAW_ROOT, 
                          SN_SIGNALSTRENGTHRAW_PATH, 
                          SN_SIGNALSTRENGTHRAW_VALUE, 
                          &cCellSignalPercent);

    if (SUCCEEDED(hr))
        return cCellSignalPercent;
    else
	{
		/*
		MessageBox(NULL,
				   L"Failed", 
				   L"Failed",
				   MB_OK);
		*/
        return 0;
	}
}


int GetBatteryPercent()
{
	int battery = 255;
	SYSTEM_POWER_STATUS_EX* powerStatus = new SYSTEM_POWER_STATUS_EX();

	if(GetSystemPowerStatusEx(powerStatus, true))
	{
		battery = powerStatus->BatteryLifePercent;
	}
	
	return battery;
}


