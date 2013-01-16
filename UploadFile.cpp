// UploadFile.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "UploadInterface.h"
#include "FileClient.h"
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" __declspec(dllexport) 

ISendFileInterface * CreateSendFile()
{	
	return new CFileClient();
}

extern "C"  __declspec(dllexport) 
void DeleteSendFile( ISendFileInterface * pFileClient)
{
	try
	{
		if(pFileClient) delete pFileClient;
	}
	catch(...)
	{
		printf("this is a catch\n");
	}
}