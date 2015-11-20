#pragma once
#include <new_net/netclientex.h>
#include "NetServerAdd.h"

// pEntry ������ int __thiscall Entry(void *pMsg); ���͵ĺ��������Ҳ������麯��
// ���� pEntry �� int __callback Entry(void *pMsg); ���͵ĺ���

//ģ��˵����
//���� NetClient��һ��ģ�飬��CNetModule�̳ж�����������client�˵�����
//Initialize ��Ϊ��ʼ������
class CNetConsumerModule : 
    public CNetClientEx
{
public:
	 CNetServerAdd  m_netserveradd;
    // ���̿��ƺ���
    template < typename op >
    int Execution( op _entry )
    {
        SNetClientPacket *pPacket = NULL;

        extern DWORD dwNetRunLimit;
        DWORD segTime = timeGetTime() + dwNetRunLimit;

        while ( (int)( segTime - timeGetTime() ) > 0 )
        {
            if ( GetMessage( &pPacket ) == FALSE )
                break;

			BYTE  *mergeMge;
			DWORD  dtotallenth = 0;
			if (m_netserveradd.ResolveMessage(pPacket->m_pvBuffer, pPacket->m_wLength, &mergeMge, dtotallenth))
			{
				if (dtotallenth > 0)
				{
					bool ret = _entry( mergeMge, dtotallenth);
				}
			}
			if (pPacket->m_pvBuffer)
				delete pPacket->m_pvBuffer;
            delete pPacket;
        }

        return 1;
    }

    int Destroy(void);

	BOOL SendMessage(const void *pvBuf, DWORD wSize);
	BOOL SendMessageTo(const void *pvBuf, DWORD wSize);
    // ��������
    CNetConsumerModule(BOOL bUseVerify = FALSE);
    ~CNetConsumerModule(void);

	
};


