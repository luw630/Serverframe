#pragma once
#include <new_net/netclientex.h>
#include "NetServerAdd.h"

// pEntry 必须是 int __thiscall Entry(void *pMsg); 类型的函数，并且不能是虚函数
// 或者 pEntry 是 int __callback Entry(void *pMsg); 类型的函数

//模块说明：
//基于 NetClient的一个模块，由CNetModule继承而来，负责处理client端的数据
//Initialize 作为初始化函数
class CNetConsumerModule : 
    public CNetClientEx
{
public:
	 CNetServerAdd  m_netserveradd;
    // 流程控制函数
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
    // 构造析构
    CNetConsumerModule(BOOL bUseVerify = FALSE);
    ~CNetConsumerModule(void);

	
};


