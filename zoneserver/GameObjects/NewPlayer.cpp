#include "StdAfx.h"
#include "NewPlayer.h"
#include "playermanager.h"
#include "ScriptManager.h"
#include "NETWORKMODULE/RefreshMsgs.h"

extern "C" {
#include "lauxlib.h"
}

extern BOOL SendRefreshPlayerMessage(int storeflag, DWORD gid, LPCSTR acc, SFixNewData *pData, SPlayerTempData *pTempData);
extern CScriptManager g_Script;
extern BOOL ClearClient(DNID);

CNewPlayer::CNewPlayer(void)
{
	rfalse(2, 1, "CNewPlayer");
}


CNewPlayer::~CNewPlayer(void)
{
}

void CNewPlayer::Logout(bool SaveAndLogOut /*= false*/)
{
	rfalse(2, 1, "CNewPlayer Logout");
	if (g_Script.PrepareFunction("NewPlayerLogout"))
	{
		g_Script.PushParameter(m_dwStaticID);
		g_Script.Execute();
	}
}

void CNewPlayer::Logout(BYTE *pDataBuff, DWORD  dLenth)
{
	if (pDataBuff && dLenth > 0)
	{
		SPlayerTempData stempdata;
		SFixNewData   Sfnewdata;
		Sfnewdata.m_dwStaticID =m_dwStaticID;
		Sfnewdata.m_version = m_version;
		memcpy(Sfnewdata.m_Name, m_Name, CONST_USERNAME);
		memcpy(Sfnewdata.m_UPassword, m_UPassword, CONST_USERPASS);
		Sfnewdata.m_byStoreFlag = m_byStoreFlag;
		Sfnewdata.m_bNewPlayer = m_bNewPlayer;
		Sfnewdata.dBufferLenth = dLenth;
		memcpy(Sfnewdata.bData, pDataBuff, dLenth);

		// 保存数据
		SendRefreshPlayerMessage(SARefreshPlayerMsg::SAVE_AND_LOGOUT, GetGID(), m_szAccount.c_str(), &Sfnewdata, &stempdata);

		ClearClient(m_ClientIndex);
		m_ClientIndex = 0;

		// 	// 将自己从世界中清除
		// 	if (m_ParentRegion)
		// 		m_ParentRegion->DelObject(PTemp);

		extern void RemoveFromCache(LPCSTR account);
		RemoveFromCache(m_szAccount.c_str());
		// 将自己设为无效，这将会被管理器清除
		m_bValid = false;

		if (m_bInit)
		{
			RemoveNameRelation(m_Name);
			RemoveSIDRelation(m_dwStaticID);
		}

		RemoveAccountRelation(m_szAccount.c_str());
	}
}

void CNewPlayer::SendData(BOOL isSave)
{
	rfalse(2, 1, "CNewPlayer SendData");
}

BOOL CNewPlayer::SetFixData(SFixData *pData)
{
	rfalse(2, 1, "CNewPlayer SetFixData");
	// 绑定一些查询用数据
	BindNameRelation(pData->m_Name, self.lock());
	BindSIDRelation(pData->m_dwStaticID, self.lock());

	size_t nsize = sizeof(SFixData);

	if (pData->m_bNewPlayer)//如果是新玩家
	{
		lua_State *ls = g_Script.ls;
		if (lua_gettop(ls) == 0)
		{
			lua_getglobal(ls, "NewPlayerSetFixData");
			if (lua_isnil(ls, -1))
			{
				lua_pop(ls, 1);
				return FALSE;
			}
			lua_pushinteger(ls, 1);
			lua_newtable(ls);
			g_Script.SetTableNumber(ls, "sid", pData->m_dwStaticID);
			if (lua_pcall(ls, 2, 0, 0) != 0)
			{
				char err[1024];
				sprintf(err, "%s\r\n", lua_tostring(ls, -1));
				rfalse(2, 1, err);
				lua_settop(ls, 0);
				return FALSE;
			}
			pData->m_bNewPlayer = FALSE;
		}
	}
	else
	{
		lua_State *ls = g_Script.ls;
		if (lua_gettop(ls) == 0)
		{
			lua_getglobal(ls, "NewPlayerSetFixData");
			if (lua_isnil(ls, -1))
			{
				//rfalse(2,1,FormatString("PrepareFunction %s not find \r\n",funcname));
				lua_pop(ls, 1);
				return FALSE;
			}
			lua_pushinteger(ls, 0);
			lua_newtable(ls);
			int ck = luaEx_unserialize(ls, pData->bData, pData->dBufferLenth);
			if (ck <= 0)
			{
				return FALSE;
			}
			g_Script.SetTableNumber(ls, "sid", pData->m_dwStaticID);
			lua_rawseti(ls, -2, 1);
			if (lua_pcall(ls,2, 0, 0) != 0)
			{
				char err[1024];
				sprintf(err, "%s\r\n", lua_tostring(ls, -1));
				rfalse(2, 1, err);
				lua_settop(ls, 0);
				return FALSE;
			}
		}
	}
	m_dwStaticID = pData->m_dwStaticID;
	m_version = pData->m_version;
	
	memcpy(m_Name, pData->m_Name, CONST_USERNAME);
	memcpy(m_UPassword, pData->m_UPassword,CONST_USERPASS);

	m_byStoreFlag = pData->m_byStoreFlag;
	m_bNewPlayer = pData->m_bNewPlayer;
	return TRUE;
}

void CNewPlayer::SetAccount(const char * szSetAccount)
{
	m_szAccount = szSetAccount;
}

int CNewPlayer::OnCreate(_W64 long pParameter)
{
	SParameter *pParam = (SParameter *)pParameter;
	rfalse(2, 1, "CNewPlayer OnCreate");
	return 1;
}

void CNewPlayer::OnRun(void)
{
	//rfalse(2, 1, "CNewPlayer OnRun");
	return;
}

int CNewPlayer::AddObject(LPIObject pChild)
{
	rfalse(2, 1, "CNewPlayer AddObject");
	return 1;
}

int CNewPlayer::DelObject(LPIObject pChild)
{
	rfalse(2, 1, "CNewPlayer DelObject");
	return 1;
}

void CNewPlayer::OnClose()
{
	rfalse(2, 1, "CNewPlayer OnClose");
	return;
}
