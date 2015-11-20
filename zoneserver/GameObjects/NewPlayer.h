#pragma once
#include "BaseObject.h"
#include "NETWORKMODULE/SanguoPlayer.h"
const __int32	IID_NEWPLAYER = 0x110EFDBA;

class CNewPlayer :public IObject
{
public:
	CNewPlayer(void);
	~CNewPlayer(void);
	void Logout(bool SaveAndLogOut = false);	// 玩家退出
	void Logout(BYTE  *pDataBuff,DWORD  dLenth);	// 玩家退出
	void SendData(BOOL isSave);
	BOOL SetFixData(SFixData *pData); //设置三国玩家数据
	void SetAccount(const char * szSetAccount);
public:
	int AddObject(LPIObject pChild);
	int DelObject(LPIObject pChild);
	void OnClose();

public:
	DNID m_ClientIndex;
	bool m_bInit;
	std::string m_szAccount;
	DWORD m_nchecklivetime;
	struct SParameter
	{
		DNID    dnidClient;     // 连接编号
		DWORD   dwVersion;      // 玩家数据
	};

public:  //这部分数据主要是最后在存储数据库时候需要SFixData 结构，这里Player没有这个结果，所以需要记录部分数据创建
	DWORD   m_dwStaticID;					///< 玩家对服务器集群唯一静态的ID，由数据库服务器生成
	DWORD	m_version; ///<版本号
	char			m_Name[CONST_USERNAME];  //玩家姓名定义外部方便使用
	char			m_UPassword[CONST_USERPASS];///< 玩家二级密码
	BYTE			 m_byStoreFlag;					///< 玩家的当前状态，用于区分保存时的信息
	BYTE			m_bNewPlayer;

protected:
	void OnRun(void);
	int OnCreate(_W64 long pParameter);
};

