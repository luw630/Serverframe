//#######################
//# xo checked at 09-24 #
//#######################
#include "NetProviderModule.h"

#include <crtdbg.h>

#define USE_CRC
#include "new_net/desdec.h"
#include "new_net/netserverex.cpp"

#define DOTHROW

extern BOOL rfalse(char lTraceType, char bUseEndl, LPCSTR szFormat, ...);

CNetProviderModule::CNetProviderModule(BOOL bUseVerify) :
    CNetServerEx(bUseVerify)
{

}

CNetProviderModule::~CNetProviderModule()
{

}

int CNetProviderModule::Destroy()
{
    Close();
    return 1;
}

BOOL CNetProviderModule::SendMessage(DNID dnidClient, const void *pvBuf, DWORD wSize)
{
	return m_netadd.SendMessage(this, dnidClient, pvBuf, wSize);
}

BOOL CNetProviderModule::SendMessageTo(DNID dnidClient, const void *pvBuf, DWORD wSize)
{
	return __super::SendMessage(dnidClient, pvBuf, wSize);
}

