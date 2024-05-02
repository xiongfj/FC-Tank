#pragma once
#include "PlayerBase.h"
#include "EnemyBase.h"
#include "TimeClock.h"

struct Map
{
	char buf[26][27];
};

/* �๦��:
* �� main ���������ڻ�ͼ�ӿ�
* ����һ�� IMAGE ����, ���Ƶ�ͼ/�л��б���Ϣ/����/�ؿ�
* �洢���ӱ��
* �洢�������
* ������Ϸѭ��

** ��Ա����:
- EnemyList: �洢������� and �����ƶ� and �ȴ����ֵĵл�
*/

enum GameResult {Victory, Fail};		// ÿһ��ʤ��\ʧ��

class GameControl
{
public:
	GameControl( HDC des_hdc, HDC image_hdc/*, BoxMarkStruct* */);
	~GameControl();
	void Init();
	void AddPlayer(int player_num);					// ������ҽ��������
	void LoadMap();									// ��ȡ .map ��ͼ�ļ�|�������Ƶ�ͼ
	bool CreateMap(bool* out);				// ����Զ����ͼ
	void GameLoop();
	GameResult StartGame();								// ���»�����Ϸ���ֶ���, ���� false ������Ϸ

private:
	void CutStage();		// ����
	void ShowStage();				// ��ʼÿһ��֮ǰ��ʾ STAGE 2 ֮�������
	void ClearSignBox();
	void InitSignBox();				// ��ʼ�����ӱ��
	void AddEnemy();						// ��Ϸ��ʼǰ ���20 �ܵл�
	void SignBox_4(int i, int j, int sign_val);	// ��Ǹ���, LoadMap() �ڵ���
	bool RefreshData();
	void RefreshRightPanel();						// ˢ���ұ���Ϣ�������, �������ݱ仯�ŵ��ø���
	void RefreshCenterPanel();						// ˢ���м���Ϸ���� 208 * 208
	void CheckKillEnemy(PlayerBase*);		// �������Ƿ���ел�, ������ɾ���õл�
	void IsGameOver();				// ѭ������Ƿ�flag,  gameover
	void IsWinOver();				// �������ел�ʤ������

public:
	static int mCurrentStage;						// [1-35] ��ǰ�ؿ�, SelectPanel ��ʹ��, ������ʹ��

private:
	// ���ڻ�ͼ�ӿ�
	IMAGE mCenterImage;				// ���ڱ��� mCenter_hdc ������Ч��.
	HDC mDes_hdc,					// ������hdc, main ����
		mImage_hdc,					// 256*224 �� hdc, main ����
		mCenter_hdc;				// 208*208 �� hdc, �������, ����Ϸ����,�ֿ�,��������,����Ҳ�����趨

	BoxMarkStruct* mBoxMarkStruct;			// �洢���ӱ��
	list<PlayerBase*> PlayerList;
	list<EnemyBase*> EnemyList;				// �л��б�

	IMAGE mGrayBackgroundImage;		// ��Ϸ��ɫ����ͼ
	IMAGE mBlackBackgroundImage;	// ��ɫ����ͼ

	Map mMap;						// �洢��ͼ���ݽṹ
	IMAGE mStoneImage;				// ʯͷ
	IMAGE mForestImage;				// ����
	IMAGE mIceImage;				// ����
	IMAGE mRiverImage[2];			// ����
	IMAGE mWallImage;				// ��ǽ
	IMAGE mCamp[2];					// ��Ӫ

	IMAGE mEnemyTankIcoImage;		// �л�̹��ͼ��
	IMAGE mFlagImage;				// ����

	int mCutStageCounter;
	IMAGE mCurrentStageImage;		// STAGE ����
	IMAGE mBlackNumberImage;		// 0123456789 ��ǰ�ؿ���
	IMAGE mGameOverImage;			//

	int mRemainEnemyTankNumber;		// ����δ��ʾ�ĵл�����, ��ʼֵ=20, ����һ�ܵл���ֵ��һ
	int mCurMovingTankNumber;		// ��ǰ���ֵĵл�����
	int mKillEnemyNum;				// �Ѿ�����ĵл���

	BlastStruct mBlast;				// ��Ӫ��ը
	bool mCampDie;					// ��Ӫ������, ���� Camp ��ͼ 

	bool mEnemyPause;			// �Ƿ�����ƶ��л�
	//int mEnemyPauseCounter;		// �л���ͣ����
	TimeClock mEnemyPauseTimer;		// �л���ͣ���

	TimeClock mMainTimer;			// ���ƻ�ͼƵ��
	TimeClock mCampTimer;		// Camp ��ը�ٶ�

	int mGameOverCounter;
	int mGameOverX, mGameOverY;	// 
	bool mGameOverFlag;			// ��Ϸ�������, �����������, �񳲱�����
	TimeClock mGameOverTimer;	// ��������

	IMAGE mCreateMapTankImage;		// �Զ����ͼ�е� ̹���α�ͼƬ
	int mCMTImageX, mCMTImageY;		// �α����ĵ�����

	bool mHasCustomMap;			// ��ǰ�Ƿ���������ͼ, �����˳������ٴν�������, ����֮ǰ����������
	bool mShowScorePanel;		// �Ƿ���ʾ�������

	bool mWin;			// ������л�,ʤ��
	int mWinCounter;	// ���������������ת

	bool mShowGameOverAfterScorePanel;
	IMAGE msgoas_image;
	int msgoas_y;		// ��������
	int msgoas_counter;
};