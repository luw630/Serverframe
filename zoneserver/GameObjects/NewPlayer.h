#pragma once
#include "BaseObject.h"
#include "NETWORKMODULE/SanguoPlayer.h"
const __int32	IID_NEWPLAYER = 0x110EFDBA;

class CNewPlayer :public IObject
{
public:
	CNewPlayer(void);
	~CNewPlayer(void);
	void Logout(bool SaveAndLogOut = false);	// ����˳�
	void Logout(BYTE  *pDataBuff,DWORD  dLenth);	// ����˳�
	void SendData(BOOL isSave);
	BOOL SetFixData(SFixData *pData); //���������������
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
		DNID    dnidClient;     // ���ӱ��
		DWORD   dwVersion;      // �������
	};

public:  //�ⲿ��������Ҫ������ڴ洢���ݿ�ʱ����ҪSFixData �ṹ������Playerû����������������Ҫ��¼�������ݴ���
	DWORD   m_dwStaticID;					///< ��ҶԷ�������ȺΨһ��̬��ID�������ݿ����������
	DWORD	m_version; ///<�汾��
	char			m_Name[CONST_USERNAME];  //������������ⲿ����ʹ��
	char			m_UPassword[CONST_USERPASS];///< ��Ҷ�������
	BYTE			 m_byStoreFlag;					///< ��ҵĵ�ǰ״̬���������ֱ���ʱ����Ϣ
	BYTE			m_bNewPlayer;

protected:
	void OnRun(void);
	int OnCreate(_W64 long pParameter);
};

