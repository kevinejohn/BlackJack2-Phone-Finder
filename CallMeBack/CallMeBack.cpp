// CallMeBack.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <astdtapi.h>


int _tmain(int argc, _TCHAR* argv[])
{

	if(argc == 2){
		LONG lResult;
		lResult = tapiRequestMakeCall(argv[1], NULL, NULL, NULL);
		return (0 == lResult);
	}


	return 0;
}

