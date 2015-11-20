#pragma once
#include "NetProviderAdd.h"
#include <new_net/netserverex.h>

typedef unsigned __int64 QWORD;


// pEntry ������ int __thiscall Entry(unsigned __int64 qwDnid, LINKSTATUS enumStatus, void *pMsg); ���͵ĺ���
// ���� pEntry �� int __callback Entry(unsigned __int64 qwDnid, LINKSTATUS enumStatus, void *pMsg); ���͵ĺ���

//ģ��˵����
//���� NetServer��һ��ģ�飬��CNetModule�̳ж�����������server�˵�����
//Initialize ��Ϊ��ʼ������
class CNetProviderModule : 
    public CNetServerEx
{
public:
    template < typename op >
    int Execution( op _entry )
    {
        SNetServerPacket *pPacket = NULL;

        extern DWORD dwNetRunLimit;
        DWORD segTime = timeGetTime() + dwNetRunLimit;

        while ( (int)( segTime - timeGetTime() ) > 0 )
        {
            if ( GetMessage( &pPacket ) == FALSE )
                break;

			BYTE  *mergeMge;
			DWORD  dtotallenth = 0;
			if (m_netadd.ResolveMessage(pPacket->m_pvBuffer, pPacket->m_wLength, &mergeMge, dtotallenth))
			{
				if (dtotallenth > 0)
				{
					bool ret = _entry(pPacket->m_dnidClient,
						pPacket->m_enumStatus, mergeMge, dtotallenth);
				}
			}
			if (pPacket->m_pvBuffer)
				delete pPacket->m_pvBuffer;
			delete pPacket;
        }

        return 1;
    }

    int Destroy(void);


	BOOL SendMessage(DNID dnidClient, const void *pvBuf, DWORD wSize);

	BOOL SendMessageTo(DNID dnidClient, const void *pvBuf, DWORD wSize);
    // ��������
    CNetProviderModule(BOOL bUseVerify = FALSE);
    ~CNetProviderModule(void);

	CNetProviderAdd  m_netadd;
	
}; 

