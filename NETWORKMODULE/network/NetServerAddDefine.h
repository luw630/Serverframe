#pragma once

#define MAX_SEND_BUFFER	0xea00
#define BUFFERMESSAGESIZE 0x100000  
#define LIMITLENTH  0x9e00
typedef struct SNETPACKETINFO
{
	WORD  m_wPacketNum;
	WORD    m_wLength;
	WORD	m_wIndex;
	BYTE   m_bBuffer[MAX_SEND_BUFFER];
}SNetPacketInfo;