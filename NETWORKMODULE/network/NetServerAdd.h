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
#include <new_net/netserverex.h>
#include <new_net/netclientex.h>


#define MAX_SEND_BUFFER	0xea00
#define BUFFERMESSAGESIZE 0x100000  
#define LIMITLENTH  0x0a
typedef struct SNETPACKETINFO
{
	WORD  m_wPacketNum;
	WORD    m_wLength;
	WORD	m_wIndex;
	BYTE   m_bBuffer[MAX_SEND_BUFFER];
}SNetPacketInfo;


class CNetServerAdd 
{
public:
	CNetServerAdd(void);
	~CNetServerAdd(void);

	BOOL SendMessage(CNetServerEx  *pserverex, DNID dnidClient, const void *pvBuf, DWORD wSize);
	BOOL SendMessage(CNetClientEx  *petclientex, const void *pvBuf, DWORD wSize);
	BOOL  ResolveMessage(LPVOID  pMessage, WORD  wLenth, BYTE  **mergeMge, DWORD  &dtotallenth);

	BOOL   GetResolveMessage(BYTE  **pMessage);
private:

	bool   m_bResolveFinish;
	BYTE   m_Buffer[BUFFERMESSAGESIZE];
	BYTE    *m_BuffPoint;

};
//---------------------------------------------------------------------------
#endif
