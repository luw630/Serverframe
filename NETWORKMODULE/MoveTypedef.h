#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////
// ���ݶ���ԭ��
// ��������Ķ��ǻ�ȫ���õ������ݽṹ
///////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_TRACK_LENGTH        128

#define MY_WALK_FRAMESKIP       8
#define MY_RUN_FRAMESKIP        5
#define MY_JUMP_FRAMESKIP       3   // �൱�ڿͻ����ڿ��зɹ���3��
#define MY_FLY_FRAMESKIP        4

// comment by chenj �����Ծ���벻���ǹ̶�����
//const   int     MAX_JUMPOFFSETX = 10;       // ��Ծ������루�����Ļ��x
//const   int     MAX_JUMPOFFSETY = 13;       // ��Ծ������루�����Ļ��y

// ·���ڵ�
struct SWayTrack
{
    WORD    dir:3;  // �ýڵ㷽��
    WORD    len:13;  // �������ϵľ���
};

// �������紫�ݵ�·���ڵ㣨���յϿ������꣩
struct  SNWayNode 
{
    WORD    x;
    WORD    y;
};

// ͬ���ƶ�·����
struct SSynWay
{
    WORD wSegX, wSegY;                      // �ƶ���㣬�Կͻ��˵ĵ�ǰλ��Ϊ׼�������վ��״̬��Ϊ��ǰ�������꣬������ƶ�״̬��Ϊ���ǽ�Ҫ�ƶ�������һ����Ϊ����
    BYTE byAction;                          // ��Ϊ��ʽ��������ǰ���ƶ���ʽ�Լ���׼�ٶ�
    SWayTrack Track[MAX_TRACK_LENGTH];      // �ƶ�·�������ƶ����������ƶ�·���������˷���ͳ���
};

// �����3·����ṹ
struct SWayPoint
{
	float	m_X;							// ·����X����
	float	m_Y;							// ·����Y����
};

// �����3ͬ���ƶ�·���ṹ
// Ϊ���Ż������������������Ҫÿ�ζ����������ṹ������MAX_TRACK_LENGTH��·������Ϣ��
// ����Ӧ�÷������m_wayPoints��SWayPoint�ṹ�����ˡ��ڶ���ͬ���ƶ���Ϣ��ʱ��SPath
// �ṹ��Ա��������һ����Ա��
struct SPath
{
	WORD		m_wMapID;					// ������ͼ���
	BYTE		m_wayPoints;				// �ƶ���·�������
	BYTE		m_moveType;					// �ƶ��ķ�ʽ����/�ܣ��������ƶ����ٶ�
	SWayPoint	m_path[MAX_TRACK_LENGTH];	// �ƶ�·����������·���ϵ�ÿһ��·����
};											// ����һ����Ϊ��㣺�����վ��״̬��Ϊ��ǰ�������꣬������ƶ�״̬��Ϊ���ǽ�Ҫ�ƶ�������һ����Ϊ���꣩

///////////////////////////////////////////////////////////////////////////////////////////////
// ����·�����ɫ��Ϣ
// �·��б������£����£�������Ь�ӣ�
enum E_EQUIP_COL 
{
    EEC_FROCK = 0,
    EEC_SHIRT,
    EEC_GIRDLE,
    EEC_SHOES,
    //...
    EEC_MAX
};