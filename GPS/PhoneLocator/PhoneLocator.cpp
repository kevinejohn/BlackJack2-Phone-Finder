// PhoneLocator.cpp : Defines the entry point for the console application.
//

#pragma once
#include "stdafx.h"
#include "..\MobileGPSDemo\GPSSink.h"
#include <windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	/*
	MessageBox(NULL,
			L"Opened", 
			L"Opening Program",
			MB_OK);
	*/
	
	LPTSTR phoneNumber;
	int timeout = 0;

	// First argument should be phone number to SMS back
	if(argc != 2){
		phoneNumber = L"";
	}
	else{
		phoneNumber = argv[1];
	}


	CGPSSink *pGPSSink = NULL;
	CGPSController *pGPSController = NULL;

	HRESULT hr = E_FAIL;

	// Both CGPSSink and CGPSController instance are dynamically allocated
    // so that they can be shared with the worker thread
	pGPSSink = new CGPSSink();
    if (NULL == pGPSSink)
    {
        goto Exit;
    }
	pGPSSink->m_DisplayGPSInfo.Initialize(phoneNumber);

	pGPSController = new CGPSController();
    if (NULL == pGPSController)
    {
        goto Exit;
    }

	hr = pGPSController->InitDevice(pGPSSink);
    if (FAILED(hr))
    {
        goto Exit;
    }

	// Check if GPS location was found and SMS sent.
	// Or send timeout message after 10 minutes (600 seconds) if GPS position was not found.
	while((!pGPSSink->m_DisplayGPSInfo.DidSMSSend()) && (timeout < 600) ){
		timeout++;
		Sleep(1000);
	}

// Cleanup 
Exit:
	if ((NULL != pGPSSink) && (!pGPSSink->m_DisplayGPSInfo.DidSMSSend()))
    {		
		if(pGPSSink->m_DisplayGPSInfo.AnyGPSLocationFound())
		{
			pGPSSink->m_DisplayGPSInfo.DisplayInformation();
		}
		else
		{
			pGPSSink->m_DisplayGPSInfo.SendError();
		}
	}

	// Shutdown GPS
	if (NULL != pGPSController)
    {
	    pGPSController->UninitDevice();
    }

    // It is ok to call delete on a NULL pointer
	delete pGPSSink;
	delete pGPSController;

/*
			MessageBox(NULL,
					L"Closed", 
					L"Closing Program",
					MB_OK);
*/

	return 0;
}

