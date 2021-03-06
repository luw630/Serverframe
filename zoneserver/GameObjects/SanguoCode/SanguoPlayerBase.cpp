#include "..\stdafx.h"
#include "SanguoPlayerBase.h"
#include"HeroMoudle\HeroDataManager.h"
#include "BaseDataManager.h"
#include "ExtendedDataManager.h"
#include "ExpeditionMoudle\ExpeditionDataManager.h"
#include "ExpeditionMoudle\ExpeditionManager.h"
#include "BlessMoudle\BlessDataMgr.h"
#include "BlessMoudle\BlessMoudle.h"
#include "ExpeditionMoudle\ExpeditionManager.h"
#include "Common\ConfigManager.h"
#include "StorageMoudle\StorageManager.h"
#include "HeroMoudle\HeroUpgradeMoudle.h"
#include "HeroMoudle\HeroExtendsGameplayModel.h"
#include "MailMoudle\MailMoudle.h"
#include "MissionMoudle\CMissionUpdate.h"
#include "MissionMoudle\MissionDataManager.h"
#include "MissionMoudle\MissionManager.h"
#include "MissionMoudle\MissionActiveness.h"
#include "MallMoudle\VarietyShopManager.h"
#include "MallMoudle\ArenaShopManager.h"
#include "MallMoudle\ExpeditionShopManager.h"
#include "MallMoudle\MiracleMerchantManager.h"
#include "MallMoudle\SoulExchangeManager.h"
#include "MallMoudle\GemMerchantManager.h"
#include "MallMoudle\LegionShopManager.h"
#include "MallMoudle\WarOfLeagueShopManager.h"
#include "MallMoudle\MallsMoudle.h"
#include "ExchangeMoudle\ExchangeGoldProcess.h"
#include "ExchangeMoudle\ExchangeEndurance.h"
#include "ExchangeMoudle\DiamondExchangeModule.h"
#include "AchievementMoudle\AchievementManager.h"
#include "AchievementMoudle\AchievementDataManager.h"
#include "AchievementMoudle\RandomAchieveManager.h"
#include "AchievementMoudle\RandomAchieveDataManager.h"
#include "AchievementMoudle\RandomAchieveUpdate.h"
#include "DuplicateMoudle\DuplicateDataManager.h"
#include "DuplicateMoudle\DuplicateBaseManager.h"
#include "DuplicateMoudle\EliteDuplicateManager.h"
#include "DuplicateMoudle\StoryDulicateManager.h"
#include "DuplicateMoudle\LeagueOfLegendDuplicateMananger.h"
#include "DuplicateMoudle\TreasureHuntingDuplicateManager.h"
#include "DessertMoudle\DessertSystem.h"
#include "ForgingMoudle\ForgingManager.h"
#include "ArenaMoudel\ArenaMoudel.h"
#include "ArenaMoudel\ArenaDataMgr.h"
#include "ScriptManager.h"
#include "SkillUpgradeMoudle\SkillUpgradeManager.h"
#include "SkillUpgradeMoudle\SkillUpgradeMoudle.h"
#include "LuckyTimeMoudle\ChaseThiefDataManager.h"
#include "LuckyTimeMoudle\ChaseThiefTollgateManager.h"
#include "LuckyTimeMoudle\ChaseThiefMoudle.h"
#include"Common\PubTool.h"
#include "../zoneserver/Recharge.h"
#include "../zoneserver/GameObjects/FactionManager.h"
extern SIZE_T GetMemoryInfo();

CSanguoPlayer::CSanguoPlayer()
	:m_pPlayerData(nullptr),
	cPlayer(nullptr),
	m_ClientIndex(0),
	m_GMFlag(false)
{
	m_dwExtendsDataIndex = 0;
	m_DataMgrList.clear();
	m_MallMgrList.clear();
	m_duplicateMgrs.clear();
}

CSanguoPlayer::~CSanguoPlayer()
{
	Release();
}

int CSanguoPlayer::OnCreate(_W64 long pParameter)
{
	rfalse(2, 1, "CSanguoPlayer::OnCreate");
	return 1;
}

void CSanguoPlayer::OnRecvSanguoPlayerRequest(SSGPlayerMsg* pMsg)
{
	switch (pMsg->_protocol)
	{
	case SSGPlayerMsg::EPRO_SANGUO_BLESS://祈福
		BlessMoudle::getSingleton()->DispatchBlessMsg((CBlessDataMgr*)(m_DataMgrList[Bless_Mgr].get()), (SBlessMsg*)pMsg);
		break;
	case SSGPlayerMsg::EPRO_SANGUO_HEROUPGRADE: //武将升级
		CHeroUpgradeMoudle::getSingleton()->DispatchHeroUpgradeMsg((SBaseHeroUpgrade*)pMsg, m_upBaseDataMgr.get());
		break;
	case SSGPlayerMsg::EPRO_EXPEDITION_MESSAGE: //远征消息
		CExpeditionManager::getSingleton()->DispatchExpeditionMsg(pMsg, m_ExpeditionDataMgr.get());
		break;
	case SSGPlayerMsg::EPRO_MISSION_MESSAGE:
		if (m_MissionMgr != nullptr)
			m_MissionMgr->RecvMsg(pMsg);
		break;
	case SSGPlayerMsg::EPRO_ACHIEVEMENT_MESSAGE:
		m_AchievementMgr->RecvMsg(pMsg);
		break;
	case  SSGPlayerMsg::EPRO_SANGUO_FORGING:
		CForgingManager::getSingleton()->OnRecvMsg(pMsg, m_upBaseDataMgr.get());
		break;
	case SSGPlayerMsg::EPRO_SANGUO_DUPLICATE:
		{
			const SDuplicateTypeMsg * ptrMsg = static_cast<const SDuplicateTypeMsg*>(pMsg);
			if (ptrMsg == nullptr)
				break;

			auto findResult = m_duplicateMgrs.find((InstanceType)ptrMsg->duplicateType);
			if (findResult != m_duplicateMgrs.end())
			{
				findResult->second->OnRecvMsg(ptrMsg);
			}
		}
		break;
	case SSGPlayerMsg::EPRO_SANGUO_DIAMONDEXCHANGE:
		if (m_DiamondExchangeModule != nullptr)
			m_DiamondExchangeModule->RecvMsg(pMsg);
		break;
	case SSGPlayerMsg::EPRO_SANGUO_MALL:
		{
			SMallMsgInfo* pMallMsg = static_cast<SMallMsgInfo*>(pMsg);
			MallType type = (MallType)pMallMsg->m_MallType;
			auto iter = m_MallMgrList.find(type);
			if (iter == m_MallMgrList.end())
				return;
			CMallsMoudle::getSingleton()->DispatchMallMsg(iter->second.get(), pMallMsg);
		}
		break;
	case  SSGPlayerMsg::EPRO_SANGUO_CHECKIN:
		{
			SQCHECKIN_Quest* pckMsg = static_cast<SQCHECKIN_Quest*>(pMsg);
			g_Script.SetCondition(0, cPlayer, 0);
			LuaFunctor(g_Script, "checkin_quest")[m_pPlayerData->m_dwStaticID][pckMsg->itype]();
			g_Script.CleanCondition();
		}
		break;
	case SSGPlayerMsg::EPRO_SANGUO_SKILLUPGRADE:
		CSkillUpgradeMoudle::getSingleton()->DispatchSkillUpgradeEvent((SSkillUpgrade*)pMsg, m_SkillUpgradeMgr.get());
		break;
	case SSGPlayerMsg::EPRO_SANGUO_RANDOMAVHIEVEMNT_MESSAGE:
		if (m_RandomAchievementMgr != nullptr)
			m_RandomAchievementMgr->RecvMsg(pMsg);
		break;
	case SSGPlayerMsg::EPRO_SANGUO_DESSERT_MESSAGE:
		if (m_DessertSystem != nullptr)
			m_DessertSystem->RecvMsg(pMsg);
		break;
	case SSGPlayerMsg::EPRO_SANGUO_CHASETHIEF:
	{
		SChaseThiefMsg* pctMsg = static_cast<SChaseThiefMsg*>(pMsg);
		if (nullptr == pctMsg)
			return;
		CChaseThiefMoudle::getSingleton()->OnRecvive(m_ChaseThiefDataMgr.get(), m_ChaseThiefTollgateMgr.get(), pctMsg);
	}
		break;
	case SSGPlayerMsg::EPRO_SANGUO_CHANGENAME:
	{
		CSanguoPlayer::ChangeGameName((SQChangePlayerName*)pMsg);
	}
		break;
	case SSGPlayerMsg::EPRO_SANGUO_ACTIVITY: //活动相关
	{
		SQGetActReward* pGetRwardMsg = static_cast<SQGetActReward*>(pMsg);
		g_Script.SetCondition(0, cPlayer, 0);
		LuaFunctor(g_Script, "act_get_reward")[m_pPlayerData->m_dwStaticID][pGetRwardMsg->actType][pGetRwardMsg->reawrdIndex]();
		g_Script.CleanCondition();
	}
		break;
	case SSGPlayerMsg::EPRO_SANGUO_NEWGUIDE:
	{
		m_upBaseDataMgr->OnRecvGuideOperation(pMsg);
	}
		break;
	case SSGPlayerMsg::EPRO_SANGUO_RECHARGE: //充值
	{
		CRecharge::getSingleton()->RecvRechargeMsg(cPlayer, pMsg);
	}
		break;
	case SSGPlayerMsg::EPRO_SANGUO_HEROEXTENDSGAMEPLAY:
		if (m_HeroExtendsGameplayMgr != nullptr)
			m_HeroExtendsGameplayMgr->OnRecvMsg(pMsg);
		break;
	case SSGPlayerMsg::EPRO_SANGUO_fIRSTRECHARGE:
	{
		g_Script.SetCondition(0, cPlayer, 0);
		LuaFunctor(g_Script, "f_recharge_award")[m_pPlayerData->m_dwStaticID]();
		g_Script.CleanCondition();
	}
	break;
	case SSGPlayerMsg::EPRO_SANGUO_WeekReMoney:
	{
		SQWeekRemoney *msg = static_cast<SQWeekRemoney*>(pMsg);
		if (msg->day == 0){
			g_Script.SetCondition(0, cPlayer, 0);
			LuaFunctor(g_Script, "BuyReMoneyAct")[m_pPlayerData->m_dwStaticID]();
			g_Script.CleanCondition();
		}
		else
		{	
			g_Script.SetCondition(0, cPlayer, 0);
			LuaFunctor(g_Script, "OnTriggerWeekTreasure")[m_pPlayerData->m_dwStaticID][msg->day]();
			g_Script.CleanCondition();
		}

	}
	break;
	case SSGPlayerMsg::EPRO_SANGUO_War://国战
	{
		SWarMsg *msg = static_cast<SWarMsg*>(pMsg);
		switch (msg->_protocol)
		{
		case SWarMsg::war_join:
			{
				SQWarJoin *warMsg = static_cast<SQWarJoin*>(msg);
				g_Script.SetCondition(0, cPlayer, 0);
				LuaFunctor(g_Script, "war_join")[m_pPlayerData->m_dwStaticID][warMsg->dwHeroId[0]][warMsg->dwHeroId[1]][warMsg->dwHeroId[2]][warMsg->dwHeroId[3]][warMsg->dwHeroId[4]]();
				g_Script.CleanCondition();
			}
			break;
		case SWarMsg::war_teamList:
			{
				SQwar_teamList *warMsg = static_cast<SQwar_teamList*>(msg);
				g_Script.SetCondition(0, cPlayer, 0);
				LuaFunctor(g_Script, "war_get_teamList")[warMsg->teamNum][warMsg->beginNum]();
				g_Script.CleanCondition();
			}
			break;
		case SWarMsg::war_fight_res:
			{
				SQwar_fight_res *warMsg = static_cast<SQwar_fight_res*>(msg);
				g_Script.SetCondition(0, cPlayer, 0);
				//LuaFunctor(g_Script, "war_fight_res")[m_pPlayerData->m_dwStaticID][warMsg->res]();
				if (g_Script.PrepareFunction("war_fight_res"))
				{
					g_Script.PushParameter(m_pPlayerData->m_dwStaticID);
					g_Script.PushParameter(warMsg->res);
					std::list<DWORD> tempTable;
					for (int i = 0; i < 3 * MAX_War_HERONUM;++i)
					{
						tempTable.push_back(warMsg->heroInfo[i]);
					}
					g_Script.PushDWORDArray(tempTable);
					g_Script.Execute();
				}
				g_Script.CleanCondition();
			}
			break;
		case SWarMsg::war_exit:
			{
				SQwar_exit *warMsg = static_cast<SQwar_exit*>(msg);
				g_Script.SetCondition(0, cPlayer, 0);
				LuaFunctor(g_Script, "war_exit")[m_pPlayerData->m_dwStaticID]();
				g_Script.CleanCondition();
			}
			break;
		case SWarMsg::war_isjoin:
		{
			
			g_Script.SetCondition(0, cPlayer, 0);
			LuaFunctor(g_Script, "war_isJoin")[m_pPlayerData->m_dwStaticID][1]();
			g_Script.CleanCondition();
		}
		break;
		case SWarMsg::war_rank:
		{
			g_Script.SetCondition(0, cPlayer, 0);
			LuaFunctor(g_Script, "war_getrank")[m_pPlayerData->m_dwStaticID]();
			g_Script.CleanCondition();
		}
		break;
		default:
			break;
		}

	}
	break;
	case SSGPlayerMsg::EPRO_SANGUO_Rank:
	{
		SRankMsg *msg = static_cast<SRankMsg*>(pMsg);
		if (msg->_protocol == SRankMsg::rank_get){
			SQRank *rankMsg = static_cast<SQRank*>(msg);
			g_Script.SetCondition(0, cPlayer, 0);
			LuaFunctor(g_Script, "rank_getList")[rankMsg->itype]();
			g_Script.CleanCondition();
		}
	}
	break;
	case SSGPlayerMsg::EPRO_SANGUO_Excheange://兑换活动
	{
		SExcheangeMsg *msg = static_cast<SExcheangeMsg*>(pMsg);
		if (msg->_protocol == SExcheangeMsg::getExchange){
			SQGetExcheange *ExcheangeMsg = static_cast<SQGetExcheange*>(msg);
			g_Script.SetCondition(0, cPlayer, 0);
			LuaFunctor(g_Script, "exchangeACT_getAwards")[m_pPlayerData->m_dwStaticID][ExcheangeMsg->itype][ExcheangeMsg->index][ExcheangeMsg->num]();
			g_Script.CleanCondition();
		}
	}
	break;
	default:
		break;
	}
}


void CSanguoPlayer::ChangeGameName(SQChangePlayerName* pMsg)
{
	CBaseDataManager* baseDataMgr = CSanguoPlayer::GetBaseDataMgr();

	int decmoney = CConfigManager::getSingleton()->globalConfig.ChangeNameConsumption;

	SAChangeName msg;

	lite::Variant ret;//从lua获取更名次数
	LuaFunctor(g_Script, "GetChangeNameCount")[baseDataMgr->GetSID()](&ret);
	int count = (int)ret;
	if (count > 99999 || count < 0) //如果改名次数过大或小于0，则代表lua获取改名次数失效，立即返回
		return;
	//获取免费修改的次数
	int num = CConfigManager::getSingleton()->globalConfig.ChangeNameConsumptionFreeTime;

	//这是付费修改 

	if (count > num)
	{
		if (baseDataMgr->GetDiamond() < decmoney)
		{
			//金币不足 不做处理  或返回服务端错误消息
			return;
		}
	}

	dwt::stringkey<char[32]> str = pMsg->name;
	char name[32];

	memcpy(name, pMsg->name, sizeof(pMsg->name));
	//判断名称是否可用
	if (baseDataMgr->CheckGameName(name, baseDataMgr->GetSID()) == 0)
	{
		//缓存中不可用回发
		msg.ChangeNameCount = count;
		msg.falg = false;
		g_StoreMessage(baseDataMgr->GetDNID(), &msg, sizeof(msg));
	}
	
}

void CSanguoPlayer::Db_Back_ChangeGameName(char* name, DWORD SID, CBaseDataManager* baseDataMgr)
{
	
	SAChangeName msg;

	lite::Variant ret;//从lua获取更名次数
	LuaFunctor(g_Script, "GetChangeNameCount")[SID](&ret);
	int count = (int)ret;
	if (count > 99999 || count < 0) //如果改名次数过大或小于0，则代表lua获取改名次数失效，立即返回
		return;

	//获取更名所需费用
	int decmoney = CConfigManager::getSingleton()->globalConfig.ChangeNameConsumption;

	//宝石不足
	if (decmoney < 0)
		return;

	//获取免费修改的次数
	int num = CConfigManager::getSingleton()->globalConfig.ChangeNameConsumptionFreeTime;

	//这是付费修改 
	if (count > num)
	{
		if (baseDataMgr->GetDiamond() < decmoney)
		{
			//金币不足 不做处理  或返回服务端错误消息

			return;
		}

		else
		{
			//为玩家设置新的名称
			std:: string outname = baseDataMgr->GetName();//.c_str();

			baseDataMgr->SetGameName(name);
			CArenaMoudel::getSingleton()->ChangeNametoArena(baseDataMgr->GetSID(), name);

			//变更成员帮派信息
			//CFactionManager::getSingleton()->UpdateMemberMsg(cPlayer, outname.data());
			
			//扣除费用 
			baseDataMgr->DecGoods_SG(GoodsType::diamond, 0, decmoney,GoodsWay::changename);

		}
	}
	else
	{
		//这是免费修改
		std::string outname = baseDataMgr->GetName();//.c_str();
		//为玩家设置新的名称
		baseDataMgr->SetGameName(name);
		//变更成员帮派信息
		//CFactionManager::getSingleton()->UpdateMemberMsg(cPlayer, outname.data());
	}


	//变更更名次数
	lite::Variant ret1;//从lua变更改名次数
	LuaFunctor(g_Script, "ChangeNameCountAdd")[SID](&ret1);
	int result = (int)ret1;
	if (result != 0)
	{
		msg.falg = true;
	}
	else
	{
		msg.falg = false;
	}
	msg.ChangeNameCount = result;

	int a= g_StoreMessage(baseDataMgr->GetDNID(), &msg, sizeof(msg));


}

void CSanguoPlayer::SetCplayerPtr(CPlayer*  _cPlayer)
{
	cPlayer = _cPlayer;
}
void CSanguoPlayer::SetSanguoPlayerData(SFixData *fixData)
{
	///初始化成就的逻辑管理
	m_AchievementMgr.reset(new CAchievementManager(*((CExtendedDataManager*)m_AchievementDataMgr.get())));
	m_RandomAchievementDataMgr.reset(new CRandomAchieveDataManager(*m_upBaseDataMgr));
	m_RandomAchievementDataMgr->InitDataMgr(m_pPlayerData);
	m_RandomAchievementMgr.reset(new CRandomAchieveManager( *((CExtendedDataManager*)m_RandomAchievementDataMgr.get())) );
	///处理基本数据的成就相关的逻辑
	///暂时这样丑陋的让数据管理类来访问逻辑层的逻辑，TODO：重构@Jonson
	//////////////////////////////////////////////////////////////////////////
	m_upBaseDataMgr->SetAchievementUpdator(m_AchievementMgr.get());
	m_upBaseDataMgr->SetRandomAchievementUpdator(m_RandomAchievementMgr.get());
	m_upBaseDataMgr->SetMissionUpdator(m_MissionMgr.get());
	//////////////////////////////////////////////////////////////////////////
	///初始化秘境寻宝、将神传说、普通副本、精英副本的数据管理
	m_DuplicateDataMgr.reset(new CDuplicateDataManager(*m_upBaseDataMgr));
	m_DuplicateDataMgr->InitDataMgr(m_pPlayerData);
	shared_ptr<CStoryDulicateManager> m_storyDuplicateMgr = make_shared<CStoryDulicateManager>(*((CExtendedDataManager*)m_DuplicateDataMgr.get()));
	m_storyDuplicateMgr->InitMgr();
	m_storyDuplicateMgr->ActiveTo(m_DuplicateDataMgr->GetLatestNormalTollgateID());
	m_duplicateMgrs.insert(make_pair(InstanceType::Story, m_storyDuplicateMgr));
	shared_ptr<CEliteDuplicateManager> m_eliteDuplicateMgr = make_shared<CEliteDuplicateManager>(*((CExtendedDataManager*)m_DuplicateDataMgr.get()));
	m_eliteDuplicateMgr->InitMgr();
	m_eliteDuplicateMgr->ActiveTo(m_DuplicateDataMgr->GetLatestEliteTollgateID());
	m_duplicateMgrs.insert(make_pair(InstanceType::StoryElite, m_eliteDuplicateMgr));
	shared_ptr<CDuplicateBaseManager> m_LOLDuplicateMgr = make_shared<CLeagueOfLegendDuplicateMananger>(*((CExtendedDataManager*)m_DuplicateDataMgr.get()));
	m_LOLDuplicateMgr->InitMgr();
	m_duplicateMgrs.insert(make_pair(InstanceType::LeagueOfLegends, m_LOLDuplicateMgr));
	shared_ptr<CDuplicateBaseManager> m_TreasureHuntingDuplicateMgr = make_shared<CTreasureHuntingDuplicateManager>(*((CExtendedDataManager*)m_DuplicateDataMgr.get()));
	m_TreasureHuntingDuplicateMgr->InitMgr();
	m_duplicateMgrs.insert(make_pair(InstanceType::TreasureHunting, m_TreasureHuntingDuplicateMgr));
	///初始化宝石兑换其他物件的模块
	m_DiamondExchangeModule.reset(new CDiamondExchangeModule());

	/// 绑定激活追击盗贼功能至普通关卡结算事件
	m_storyDuplicateMgr->BindClearingEvent(m_ChaseThiefDataMgr->GetActivateFunc());
	/// 绑定开启珍宝商人功能至普通关卡结算事件
	m_storyDuplicateMgr->BindClearingEvent(m_MallMgrList[MallType::GemMerchant]->GetOpenFunc());
	/// 绑定激活追击盗贼功能至精英关卡结算事件
	m_eliteDuplicateMgr->BindClearingEvent(m_ChaseThiefDataMgr->GetActivateFunc());
	/// 绑定开启奇缘商人功能至精英关卡结算事件
	m_eliteDuplicateMgr->BindClearingEvent(m_MallMgrList[MallType::MiracleMerchant]->GetOpenFunc());

	//追击盗贼关卡初始化
	m_ChaseThiefTollgateMgr.reset(new CChaseThiefTollgateManager());
	m_ChaseThiefTollgateMgr->Init();


	if (true == m_GMFlag && true == m_pPlayerData->m_bNewPlayer)
		InitGMData(); //初始化GM数据
	
	//发送竞技场上次挑战时间
	CArenaMoudel::getSingleton()->SendLastChallengTime(m_upBaseDataMgr->GetDNID(), m_upBaseDataMgr->GetSID());


}

void CSanguoPlayer::FristLogin()
{
	//需要根据登录时间 对数据进行刷新操作的逻辑
	// 5点刷新数据
	INT64 now;
	_time64(&now);
	tm targetTime;
	_localtime64_s(&targetTime, &now);
	targetTime.tm_hour = 5;
	targetTime.tm_min = 0;
	targetTime.tm_sec = 0;
	if (RefreshJudgement::JudgeCrossed(m_upBaseDataMgr->GetLogoutTime(), targetTime))
	{
		//调用脚本刷新回调
		if (g_Script.PrepareFunction("OnLoginRefresh"))
		{
			g_Script.PushParameter(m_upBaseDataMgr->GetSID());
			g_Script.PushParameter(5);
			g_Script.Execute();
		}
	}
}

bool CSanguoPlayer::Release()
{
	m_pPlayerData = nullptr;
	cPlayer = nullptr;
	return true;
}

void CSanguoPlayer::InitGMData()
{
	rfalse("创建gm账号");

	std::map<int32_t, HeroConfig>  heorConfigMap = CConfigManager::getSingleton()->GetHeroConfigMap();
	for (auto heroConfig : heorConfigMap)
		m_upBaseDataMgr->AddHero(heroConfig.first);
	m_upBaseDataMgr->PlusDiamond(10000000);
	m_upBaseDataMgr->PlusMoney(10000000);
	m_upBaseDataMgr->PlusHonor(10000000);
	m_upBaseDataMgr->PlusExploit(10000000);
	m_upBaseDataMgr->PlusPrestige(10000000);
	
	vector<int32_t> equipIDList;
	CConfigManager::getSingleton()->GetEquipIDList(equipIDList);
	CStorageManager& storageMgr = m_upBaseDataMgr->GetStorageManager();
	for each (int32_t var in equipIDList)
		storageMgr.AddItem(var, 99);

	//适应性代码, GM方便测功能
	if (!storageMgr.ItemExistOrEnough(20002))///灵芝
		storageMgr.AddItem(20002, 1000);
	if (!storageMgr.ItemExistOrEnough(20010))///扫荡卷
		storageMgr.AddItem(20010, 1000);
	if (!storageMgr.ItemExistOrEnough(20011))///体力药剂
		storageMgr.AddItem(20011, 1000);
	if (!storageMgr.ItemExistOrEnough(20012))///神秘宝箱
		storageMgr.AddItem(20012, 1000);
	if (!storageMgr.ItemExistOrEnough(20013))///绿装宝箱
		storageMgr.AddItem(20013, 1000);
	if (!storageMgr.ItemExistOrEnough(20014))///蓝装宝箱
		storageMgr.AddItem(20014, 1000);
	if (!storageMgr.ItemExistOrEnough(20015))///紫装宝箱
		storageMgr.AddItem(20015, 1000);
	if (!storageMgr.ItemExistOrEnough(20016))///桃园结义宝箱
		storageMgr.AddItem(20016, 1000);


	//GM直接解锁竞技场
	CArenaMoudel::getSingleton()->DebutArena(m_upBaseDataMgr->GetDNID(), m_upBaseDataMgr.get());
	

}

void CSanguoPlayer::OnRecvStorageOperation(SSGPlayerMsg* pMsg)
{
	m_upBaseDataMgr->OnRecvStorageOperation(pMsg);
}
CBaseDataManager* CSanguoPlayer::GetSanguoBaseData()
{
	return m_upBaseDataMgr.get();
}

void CSanguoPlayer::OnRun()
{

}

CBaseDataManager* CSanguoPlayer::GetBaseDataMgr()
{
	return m_upBaseDataMgr.get();
}

CExtendedDataManager* CSanguoPlayer::GetDataMgr(DataMgrType type)
{
	return m_DataMgrList[type].get();
}

bool CSanguoPlayer::SanguoLogout()
{
	CArenaMoudel::getSingleton()->SetPlayerManagedArenaData(m_upBaseDataMgr.get());//玩家登出 更新竞技场托管数据
	if (m_RandomAchievementDataMgr != nullptr)
		m_RandomAchievementDataMgr->LogoutProcess();
	m_bValid = false;
	return true;
}

void CSanguoPlayer::FiveOclockRefreshEvent()
{
	///都刷新后往客户端同步刷新消息
	if (m_ExpeditionDataMgr != nullptr)
		m_ExpeditionDataMgr->RefreshTicket(1);

	if (m_MissionDataMgr != nullptr)
	{
		m_MissionDataMgr->ResetMissionData(1);
		///开启午餐体力礼包任务的显示
		m_MissionDataMgr->LunchMissionVisible();
	}

	if (m_MissionActivenessMgr != nullptr)
	{
		m_MissionActivenessMgr->ResetMissionActivenessData(1);
	}

	if (m_RandomAchievementDataMgr != nullptr)
	{
		m_RandomAchievementDataMgr->RefreshData(1);
	}

	if (m_DuplicateDataMgr != nullptr)
		m_DuplicateDataMgr->RefreshData(1);

	if (m_DiamondExchangeModule != nullptr)
		m_DiamondExchangeModule->RefreshData();

	//if (m_upBaseDataMgr != nullptr)
	//	m_upBaseDataMgr->ResetDataForNewDay();

	if (m_DessertSystem != nullptr)
		m_DessertSystem->ResetData();

	///下面开始激活将神传说副本的逻辑
	//////////////////////////////////////////////////////////////////////////
	///GM号默认副本全开放
	if (m_GMFlag == false)
	{
		auto findResut = m_duplicateMgrs.find(InstanceType::LeagueOfLegends);
		if (findResut == m_duplicateMgrs.end())
			return;

		shared_ptr<CLeagueOfLegendDuplicateMananger> m_LOLDuplicateMgr = dynamic_pointer_cast<CLeagueOfLegendDuplicateMananger>(findResut->second);
		if (m_LOLDuplicateMgr == nullptr)
			return;

		m_LOLDuplicateMgr->JudgeDuplicateAcitiveState();
	}
	//////////////////////////////////////////////////////////////////////////
	/// 重置商店刷新次数
	for (auto &iter : m_MallMgrList)
	{
		iter.second->ResetRefreshedCount();
	}
}

void CSanguoPlayer::TwelveOclockRefreshEvent()
{
	///都刷新后往客户端同步刷新消息
	if (m_MissionDataMgr != nullptr)
	{
		m_MissionDataMgr->LunchMissionAccessible();
	}
}

void CSanguoPlayer::FourteenOclockRefreshEvent()
{
	///都刷新后往客户端同步刷新消息
	if (m_MissionDataMgr != nullptr)
	{
		m_MissionDataMgr->DinnerMissionVisible();
	}
}

void CSanguoPlayer::EighteenOclockRefreshEvent()
{
	///都刷新后往客户端同步刷新消息
	if (m_MissionDataMgr != nullptr)
	{
		m_MissionDataMgr->DinnerMissionAccessible();
	}
}

void CSanguoPlayer::TwentyOclockRefreshEvent()
{
	///都刷新后往客户端同步刷新消息
	if (m_MissionDataMgr != nullptr)
	{
		m_MissionDataMgr->SupperMissionVisible();
	}
}

void CSanguoPlayer::TwentyOneOclockRefreshEvent()
{
	///都刷新后往客户端同步刷新消息
	if (m_MissionDataMgr != nullptr)
	{
		m_MissionDataMgr->SupperMissionAccessible();
	}
}

void CSanguoPlayer::TwentyFourOclockRefreshEvent()
{
	///都刷新后往客户端同步刷新消息
	if (m_MissionDataMgr != nullptr)
	{
		m_MissionDataMgr->SupperMissionDisable();
	}
	if (m_upBaseDataMgr != nullptr) //免费金钱祈福数据重置
	{
		((CBlessDataMgr*)m_DataMgrList[Bless_Mgr].get())->SetMoneyFreeBlessCount(5);
	}

}

void CSanguoPlayer::VarietyShopRefreshEvent()
{
	auto iter = m_MallMgrList.find(MallType::VarietyShop);
	if (iter == m_MallMgrList.end())
		return;

	CMallsMoudle::getSingleton()->SendAutoRefreshMsgToClient(iter->second.get());
}

void CSanguoPlayer::ArenaShopRefreshEvent()
{
	auto iter = m_MallMgrList.find(MallType::ArenaShop);
	if (iter == m_MallMgrList.end())
		return;

	CMallsMoudle::getSingleton()->SendAutoRefreshMsgToClient(iter->second.get());
}

void CSanguoPlayer::ExpeditionShopRefreshEvent()
{
	auto iter = m_MallMgrList.find(MallType::ExpeditionShop);
	if (iter == m_MallMgrList.end())
		return;

	CMallsMoudle::getSingleton()->SendAutoRefreshMsgToClient(iter->second.get());
}

void CSanguoPlayer::LegionShopRefreshEvent()
{
	auto iter = m_MallMgrList.find(MallType::LegionShop);
	if (iter == m_MallMgrList.end())
		return;

	CMallsMoudle::getSingleton()->SendAutoRefreshMsgToClient(iter->second.get());
}

void CSanguoPlayer::MiracleMerchantRefreshEvent()
{
	auto iter = m_MallMgrList.find(MallType::MiracleMerchant);
	if (iter == m_MallMgrList.end())
		return;

	CMallsMoudle::getSingleton()->SendAutoRefreshMsgToClient(iter->second.get());
}

void CSanguoPlayer::GemMerchantRefreshEvent()
{
	auto iter = m_MallMgrList.find(MallType::GemMerchant);
	if (iter == m_MallMgrList.end())
		return;

	CMallsMoudle::getSingleton()->SendAutoRefreshMsgToClient(iter->second.get());
}

void CSanguoPlayer::SoulPointExchangeShopRefreshEvent()
{
	auto iter = m_MallMgrList.find(MallType::SoulExchange);
	if (iter == m_MallMgrList.end())
		return;

	CMallsMoudle::getSingleton()->SendAutoRefreshMsgToClient(iter->second.get());
}

void CSanguoPlayer::WarOfLeagueShopRefreshEvent()
{
	auto iter = m_MallMgrList.find(MallType::WarOfLeagueShop);
	if (iter == m_MallMgrList.end())
		return;

	CMallsMoudle::getSingleton()->SendAutoRefreshMsgToClient(iter->second.get());
}

void CSanguoPlayer::UpdateDataForVIP(int vipLevel)
{
	m_SkillUpgradeMgr->SetSkillPointForVIP(vipLevel);
	auto iter = m_MallMgrList.find(MallType::GemMerchant);
	if (iter != m_MallMgrList.end())
	{
		iter->second->VipLevelChange(vipLevel);
	}

	iter = m_MallMgrList.find(MallType::MiracleMerchant);
	if (iter != m_MallMgrList.end())
	{
		iter->second->VipLevelChange(vipLevel);
	}

	///更新VIP等级相关的任务
	if (m_upBaseDataMgr != nullptr)
	{
		//CMissionUpdate * missionUpdator = m_upBaseDataMgr->GetMissionUpdator();
		//if (missionUpdator != nullptr)
		//	missionUpdator->UpdateVipMission(vipLevel);
		m_upBaseDataMgr->UpdateDateForVIP(vipLevel);
	}
}

void CSanguoPlayer::InitDataOfLogin()
{
	if (nullptr != m_SkillUpgradeMgr)
		m_SkillUpgradeMgr->InitSkillPointData();

	///初始化小额奖励的相关lua端的数据
	if (m_DessertSystem != nullptr)
		m_DessertSystem->InitLuaData();
	///初始化任务的一些数据
	if (m_MissionDataMgr != nullptr)
		m_MissionDataMgr->OnLuaInitialed();
	if (m_MissionActivenessMgr != nullptr)
		m_MissionActivenessMgr->OnLuaInitialed();

	/// 刷新距离下次刷新时间
	for (auto& iter : m_MallMgrList)
	{
		iter.second->SetRefreshInfo(false);
	}

	auto iter = m_MallMgrList.find(MallType::GemMerchant);
	if (iter != m_MallMgrList.end())
	{
		iter->second->LoginProcess();
		iter->second->IsOpened();
	}

	iter = m_MallMgrList.find(MallType::MiracleMerchant);
	if (iter != m_MallMgrList.end())
	{
		iter->second->LoginProcess();
		iter->second->IsOpened();
	}

	if (nullptr != m_upBaseDataMgr)
		m_upBaseDataMgr->InitDataOfLogin();
}

const CDuplicateBaseManager* CSanguoPlayer::GetDuplicatemanager(InstanceType type)
{
	auto findResult = m_duplicateMgrs.find(type);
	if (findResult == m_duplicateMgrs.end())
		return nullptr;

	return findResult->second.get();
}

CDessertSystem* CSanguoPlayer::GetDessertSystem()
{
	return m_DessertSystem.get();
}

void CSanguoPlayer::AddSkillPoint(int num)
{
	if (m_SkillUpgradeMgr != nullptr)
		m_SkillUpgradeMgr->AddSkillPoint(num);
}

