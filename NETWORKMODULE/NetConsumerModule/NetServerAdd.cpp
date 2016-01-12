#include "stdafx.h"
#include "NetServerAdd.h"



extern BOOL rfalse(char, char, LPCSTR, ...);

CNetServerAdd::CNetServerAdd()
{
	m_bResolveFinish = false;
	memset(m_Buffer, 0, BUFFERMESSAGESIZE);
	m_BuffPoint = m_Buffer;
}

CNetServerAdd::~CNetServerAdd()
{
	m_BuffPoint = nullptr;
}

// BOOL CNetServerAdd::SendMessage(CNetServerEx  *pserverex, DNID dnidClient, const void *pvBuf, DWORD wSize)
// {
// 	if (!pserverex)
// 	{
// 		return FALSE;
// 	}
// 	SNetPacketInfo  sPackInfo;
// 	memset(sPackInfo.m_bBuffer, 0, MAX_SEND_BUFFER);
// 
// 	sPackInfo.m_wPacketNum = wSize / LIMITLENTH + 1;
// 
// 	BYTE  *pTemp = (BYTE*)pvBuf;
// 	while (true)
// 	{
// 		if (pTemp - pvBuf < wSize)
// 		{
// 			WORD  cpylenth = LIMITLENTH;
// 			if (pTemp + LIMITLENTH - pvBuf > wSize)
// 			{
// 				cpylenth = pTemp + LIMITLENTH - pvBuf - wSize;
// 			}
// 			memset(sPackInfo.m_bBuffer, 0, MAX_SEND_BUFFER);
// 			memcpy(sPackInfo.m_bBuffer, pTemp, cpylenth);
// 			pTemp += cpylenth;
// 			sPackInfo.m_wIndex++;
// 			sPackInfo.m_wLength = cpylenth;
// 
// 			WORD sendsize = sizeof(SNETPACKETINFO) - MAX_SEND_BUFFER + cpylenth;
// 
// 			CNetProviderModule  *pProviderModule = static_cast<CNetProviderModule*>(pserverex);
// 			if (pProviderModule)
// 			{
// 				pProviderModule->SendMessageTo(dnidClient, &sPackInfo, sendsize);
// 			}
// 		}
// 		else
// 		{
// 			break;
// 		}
// 	}
// 	return TRUE;
// 
// }

BOOL CNetServerAdd::SendMessage(CNetClientEx  *petclientex, const void *pvBuf, DWORD wSize)
{
	if (!petclientex)
	{
		return FALSE;
	}
	CNetConsumerModule  *pProviderModule = static_cast<CNetConsumerModule*>(petclientex);
	if (!pProviderModule)
	{
		return FALSE;
	}

	SNetPacketInfo  sPackInfo;
	memset(&sPackInfo, 0, sizeof(SNetPacketInfo));

	sPackInfo.m_wPacketNum = wSize / LIMITLENTH + 1;

	BYTE  *pTemp = (BYTE*)pvBuf;
	while (true)
	{
		if (wSize <= LIMITLENTH)
		{
			memset(sPackInfo.m_bBuffer, 0, MAX_SEND_BUFFER);
			memcpy(sPackInfo.m_bBuffer, pTemp, wSize);
			pTemp += wSize;
			sPackInfo.m_wIndex++;
			sPackInfo.m_wLength = wSize;
			WORD sendsize = sizeof(SNETPACKETINFO) - MAX_SEND_BUFFER + wSize;
			if (!pProviderModule->SendMessageTo(&sPackInfo, sendsize))
			{
				rfalse(2, 1, "SendMessage Faile onIndex %d  Send:%d \n", sPackInfo.m_wIndex, sendsize);
				return FALSE;
			}
			break;
		}
		else
		{
			if (pTemp - pvBuf < wSize)
			{
				WORD  cpylenth = LIMITLENTH;
				if (pTemp + LIMITLENTH - pvBuf > wSize)
				{
					cpylenth = wSize - (pTemp - pvBuf);
				}
				memset(sPackInfo.m_bBuffer, 0, MAX_SEND_BUFFER);
				memcpy(sPackInfo.m_bBuffer, pTemp, cpylenth);
				pTemp += cpylenth;
				sPackInfo.m_wIndex++;
				sPackInfo.m_wLength = cpylenth;

				WORD sendsize = sizeof(SNETPACKETINFO) - MAX_SEND_BUFFER + cpylenth;
				if (!pProviderModule->SendMessageTo(&sPackInfo, sendsize))
				{
					rfalse(2, 1, "SendMessage Faile onIndex %d  Send:%d \n", sPackInfo.m_wIndex, sendsize);
					return FALSE;
				}
			}
			else
			{
				break;
			}
		}

	}
	return TRUE;
// 	memcpy(sPackInfo.m_bBuffer, pvBuf, wSize);
// 	sPackInfo.m_wPacketNum = 1;
// 	sPackInfo.m_wLength = wSize;

}

BOOL CNetServerAdd::ResolveMessage(LPVOID  pMessage, WORD wLenth, BYTE  **mergeMge, DWORD  &dtotallenth)
{
	SNetPacketInfo   sPackInfo;
	memset(&sPackInfo, 0, sizeof(SNetPacketInfo));
	memcpy(&sPackInfo, pMessage, wLenth);

	if (sPackInfo.m_wIndex == 1 )
	{
		memset(m_Buffer, 0, BUFFERMESSAGESIZE);
		m_bResolveFinish = false;
		m_BuffPoint = m_Buffer;
	}
	memcpy(m_BuffPoint, sPackInfo.m_bBuffer, sPackInfo.m_wLength);
	m_BuffPoint += sPackInfo.m_wLength;
	if (sPackInfo.m_wPacketNum == sPackInfo.m_wIndex)
	{
		m_bResolveFinish = true;
	}

	if (m_bResolveFinish)
	{
		dtotallenth = m_BuffPoint - m_Buffer;
		*mergeMge = new BYTE[dtotallenth];
		memcpy(*mergeMge, m_Buffer, dtotallenth);
		m_bResolveFinish = false;
		return TRUE;
	}
	return m_bResolveFinish;
}


BOOL CNetServerAdd::GetResolveMessage(BYTE **pMessage)
{
	DWORD  dlenth = m_BuffPoint - m_Buffer;
	*pMessage = new BYTE[dlenth];
	memcpy(*pMessage, m_Buffer, dlenth);
	return TRUE;
}
