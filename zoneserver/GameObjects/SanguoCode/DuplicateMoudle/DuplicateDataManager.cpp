#include "stdafx.h"
#include "time.h"
#include "..\BaseDataManager.h"
#include "Networkmodule\SanguoPlayerMsg.h"
#include "NETWORKMODULE\SanguoPlayer.h"
#include "..\Common\PubTool.h"
#include "DuplicateDataManager.h"
#include "..\TimerEvent_SG\TimerEvent_SG.h"
#include "extraScriptFunctions/lite_lualibrary.hpp"
#include "Player.h"
#include "ScriptManager.h"
#include "../GuideMoudle/GuideManager.h"
extern LPIObject GetPlayerBySID(DWORD dwStaticID);

///��ʱ�ڱ���������Щ������ؿ��ĵ�һ��ID
#define  TreausreHuntingDuplicateBeginID 800001
#define  LeagueOfLegendDuplicateBeginID 700001
#define  NormalTollgateBeginID 100000
#define  EliteTollgateBeginID 200000
#define  TreasureHuntingTollgateBeginID 800001
#define  LeagueOfLegendTollgateBeginID 700001

CDuplicateDataManager::CDuplicateDataManager(CBaseDataManager& baseDataManager)
	:CExtendedDataManager(baseDataManager)
{
	m_playerData = nullptr;

	m_iLatestNormalTollgateID = 0;
	m_iLatestEliteTollgateID = 0;
	m_iCurHeroExpGain = 0;
	m_listSelectedHero.clear();
	m_normalTollgateDatas.clear();
	m_eliteTollgateDatas.clear();
	m_treasureHuntingTollgateDatas.clear();
	m_leagueOfLegendTollgateDatas.clear();
	m_treasureHuntingDuplicateChallengedTimes.clear();
	m_leagueOfLegendDuplicateChallengedTiems.clear();
}


CDuplicateDataManager::~CDuplicateDataManager()
{
	m_playerData = nullptr;
}

bool CDuplicateDataManager::InitDataMgr(void * pData)
{
	return true;
}

bool CDuplicateDataManager::ReleaseDataMgr()
{
	m_playerData = nullptr;
	return true;
}

void CDuplicateDataManager::RefreshData(int sendMsg /* = 0 */)
{
}

int CDuplicateDataManager::GetDuplicateChallengedTimes(InstanceType type, int duplicateID)
{
	duplicateChallengedTimesItor findResult;
	switch (type)
	{
	case InstanceType::LeagueOfLegends:///��ȡ����˵��������ս��������
		findResult = m_leagueOfLegendDuplicateChallengedTiems.find(duplicateID);
		if (findResult != m_leagueOfLegendDuplicateChallengedTiems.end())
			return *(findResult->second);
		break;
	case InstanceType::TreasureHunting:///��ȡ�ؾ�Ѱ����������ս��������
		findResult = m_treasureHuntingDuplicateChallengedTimes.find(duplicateID);
		if (findResult != m_treasureHuntingDuplicateChallengedTimes.end())
			return *(findResult->second);
		break;
	}

	return 0;
}

void CDuplicateDataManager::GetTollgateData(InstanceType type, int tollgateID, OUT int& starLevel, OUT int& challengeTime)
{
	commonTollgateDataItor findResult;
	storyEliteTollgateDataItor eliteFindresult;
	switch (type)
	{
	case InstanceType::StoryElite:///��ȡ��ͨ�������������¸������ؿ�������
		eliteFindresult = m_eliteTollgateDatas.find(tollgateID);
		if (eliteFindresult != m_eliteTollgateDatas.end() && eliteFindresult->second != nullptr)
		{
			starLevel = eliteFindresult->second->tollgateData.m_dwStarLevel;
			challengeTime = eliteFindresult->second->tollgateData.m_dwChallengeTime;
		}
		break;
	case InstanceType::LeagueOfLegends:///��ȡ����˵�����ؿ�������
		findResult = m_leagueOfLegendTollgateDatas.find(tollgateID);
		if (findResult != m_leagueOfLegendTollgateDatas.end() && findResult->second != nullptr)
		{
			starLevel = findResult->second->m_dwStarLevel;
			challengeTime = findResult->second->m_dwChallengeTime;
		}
		break;
	case InstanceType::TreasureHunting:///��ȡ�ؾ�Ѱ�������ؿ�������
		findResult = m_treasureHuntingTollgateDatas.find(tollgateID);
		if (findResult != m_treasureHuntingTollgateDatas.end() && findResult->second != nullptr)
		{
			starLevel = findResult->second->m_dwStarLevel;
			challengeTime = findResult->second->m_dwChallengeTime;
		}
		break;
	}
}

void CDuplicateDataManager::GetStroyTollgateData(int tollgateID, OUT bool& bFirstTime, OUT int& starLevel, OUT int& challengeTime)
{
	auto findResult = m_normalTollgateDatas.find(tollgateID);
	if (findResult == m_normalTollgateDatas.end() || findResult->second == nullptr)
		return;

	bFirstTime = findResult->second->bFirstTime;
	starLevel = findResult->second->tollgateData.m_dwStarLevel;
	challengeTime = findResult->second->tollgateData.m_dwChallengeTime;
}

void CDuplicateDataManager::UpdateTollgateData(InstanceType type, int duplicateID, int tollgateID, int starLevel, int challengeTime)
{
	if (starLevel < 0)
		starLevel = 0;

	if (challengeTime < 0)
		challengeTime = 0;

	storyTollgateDataItor storyFindResult;
	storyEliteTollgateDataItor eliteFindresult;
	commonTollgateDataItor commonFindResult;
	duplicateChallengedTimesItor findResult;

	switch (type)
	{
	case InstanceType::Story:///������ͨ�������������¸������ؿ����������
		storyFindResult = m_normalTollgateDatas.find(tollgateID);
		if (storyFindResult != m_normalTollgateDatas.end() && storyFindResult->second != nullptr)
		{
			storyFindResult->second->bFirstTime = false;

			storyFindResult->second->tollgateData.m_dwStarLevel = max(storyFindResult->second->tollgateData.m_dwStarLevel, starLevel);
			storyFindResult->second->tollgateData.m_dwChallengeTime += challengeTime;
		}
		else
			rfalse("��ͨ�����ؿ����������¸����ؿ���%d�ڸ������ݹ���ģ�����Ҳ���", tollgateID);
		break;
	case InstanceType::StoryElite:///���¾�Ӣ�����ؿ����������
		eliteFindresult = m_eliteTollgateDatas.find(tollgateID);
		if (eliteFindresult != m_eliteTollgateDatas.end() && eliteFindresult->second != nullptr)
		{
			eliteFindresult->second->tollgateData.m_dwStarLevel = max(starLevel, eliteFindresult->second->tollgateData.m_dwStarLevel);
			eliteFindresult->second->tollgateData.m_dwChallengeTime += challengeTime;
		}
		else
			rfalse("��Ӣ�����ؿ�%d�ڸ������ݹ���ģ�����Ҳ���", tollgateID);
		break;
	case InstanceType::LeagueOfLegends:///���½���˵�����ؿ����������
		commonFindResult = m_leagueOfLegendTollgateDatas.find(tollgateID);
		if (commonFindResult != m_leagueOfLegendTollgateDatas.end() && commonFindResult->second != nullptr)
		{
			commonFindResult->second->m_dwStarLevel = max(starLevel, commonFindResult->second->m_dwStarLevel);
			commonFindResult->second->m_dwChallengeTime += challengeTime;
		}
		else
			rfalse("����˵�����ؿ�%d�ڸ������ݹ���ģ�����Ҳ���", tollgateID);

		findResult = m_leagueOfLegendDuplicateChallengedTiems.find(duplicateID);
		if (findResult != m_leagueOfLegendDuplicateChallengedTiems.end() && findResult->second != nullptr)
		{
			*(findResult->second) += challengeTime;
		}
		else
			rfalse("����˵����%d�ڸ������ݹ���ģ�����Ҳ���", duplicateID);
		break;
	case InstanceType::TreasureHunting:///�����ؾ�Ѱ�������ؿ����������
		commonFindResult = m_treasureHuntingTollgateDatas.find(tollgateID);
		if (commonFindResult != m_treasureHuntingTollgateDatas.end() && commonFindResult->second != nullptr)
		{
			commonFindResult->second->m_dwStarLevel = max(starLevel, commonFindResult->second->m_dwStarLevel);
			commonFindResult->second->m_dwChallengeTime += challengeTime;
		}
		else
			rfalse("�ؾ�Ѱ�������ؿ�%d�ڸ������ݹ���ģ�����Ҳ���", tollgateID);

		findResult = m_treasureHuntingDuplicateChallengedTimes.find(duplicateID);
		if (findResult != m_treasureHuntingDuplicateChallengedTimes.end() && findResult->second != nullptr)
		{
			*(findResult->second) += challengeTime;
		}
		else
			rfalse("�ؾ�Ѱ������%d�ڸ������ݹ���ģ�����Ҳ���", duplicateID);
		break;
	}
}

void CDuplicateDataManager::UpdateSelectedHero(int heroNum, const int heroArray[])
{
	if (heroArray == nullptr)
		return;

	m_listSelectedHero.clear();

	try
	{
		for (int heroIndex = 0; heroIndex < heroNum; ++heroIndex)
		{
			m_listSelectedHero.push_back(heroArray[heroIndex]);
		}
	}
	catch (const std::exception& oor)
	{
		rfalse("�����������ѡ���Ӣ�۵�ʱ�򱨴�����������Խ��,�����ʱ�򲻻�����Ӣ�ۼӾ���");
	}
}

const list<int>& CDuplicateDataManager::GetSelectedHeroList()
{
	return m_listSelectedHero;
}

void CDuplicateDataManager::UpdateLatestNormalTollgateID(int tollgateID)
{

}

void CDuplicateDataManager::UpdateLatestEliteTollgateID(int tollgateID)
{

}

bool CDuplicateDataManager::ResetSpecifyTollgateChallegedTimes(InstanceType instanceType, int tollgateID)
{
	storyTollgateDataItor storyFindResult;
	storyEliteTollgateDataItor eliteFindresult;
	commonTollgateDataItor commonFindResult;

	///��ʱ��VIP�ȼ��Ļ�ȡ�����ŵ�����,��ΪVIP�ĵȼ�����ʱ������䣬���ԡ�ʵʱ����ȡ
	CPlayer* pPlayer = (CPlayer *)GetPlayerBySID(m_pBaseDataMgr.GetSID())->DynamicCast(IID_PLAYER);
	int maxResetTimes = 0;
	if (pPlayer != nullptr)
	{
		g_Script.SetCondition(0, pPlayer, 0);
		lite::Variant ret;//��lua��ȡ��ǰVIP�ȼ�������ĳһ���ؿ���������
		LuaFunctor(g_Script, "SI_vip_getDetail")[g_Script.m_pPlayer->GetSID()][VipLevelFactor::VF_ReSetPass_Num](&ret);
		maxResetTimes = (int)ret;
		g_Script.CleanCondition();
	}
	else
		rfalse("��ȡ����CPlayer��ָ��");

	switch (instanceType)
	{
	case Story:
		storyFindResult = m_normalTollgateDatas.find(tollgateID);
		if (storyFindResult != m_normalTollgateDatas.end() && storyFindResult->second != nullptr)
		{
			storyFindResult->second->tollgateData.m_dwChallengeTime = 0;
		}
		break;
	case StoryElite:
		eliteFindresult = m_eliteTollgateDatas.find(tollgateID);
		if (eliteFindresult != m_eliteTollgateDatas.end() && eliteFindresult->second != nullptr)
		{
			if (eliteFindresult->second->dwResetedTimes < maxResetTimes)
				++eliteFindresult->second->dwResetedTimes;
			else
				return false;
			eliteFindresult->second->tollgateData.m_dwChallengeTime = 0;
		}
		break;
	case LeagueOfLegends:
		commonFindResult = m_leagueOfLegendTollgateDatas.find(tollgateID);
		if (commonFindResult != m_leagueOfLegendTollgateDatas.end() && commonFindResult->second != nullptr)
			commonFindResult->second->m_dwChallengeTime = 0;
		break;
	case TreasureHunting:
		commonFindResult = m_treasureHuntingTollgateDatas.find(tollgateID);
		if (commonFindResult != m_treasureHuntingTollgateDatas.end() && commonFindResult->second != nullptr)
			commonFindResult->second->m_dwChallengeTime = 0;
		break;
	default:
		break;
	}

	return true;
}

int CDuplicateDataManager::GetSpecifyTollgateResetedTimes(InstanceType instanceType, int tollgateID)
{
	storyTollgateDataItor storyFindResult;
	storyEliteTollgateDataItor eliteFindresult;
	commonTollgateDataItor commonFindResult;

	switch (instanceType)
	{
	case StoryElite:///��ʱֻ�о�Ӣ�ؿ��������ô���
		eliteFindresult = m_eliteTollgateDatas.find(tollgateID);
		if (eliteFindresult != m_eliteTollgateDatas.end() && eliteFindresult->second != nullptr)
		{
			return eliteFindresult->second->dwResetedTimes;
		}
		break;
	default:
		break;
	}

	return 0;
}

int CDuplicateDataManager::GetLatestNormalTollgateID()
{
	return m_iLatestNormalTollgateID;
}

int CDuplicateDataManager::GetLatestEliteTollgateID()
{
	return m_iLatestEliteTollgateID;
}

void CDuplicateDataManager::SetHeroExpIncreasement(int value)
{
	m_iCurHeroExpGain = value;
}

int CDuplicateDataManager::GetCurHeroExpIncreasement()
{
	return m_iCurHeroExpGain;
}
