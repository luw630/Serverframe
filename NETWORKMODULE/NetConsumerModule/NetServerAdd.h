//---------------------------------------------------------------------------
//  Engine (c) 1999-2002 by Dreamwork
//
// File:	CNetServerEx.h
// Date:	2002.07.10, 2003.04.23
// Code:	qiuxin, luolin
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef	NetServerADD_H
#define	NetServerADD_H
//---------------------------------------------------------------------------
#include <new_net/netclientex.h>
#include "../network/NetServerAddDefine.h"


class CNetServerAdd 
{
public:
	CNetServerAdd(void);
	~CNetServerAdd(void);

	//BOOL SendMessage(CNetServerEx  *pserverex, DNID dnidClient, const void *pvBuf, DWORD wSize);
	BOOL SendMessage(CNetClientEx  *petclientex, const void *pvBuf, DWORD wSize);
	BOOL  ResolveMessage(LPVOID  pMessage, WORD  wLenth, BYTE  **mergeMge, DWORD  &dtotallenth);

	BOOL   GetResolveMessage(BYTE  **pMessage);
private:

	bool   m_bResolveFinish;
	BYTE   m_Buffer[BUFFERMESSAGESIZE];
	BYTE    *m_BuffPoint;

	SNetPacketInfo   m_sPackInfo;

};
//---------------------------------------------------------------------------
#endif
