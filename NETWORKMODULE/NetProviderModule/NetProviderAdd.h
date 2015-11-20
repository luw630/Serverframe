//---------------------------------------------------------------------------
//  Engine (c) 1999-2002 by Dreamwork
//
// File:	CNetServerEx.h
// Date:	2002.07.10, 2003.04.23
// Code:	qiuxin, luolin
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef	NetPROVIDERADD_H
#define	NetPROVIDERADD_H
//---------------------------------------------------------------------------
#include <new_net/NetServerEx.h>
#include "../network/NetServerAddDefine.h"


class CNetProviderAdd 
{
public:
	CNetProviderAdd(void);
	~CNetProviderAdd(void);

	BOOL SendMessage(CNetServerEx  *pserverex, DNID dnidClient, const void *pvBuf, DWORD wSize);
	
	BOOL  ResolveMessage(LPVOID  pMessage, WORD  wLenth, BYTE  **mergeMge, DWORD  &dtotallenth);

	BOOL   GetResolveMessage(BYTE  **pMessage);
private:

	bool   m_bResolveFinish;
	BYTE   m_Buffer[BUFFERMESSAGESIZE];
	BYTE    *m_BuffPoint;

};
//---------------------------------------------------------------------------
#endif
