#include "stdafx.h"
#include "PlayerBase.h"
#include "MciSound.h"

//----------------- PlayerBase �ྲ̬����

int PlayerBase::mDevXY[4][2] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1} };	// ������������
PropClass PlayerBase::mProp;
bool PlayerBase::mTimeProp = false;
bool PlayerBase::mShovelProp = false;
int PlayerBase::mShovelProp_counter = 0;
list<PlayerBase*>* PlayerBase::mPList = NULL;
BoxMarkStruct* PlayerBase::bms = NULL;
//IMAGE PlayerBase::mGameOverImage;
int PlayerBase::mMoveSpeedDev[4] = {21, 19, 17, 15};
int PlayerBase::mBulletSpeedDev[4] = {17, 16, 15, 14};

PlayerBase::PlayerBase(byte player, BoxMarkStruct* b/*, PropClass* pc*/)
{
	int i = 0;
	player_id = player;
	mPlayerTank = new PlayerTank(player_id);
	bms = b;
	//mProp.Init(b);

	mPlayerLife = 2;		// ��� HP
	mPlayerTankLevel = 0;

	//// ��ͬ����̹���ƶ��ٶ�ϵ��
	//int temp[4] = { 1, 1, 1, 1 };
	//for (i = 0; i < 4; i++)
	//	mSpeed[i] = temp[i];

	Init();

	// ��ͬ������ݲ�ͬ
	if (player_id == 0)
		loadimage(&m12PImage, _T("./res/big/1P.gif"));		// 1P\2Pͼ��
	else
		loadimage(&m12PImage, _T("./res/big/2P.gif"));

	// ��ͬ������
	loadimage(&mPlayerTankIcoImage, _T("./res/big/playertank-ico.gif"	));	// ���̹��ͼ��
	loadimage(&mBlackNumberImage,	_T("./res/big/black-number.gif"		));	// ��ɫ����
		
	/*********************************
	* BulletStruct ���ݳ�ʼ��
	*********************************/
	// �����ӵ�ͼƬ
	TCHAR buf[100];
	for (i = 0; i < 4; i++)
	{
		_stprintf_s(buf, _T("./res/big/bullet-%d.gif"), i);
		loadimage(&BulletStruct::mBulletImage[i], buf);
	}

	// ��ըͼƬ
	for (i = 0; i < 3; i++)
	{
		_stprintf_s(buf, _T("./res/big/bumb%d.gif"), i);
		loadimage(&BombStruct::mBombImage[i], buf);
	}

	mBombTimer.SetDrtTime(20);	// ��������̫С..
	//mBlastTimer.SetDrtTime(36);

	// ��ʾ�������
	mScorePanel = new ScorePanel(player_id);

	//mGameOverTimer.SetDrtTime(20);
	//loadimage(&mGameOverImage, _T("./res/big/gameover.gif") );
}

PlayerBase::~PlayerBase()
{
	delete mPlayerTank;
	delete mScorePanel;

	// ��̬���ݷ��ö�� delete
	/*���������ͷ�, �� delete ���������ʱ��, �������󻹻��õ������̬����!!!
	if (mProp != NULL)
	{
		delete mProp;
		mProp = NULL;
	}*/

	printf("PLayerBase:: ~PlayerBase()\n");
}

void PlayerBase::Init()
{
	//printf("adasd\n");
	mProp.Init(bms);
	mStar.Init();
	mRing.Init();
	mBlast.Init();
	mPlayerGameover.Init(player_id);

	mTimeProp = false;
	mBombProp = false;
	mShovelProp = false;

	// ��ͬ������ݲ�ͬ
	if (player_id == 0)
	{
		m12PImage_x = 233;									// 1P\2P ����
		m12PImage_y = 129;
		mPlayerTankIcoImage_x = 232;						// ���̹��ͼ������
		mPlayerTankIcoImage_y = 137;
		mPlayerLife_x = 240;								// �������ֵ����
		mPlayerLife_y = 137;
		mTankX = 4 * 16 + BOX_SIZE;							// ̹���״γ���ʱ�����������
		mTankY = 12 * 16 + BOX_SIZE;

		mTankTimer.SetDrtTime(mMoveSpeedDev[mPlayerTankLevel]);		// ̹���ƶ��ٶ�, ��ͬ����ͬ��� ��һ��
		mBulletTimer.SetDrtTime(mBulletSpeedDev[mPlayerTankLevel]);

		// ���die ����ʾ���Ƶ� GAMEOVER ����
		//mGameOverX = 0;
		//mGameOver_Dev = 3;
		//mGameOver_end_x = 53;
	}
	else
	{
		m12PImage_x = 233;
		m12PImage_y = 153;
		mPlayerTankIcoImage_x = 232;
		mPlayerTankIcoImage_y = 161;
		mPlayerLife_x = 240;
		mPlayerLife_y = 161;
		mTankX = 8 * 16 + BOX_SIZE;
		mTankY = 12 * 16 + BOX_SIZE;

		mTankTimer.SetDrtTime(mMoveSpeedDev[mPlayerTankLevel]);
		mBulletTimer.SetDrtTime(mBulletSpeedDev[mPlayerTankLevel]);

		// ���die ����ʾ���Ƶ� GAMEOVER ����
		/*mGameOverX = 220;
		mGameOver_Dev = -3; 
		mGameOver_end_x = 122;*/
	}

	int i = 0;
	mDied = false;		
	mTankDir = DIR_UP;		// ̹�˷���

	// �ӵ��ṹ����
	int temp_speed[4] = { 2,3,3,4 };			// ���ܳ��� 4 !! ����Ծ�����ж�.����̹�˼�������ӵ��ٶ�ϵ��
	for (i = 0; i < 2; i++)
	{
		mBulletStruct[i].x = SHOOTABLE_X;		// x ���������ж��Ƿ���Է���
		mBulletStruct[i].y = -1000;
		mBulletStruct[i].dir = DIR_UP;
		mBulletStruct[i].mKillId = 0;			// ��¼���еĵл� id

												// ����̹�˼�������ӵ��ٶ�ϵ��
		for (int j = 0; j < 4; j++)
			mBulletStruct[i].speed[j] = temp_speed[j];
	}

	mBullet_1_counter = 9;				// �ӵ� 1 �˶� N ��ѭ����ſ��Է����ӵ� 2 
	mMoving = false;

	// ��ըͼƬ
	for (i = 0; i < 2; i++)
	{
		mBombS[i].mBombX = -100;
		mBombS[i].mBombY = -100;
		mBombS[i].canBomb = false;
		mBombS[i].counter = 0;
	}

	// SendKillNumToScorePanel() ��ʹ��
	mHasSendKillNumToScorePanel = false;

	// �Ƿ���д�Ӫ
	//mIsShootCamp = false;

	// ɱ����
	for (i = 0; i< 4; i++)
		mKillEnemyNumber[i] = 0;

	mPause = false;
	mPauseCounter = 0;

	// ̹���Ƿ��ڱ����ƶ�
	mOnIce = false;
	mAutoMove = false;
	mAutoMove_Counter = 0;
	mRandCounter = rand() % 6 + 3;

	// ��ұ��������ʾͼƬ GAMEOVER
	/*mGameOverY = 191;
	mGameOverCounter = 0;
	mShowGameOver = false;*/
}

// ������ҵ�һЩ����: 1P\2P ̹��ͼ�� ����
void PlayerBase::DrawPlayerTankIco(const HDC& right_panel_hdc)
{
	// ����1P/2P
	TransparentBlt(right_panel_hdc, m12PImage_x, m12PImage_y, PLAYER_12_ICO_SIZE_X, PLAYER_12_ICO_SIZE_Y,
							GetImageHDC(&m12PImage), 0, 0, PLAYER_12_ICO_SIZE_X, PLAYER_12_ICO_SIZE_Y, 0xffffff);
	// ����̹��ͼ��
	TransparentBlt(right_panel_hdc, mPlayerTankIcoImage_x, mPlayerTankIcoImage_y, PLAYER_TANK_ICO_SIZE_X, PLAYER_TANK_ICO_SIZE_Y,
									GetImageHDC(&mPlayerTankIcoImage), 0, 0, PLAYER_TANK_ICO_SIZE_X, PLAYER_TANK_ICO_SIZE_Y, 0x000000);
	// ���������������
	TransparentBlt(right_panel_hdc, mPlayerLife_x, mPlayerLife_y, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE,
		GetImageHDC(&mBlackNumberImage), BLACK_NUMBER_SIZE * mPlayerLife, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff );
}

bool PlayerBase::ShowStar(const HDC& center_hdc)
{
	switch (mStar.ShowStar(center_hdc, mTankX, mTankY))
	{
	case Star_State::Star_Showing:
		break;

	case Star_State::Star_Stop:
		SignBox_8(mTankX, mTankY, _EMPTY);		// ��ֹ��һ��Ƶ�ͼ��̹�˳��ֵ�λ���ڵ�ס
		SignBox_4(mTankX, mTankY, PLAYER_SIGN + player_id);
		mRing.SetShowable(3222);
		return STOP_SHOW_STAR;

	case Star_State::Tank_Out:
		return STOP_SHOW_STAR;
	}

	return SHOWING_STAR;
}

//
void PlayerBase::DrawPlayerTank(const HDC& canvas_hdc)
{
	if (!mStar.IsStop() || mDied || mBlast.IsBlasting())
		return;

	// 0-5����ʾ̹��. 6-11 ��ʾ.. ��������
	if (mPause && mPauseCounter++ / 10 % 2 != 0)
	{
		//printf("adas3432\n");
		if (mPauseCounter > 266)
			mPause = false;
		return;
	}

	IMAGE tank = mPlayerTank->GetTankImage(mPlayerTankLevel, mTankDir, mMoving);
	TransparentBlt(canvas_hdc, (int)(mTankX - BOX_SIZE), (int)(mTankY - BOX_SIZE), BOX_SIZE * 2, BOX_SIZE * 2, GetImageHDC(&tank), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

	// // ��ʾ������
	if (mRing.canshow)
	{
		if (mRing.ShowRing(canvas_hdc, mTankX, mTankY) == false)
		{
			// ��ֹ��ұ�ը��������ʱ��,����λ�ñ��л�ռ��,Ȼ����ұ�Ǳ�����
			SignBox_4(mTankX, mTankY, PLAYER_SIGN + player_id);
		}
	}
}

void PlayerBase::DrawBullet(const HDC & center_hdc)
{
	//if (mDied)
	//	return;

	for (int i = 0; i < 2; i++)
	{
		// �ӵ����ƶ�
		if (mBulletStruct[i].x != SHOOTABLE_X)
		{
			int dir = mBulletStruct[i].dir;

			TransparentBlt(center_hdc, mBulletStruct[i].x, mBulletStruct[i].y, BulletStruct::mBulletSize[dir][0],
				BulletStruct::mBulletSize[dir][1], GetImageHDC(&BulletStruct::mBulletImage[dir]),
				0, 0, BulletStruct::mBulletSize[dir][0], BulletStruct::mBulletSize[dir][1], 0x000000);
		}
	}
}

//
bool PlayerBase::PlayerControl()
{
	if ( mDied || mBlast.IsBlasting() || !mStar.IsStop())
		return true;

	// 
	if (mAutoMove)
	{
		if (mAutoMove_Counter++ < mRandCounter)
			Move(mTankDir);
		else
		{
			mAutoMove = false;
			mAutoMove_Counter = 0;
		}
	}

	switch (player_id)
	{
	case 0:										// ���һ
		if (GetAsyncKeyState('A') & 0x8000)
		{
			// ͬ�����ƶ��ſ����Զ��ƶ�
			if (mOnIce && mTankDir == DIR_LEFT )	// ���̹�˳����� ��
			{
				mAutoMove = true;
				mAutoMove_Counter = 0;
				mRandCounter = rand() % 8 + 7;
			}
			if (mMoving == false) {
				MciSound::PlayMovingSound(true);
			}
			mMoving = true;
			Move(DIR_LEFT);
		}
		else if (GetAsyncKeyState('W') & 0x8000)
		{
			// ͬ�����ƶ��ſ����Զ��ƶ�
			if (mOnIce && mTankDir == DIR_UP)
			{
				mAutoMove = true;
				mAutoMove_Counter = 0;
				mRandCounter = rand() % 8 + 7;
			}

			if (mMoving == false)
				MciSound::PlayMovingSound(true);
			mMoving = true;
			Move(DIR_UP);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			// ͬ�����ƶ��ſ����Զ��ƶ�
			if (mOnIce && mTankDir == DIR_RIGHT)
			{
				mAutoMove = true;
				mAutoMove_Counter = 0;
				mRandCounter = rand() % 8 + 7;
			}

			if (mMoving == false)
				MciSound::PlayMovingSound(true);
			mMoving = true;
			Move(DIR_RIGHT);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			// ͬ�����ƶ��ſ����Զ��ƶ�
			if (mOnIce && mTankDir == DIR_DOWN)
			{
				mAutoMove = true;
				mAutoMove_Counter = 0;
				mRandCounter = rand() % 8 + 7;
			}
			if (mMoving == false)
				MciSound::PlayMovingSound(true);
			mMoving = true;
			Move(DIR_DOWN);
		}
		else if (mMoving)		// �ɿ�������ֹͣ�����ƶ�������������²������ɿ����������ź�����������ͣ����������������
		{
			mMoving = false;
			MciSound::PlayMovingSound(false);
		}

		// ���ܼ� else if, ��Ȼ�ƶ�ʱ���޷������ӵ�
		if (GetAsyncKeyState('J') & 0x8000)	// �����ӵ�
		{
			if (!ShootBullet(0))
			{
				if (ShootBullet(1))
					MciSound::_PlaySound(S_SHOOT1);
			}
			else
				MciSound::_PlaySound(S_SHOOT0);
		}
		break;

	case 1:										// ��Ҷ�
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			// ͬ�����ƶ��ſ����Զ��ƶ�
			if (mOnIce && mTankDir == DIR_LEFT)
			{
				mAutoMove = true;
				mAutoMove_Counter = 0;
				mRandCounter = rand() % 8 + 7;
			}

			if (mMoving == false)
				MciSound::PlayMovingSound(true);
			mMoving = true;
			Move(DIR_LEFT);
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			// ͬ�����ƶ��ſ����Զ��ƶ�
			if (mOnIce && mTankDir == DIR_UP)
			{
				mAutoMove = true;
				mAutoMove_Counter = 0;
				mRandCounter = rand() % 8 + 7;
			}

			if (mMoving == false)
				MciSound::PlayMovingSound(true);
			mMoving = true;
			Move(DIR_UP);
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			// ͬ�����ƶ��ſ����Զ��ƶ�
			if (mOnIce && mTankDir == DIR_RIGHT)
			{
				mAutoMove = true;
				mAutoMove_Counter = 0;
				mRandCounter = rand() % 8 + 7;
			}

			if (mMoving == false)
				MciSound::PlayMovingSound(true);
			mMoving = true;
			Move(DIR_RIGHT);
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			// ͬ�����ƶ��ſ����Զ��ƶ�
			if (mOnIce && mTankDir == DIR_DOWN)
			{
				mAutoMove = true;
				mAutoMove_Counter = 0;
				mRandCounter = rand() % 8 + 7;
			}

			if (mMoving == false)
				MciSound::PlayMovingSound(true);
			mMoving = true;
			Move(DIR_DOWN);
		}
		else if (mMoving)
		{
			MciSound::PlayMovingSound(false);
			mMoving = false;
		}

		// ���ּ� 1 �����ӵ�
		if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
		{
			if (!ShootBullet(0))
			{
				if (ShootBullet(1))
					MciSound::_PlaySound(S_SHOOT3);
			}
			else
				MciSound::_PlaySound(S_SHOOT2);
		}
		break;
	default:
		break;
	}
	return true;
}

//
BulletShootKind PlayerBase::BulletMoving(const HDC& center_hdc)
{
	if (/* mDied*/ mBulletTimer.IsTimeOut() == false)
		return BulletShootKind::None;


	for (int i = 0; i < 2; i++)
	{
		// �ӵ����ƶ�
		if (mBulletStruct[i].x != SHOOTABLE_X)
		{
			int dir = mBulletStruct[i].dir;

			// �������ϰ������
			BulletShootKind kind = CheckBomb(i);
			if (kind == BulletShootKind::Camp || kind == BulletShootKind::Player_1 || kind == BulletShootKind::Player_2)
				return kind;
			else if (kind == BulletShootKind::Other )
				continue;

			// �ȼ����ȡ�����
			SignBullet(mBulletStruct[i].x, mBulletStruct[i].y, dir, _EMPTY);

			mBulletStruct[i].x += mDevXY[dir][0] * mBulletStruct[i].speed[mPlayerTankLevel];
			mBulletStruct[i].y += mDevXY[dir][1] * mBulletStruct[i].speed[mPlayerTankLevel];

			// ��¼�ӵ� 1 �Ĳ���, �����ɷ����ӵ� 2
			if ( i == 0 )
				mBullet_1_counter--;

			SignBullet(mBulletStruct[i].x, mBulletStruct[i].y, dir, P_B_SIGN + player_id * 10 + i);
		}
	}

	return BulletShootKind::None;
}


void PlayerBase::Bombing(const HDC& center_hdc)
{
	int index[6] = {0,1,1,2,2,1};
	for (int i = 0; i < 2; i++)
	{
		if (mBombS[i].canBomb)
		{
			TransparentBlt(center_hdc, mBombS[i].mBombX - BOX_SIZE, mBombS[i].mBombY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&BombStruct::mBombImage[index[mBombS[i].counter % 6]]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
		// bug?	if (mBombTimer.IsTimeOut())
			{
				if (mBombS[i].counter++ >= 6)
				{
					mBombS[i].counter = 0;
					mBombS[i].canBomb = false;
				}
			}
		}
	}
}

// GameControl �ڵ���, ͨ�������� mBulletStruct.mKillId ���ݽ�ȥ
void PlayerBase::GetKillEnemy(int& bullet1, int& bullet2)
{
	bullet1 = mBulletStruct[0].mKillId;
	bullet2 = mBulletStruct[1].mKillId;
	
	// ���ñ�־
	mBulletStruct[0].mKillId = 0;
	mBulletStruct[1].mKillId = 0;
}

//
//bool PlayerBase::IsShootCamp()
//{
	//return mIsShootCamp;
//}

void PlayerBase::BeKill()
{
	// �����ʾ�ű����������ܵ�����
	if (mRing.canshow)
		return;
	
	MciSound::_PlaySound(S_PLAYER_BOMB);
	SignBox_4(mTankX, mTankY, _EMPTY);
	/*m mDied = true;*/		// �������� flag , ����ƶ�����ֵ!!

	// ���ñ�ը����
	//mBlast.blastx = ;
	//mBlast.blasty = ;
	//mBlast.canBlast = true;

	mBlast.SetBlasting(mTankX, mTankY);
}

// ��ұ����б�ը
bool PlayerBase::Blasting(const HDC & center_hdc)
{
	BlastState result = mBlast.Blasting(center_hdc);
	switch (result)
	{
	case BlastState::NotBlast:
		break;

	case BlastState::Blasting:
		break;

	case BlastState::BlastEnd:
		// ����Ƿ��������
		if (mPlayerLife-- <= 0)
		{
			mDied = true;
			//mPlayerGameover.SetShow();
			mPlayerLife = 0;
			return true;
		}
		else
			Reborn();
		break;

	default:
		break;
	}
	return false;
}

//
const int& PlayerBase::GetID()
{
	return player_id;
}

/*GameControl ��ѭ������*/
bool PlayerBase::IsGetTimeProp()
{
	bool temp = mTimeProp;
	mTimeProp = false;
	return temp;
}

bool PlayerBase::IsGetShvelProp()
{
	if (mShovelProp)
	{
		// �ջ�ò��ӵ���
		if (mShovelProp_counter++ == 0)
		{
			ProtectCamp(_STONE);
		}
		else if (mShovelProp_counter > 31000 && mShovelProp_counter < 35400 )		// ��ʾһ��ʱ�����˸
		{
			int val = _STONE;
			if (mShovelProp_counter % 12 < 6)
				val = _STONE;
			else
				val = _WALL;

			ProtectCamp(val);
		}else if (mShovelProp_counter > 35400)
		{
			mShovelProp = false;
			mShovelProp_counter = 0;
			ProtectCamp(_WALL);
		}
	}
	return false;
}

/*GameControl ��ѭ������*/
bool PlayerBase::IsGetBombProp()
{
	bool temp = mBombProp;
	mBombProp = false;
	return temp;
}

//
void PlayerBase::ShowProp(const HDC& center_hdc)
{
	mProp.ShowProp(center_hdc);
}

//
bool PlayerBase::ShowScorePanel(const HDC& image_hdc)
{
	return mScorePanel->show(image_hdc);// ���Ż���������ʾ image ��������
}

//
void PlayerBase::SetPause()
{
	mPause = true;
	mPauseCounter = 0;
}

//
void PlayerBase::SetShowProp()
{
	int n = 0;
	int m = 0;
	for ( int i = 0; i < 50; i++)
	{
		n = rand() % 25;
		m = rand() % 25;
		if (CheckBox_8(n, m))
			break;
	}
	MciSound::_PlaySound(S_PROPOUT);
	mProp.StartShowProp(n, m);
}
void PlayerBase::AddKillEnemyNum(byte enemy_level)
{
	mKillEnemyNumber[enemy_level]++;
}
void PlayerBase::ResetScorePanelData(const int& player_num, const int& stage)
{
	mScorePanel->ResetData(mKillEnemyNumber, player_num, stage);
}
bool PlayerBase::IsLifeEnd()
{
	return mDied;// mPlayerLife <= 0;
}
void PlayerBase::CheckShowGameOver(const HDC & center_hdc)
{
	mPlayerGameover.Show(center_hdc);

	/*if (mGameOverCounter > 70)
		mShowGameOver = false;

	if (!mShowGameOver)
		return;

	TransparentBlt(center_hdc, mGameOverX, mGameOverY, 31, 15, GetImageHDC(&mGameOverImage), 0 ,0, 31, 15, 0x000000 );

	if (mGameOverTimer.IsTimeOut() == false)
		return;

	if (abs(mGameOverX - mGameOver_end_x) < 5)
	{
		mGameOverCounter++;
		mGameOverX = mGameOver_end_x;
	}
	else
		mGameOverX += mGameOver_Dev;*/
}
void PlayerBase::SetShowGameover()
{
	mPlayerGameover.SetShow();
}
/////////////////////////////////////////////////////////////


void PlayerBase::SignBullet(int lx, int ty, byte dir, int val)
{
	// ת����ͷ����
	int hx = lx + BulletStruct::devto_head[dir][0];
	int hy = ty + BulletStruct::devto_head[dir][1];

	// ת���� 4*4 �����±�����
	int b4i = hy / SMALL_BOX_SIZE;
	int b4j = hx / SMALL_BOX_SIZE;
	if (b4i > 51 || b4j > 51 || b4i < 0 || b4j < 0)
	{
		//printf("adad��ˮ��  %d, %d\n", lx, ty);
		return;
	}

	bms->bullet_4[b4i][b4j] = val;
}

//---------------------------------------------------------------- private function ---------
void PlayerBase::Reborn()
{
	/*m mDied = false;*/
	mTankX = (4 + 4 * player_id) * 16 + BOX_SIZE;				// ̹���״γ���ʱ�����������
	mTankY = 12 * 16 + BOX_SIZE;
	SignBox_4(mTankX, mTankY, PLAYER_SIGN + player_id);		// ̹�˳���, ���Ľ��Ǳ�Ǹ�Ϊ̹�˱��

	mPlayerTankLevel = 0;				// ̹�˼��� [0-3]
	mTankTimer.SetDrtTime(mMoveSpeedDev[mPlayerTankLevel]);
	mBulletTimer.SetDrtTime(mBulletSpeedDev[mPlayerTankLevel]);

	mTankDir = DIR_UP;		// ̹�˷���

	/*for (int i = 0; i < 2; i++)
	{
		mBulletStruct[i].x = SHOOTABLE_X;		// x ���������ж��Ƿ���Է���
		mBulletStruct[i].y = -1000;
		mBulletStruct[i].dir = DIR_UP;
		mBulletStruct[i].mKillId = 0;			// ��¼���еĵл� id
	}*/

	mBullet_1_counter = 6;				// �ӵ� 1 �˶� N ��ѭ����ſ��Է����ӵ� 2 
	mMoving = false;
	mRing.SetShowable(3222);			// ��ʾ������
}

void PlayerBase::DispatchProp(int prop_kind)
{
	MciSound::_PlaySound(S_GETPROP);
	mProp.StopShowProp(true);

	switch (prop_kind)
	{
	case ADD_PROP:			// �ӻ�
		MciSound::_PlaySound(S_ADDLIFE);
		mPlayerLife = mPlayerLife + 1 > 5 ? 5 : mPlayerLife + 1;
		break;
	case STAR_PROP:			// �����
		mPlayerTankLevel = mPlayerTankLevel + 1 > 3 ? 3 : mPlayerTankLevel + 1;
		mTankTimer.SetDrtTime(mMoveSpeedDev[mPlayerTankLevel]);
		mBulletTimer.SetDrtTime(mBulletSpeedDev[mPlayerTankLevel]);
		break;
	case TIME_PROP:			// ʱ��
		mTimeProp = true;
		break;
	case  BOMB_PROP:		// ����
		mBombProp = true;
		break;
	case SHOVEL_PROP:		// ����
		mShovelProp = true;
		break;
	case  CAP_PROP:			// ñ��
		mRing.SetShowable(12000);
		break;
	default:
		break;
	}
}

/*
* �����ͬʱ����̹�����ڸ���. ���뱣֤̹�������ڸ�������
 * ����ƶ���ʱ�� mTankTimer δ��ʱ	�����ƶ�
 * �������ֵ�ù��					�����ƶ�
 * ���̹�����ڱ�ը					�����ƶ�
 * ��ұ���һ����һ��к�				�����ƶ�

 * �ƶ�ǰ���̹�� mTankX,mTankY ���ڵ� box_4 �ĸ����� = �գ���ʾ�ô�û�ж�������
 * ����Ǳ�����ô���� mTankX,mTankY ����ȷ�ĸ���λ����
 * ����Ƿ�����ƶ�
 * ��������ƶ��������µ� mTankX, mTankY ����
 * �ƶ����� box_4 �ĸ����� = PLAYER_SIGN + player_id����ʾ�����̹�˴�����λ��

 * �µ����� mTankX, mTankY �Ѿ�������ϣ��������أ���ѭ�����������������»���̹�ˣ��Ӷ�ʵ���ƶ�Ч����
 */
void PlayerBase::Move(int new_dir)
{
	if (!mTankTimer.IsTimeOut() || mDied || mBlast.IsBlasting())
		return;

	// �����ұ���һ����һ�����ͣ
	if (mPause)
	{
		return;
	}

	SignBox_4(mTankX, mTankY, _EMPTY);

	if (mTankDir != new_dir)
	{
		// ԭ���ұ����·���
		if (mTankDir == DIR_LEFT || mTankDir == DIR_RIGHT)
		{
			if (mTankX > (mTankX / BOX_SIZE) * BOX_SIZE + BOX_SIZE / 2 - 1)	// ����ǿ����������ϵ��ұ߽ڵ�, -1������
				mTankX = (mTankX / BOX_SIZE + 1) * BOX_SIZE;
			else
				mTankX = (mTankX / BOX_SIZE) * BOX_SIZE;					// �����������ϵ���߽ڵ�
		}
		// ���±�����
		else
		{
			if (mTankY > (mTankY / BOX_SIZE) * BOX_SIZE + BOX_SIZE / 2 - 1)	// ����ǿ����������ϵ��±߽ڵ�, -1������
				mTankY = (mTankY / BOX_SIZE + 1) * BOX_SIZE;
			else
				mTankY = (mTankY / BOX_SIZE) * BOX_SIZE;					// �����������ϵ��ϱ߽ڵ�
		}

		// ���ķ���, �����ȵ���λ�ú�������÷���!!
		mTankDir = new_dir;
	}
	else								// �ƶ�
	{
		if (CheckMoveable())
		{
			mTankX += mDevXY[mTankDir][0];// *mSpeed[mPlayerTankLevel];
			mTankY += mDevXY[mTankDir][1];// *mSpeed[mPlayerTankLevel];
		}
	}
	SignBox_4(mTankX, mTankY, PLAYER_SIGN + player_id);
}

/* �жϵ�ǰ����ɷ��ƶ�
	box_8[i][j]
	i = y / BOX_SIZE		// y �����Ӧ���� i ֵ
	j = x / BOX_SIZE		// x �����Ӧ���� j ֵ
	---------
	| 1 | 2	|
	----a----
	| 3 | 4	|
	---------
* ��� (x,y) �� a ��, ת����� i,j ���ڸ��� 4
* ��� x ֵ�� a �����, ��ת����� j ���� 1��3; �ұ������� 2��4
* ��� y ֵ�� a ������, ��ת����� i ���� 1��2; ���������� 3��4
** ��� tempx,tempy ��Խ�˸����������ϰ�, ��ô�ͽ� mTankX �� mTankY ��������������,
*/
bool PlayerBase::CheckMoveable()
{
	// ̹����������
	int tempx = mTankX + mDevXY[mTankDir][0];// *mSpeed[mPlayerTankLevel];
	int tempy = mTankY + mDevXY[mTankDir][1];// *mSpeed[mPlayerTankLevel];

	// ��Ϸ��������һ�� 208*208 �Ļ����ϵģ�����ʵ����Ϸ�����С�� 208*208
	// ��������� tempx < 8 ��̹���������� < 8 ��˵����ʱ�Ѿ�Խ�磨��Ϊ�����ĵ㣩������˵̹���Ѿ��ƶ����߽���
	if (tempx < BOX_SIZE || tempy < BOX_SIZE || tempy > CENTER_WIDTH - BOX_SIZE || tempx > CENTER_HEIGHT - BOX_SIZE)
	{
		// ��������ϰ���,��̹�������������������. ��Ȼ̹�˺��ϰ�����м������ص���
		switch (mTankDir)
		{
		case DIR_LEFT:	mTankX = (mTankX / BOX_SIZE) * BOX_SIZE;	break;	// mTankX �� tempx ֮���Խ�˸���, ��̹�˷ŵ�mTankX���ڵĸ�������
		case DIR_UP:	mTankY = (mTankY / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_RIGHT: mTankX = (tempx  / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_DOWN:	mTankY = (tempy  / BOX_SIZE) * BOX_SIZE;	break;
		default:													break;
		}
		return false;
	}
	// ת�����ص����ڵ� xy[26][26] �±�
	int index_i = tempy / BOX_SIZE;
	int index_j = tempx / BOX_SIZE;

	// �ĸ�����̹�����ĵ������Ҫ�������� 8*8 ���ӵ��±�ƫ����
	int dev[4][2][2] = { {{-1,-1},{0,-1}},  {{-1,-1},{-1,0}},  {{-1,1},{0,1}}, { {1,-1},{1,0}} };

	// 8*8 �ϰ�����Ӽ��
	int temp1 = bms->box_8[index_i + dev[mTankDir][0][0]][index_j + dev[mTankDir][0][1]];
	int temp2 = bms->box_8[index_i + dev[mTankDir][1][0]][index_j + dev[mTankDir][1][1]];

	// prop_8���߸��Ӽ��
	int curi = mTankY / BOX_SIZE;	// ��ǰ̹�����ڵ�����, ������һ��������, �����жϵ���
	int curj = mTankX / BOX_SIZE;
	int prop[4] = { bms->prop_8[curi][curj],	 bms->prop_8[curi - 1][curj], 
					bms->prop_8[curi][curj - 1], bms->prop_8[curi - 1][curj - 1] };
	for (int i = 0; i < 4; i++)
	{
		if (prop[i] >= PROP_SIGN && prop[i] < PROP_SIGN + 6)
		{
			DispatchProp(prop[i] - PROP_SIGN);
			break;
		}
	}

	// ���̹�� 4*4 ����
	// �ĸ�������Ҫ�������� 4*4 �ĸ�����̹���������� 4*4 ���ӵ��±�ƫ����
	int  dev_4[4][4][2] = { {{-2,-2},{1,-2},{-1,-2},{0,-2}}, {{-2,-2},{-2,1},{-2,-1},{-2,0}},
							{{-2,2},{1,2},{-1,2},{0,2}}, {{2,-2},{2,1},{2,-1},{2,0}} };
	// ת���� [52][52] �±�
	int index_4i = tempy / SMALL_BOX_SIZE;
	int index_4j = tempx / SMALL_BOX_SIZE;
	
	// -1, 0, 1, 2 �������ƶ�
	bool tank1 = bms->box_4[index_4i + dev_4[mTankDir][0][0]][index_4j + dev_4[mTankDir][0][1]] <= _ICE;
	bool tank2 = bms->box_4[index_4i + dev_4[mTankDir][1][0]][index_4j + dev_4[mTankDir][1][1]] <= _ICE;
	bool tank3 = bms->box_4[index_4i + dev_4[mTankDir][2][0]][index_4j + dev_4[mTankDir][2][1]] <= _ICE;
	bool tank4 = bms->box_4[index_4i + dev_4[mTankDir][3][0]][index_4j + dev_4[mTankDir][3][1]] <= _ICE;

	// �����ϰ����������
	if (temp1 > 2 || temp2 > 2)
	{
		// ��������ϰ���,��̹�������������������. ��Ȼ̹�˺��ϰ�����м������ص���
		switch (mTankDir)
		{
		case DIR_LEFT:	mTankX = (mTankX / BOX_SIZE) * BOX_SIZE;	break;	// mTankX �� tempx ֮���Խ�˸���, ��̹�˷ŵ�mTankX���ڵĸ�������
		case DIR_UP:	mTankY = (mTankY / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_RIGHT: mTankX = (tempx / BOX_SIZE) * BOX_SIZE;		break;
		case DIR_DOWN:	mTankY = (tempy / BOX_SIZE) * BOX_SIZE;		break;
		default:													break;
		}
		return false;
	}
	// ������Ҳ��õ���
	else if (!tank1 || !tank2 || !tank3 || !tank4)
		return false;

	//. ����ǵ�һ�ν��� _ICE ����
	if ( !mOnIce && (temp1 == _ICE || temp2 == _ICE))
		mOnIce = true;
	if (mOnIce && temp1 != _ICE && temp2 != _ICE)
		mOnIce = false;
	return true;
}

// �����ӵ�
bool PlayerBase::ShootBullet( int bullet_id )
{
	switch (bullet_id)
	{
		case 0:
			// 1���ӵ�����ʧ��, ��ըδ���ǰ���ܷ���
			if (mBulletStruct[0].x != SHOOTABLE_X || mBombS[0].canBomb == true)		
				return false;

			// �ӵ����������
			mBulletStruct[0].x = (int)(mTankX + BulletStruct::devto_tank[mTankDir][0]);
			mBulletStruct[0].y = (int)(mTankY + BulletStruct::devto_tank[mTankDir][1]);
			mBulletStruct[0].dir = mTankDir;
			mBullet_1_counter = 6;

			SignBullet(mBulletStruct[0].x, mBulletStruct[0].y, mBulletStruct[0].dir, P_B_SIGN + player_id * 10 + bullet_id );
			//_PlayerSound(NULL, L"shoot", L"shoot");
			return true;

		case 1:
			// 2 ���ӵ�����ʧ��
			if (mPlayerTankLevel < 2 || mBulletStruct[1].x != SHOOTABLE_X || mBullet_1_counter > 0 || mBombS[1].canBomb == true)
				return false;

			// �ӵ����������
			mBulletStruct[1].x = (int)(mTankX + BulletStruct::devto_tank[mTankDir][0]);
			mBulletStruct[1].y = (int)(mTankY + BulletStruct::devto_tank[mTankDir][1]);
			mBulletStruct[1].dir = mTankDir;

			SignBullet(mBulletStruct[1].x, mBulletStruct[1].y, mBulletStruct[1].dir, P_B_SIGN + player_id * 10 + bullet_id);
			//_PlayerSound(NULL, L"shoot", L"shoot");
			return true;

		default:
			break;
	}
	return false;
}

//
BulletShootKind PlayerBase::CheckBomb(int i)
{
	//if (mBombS[i].canBomb)
	//	return true;
	int dir = mBulletStruct[i].dir;

	// �ӵ�ͷ�Ӵ����ϰ�����Ǹ���, ���ҷ��������, ���·��������
	int bombx = mBulletStruct[i].x + BulletStruct::devto_head[dir][0];
	int bomby = mBulletStruct[i].y + BulletStruct::devto_head[dir][1];

	bool flag = false;
	int adjust_x = 0, adjust_y = 0;		// ��������ͼƬ��ʾ������

	// ������ bombx ���� mBulletStruct[i].x,����Ḳ���ϰ���ļ��
	if (mBulletStruct[i].x < 0 && mBulletStruct[i].dir == DIR_LEFT)
	{
		flag = true;
		adjust_x = 5;					// ����ըͼƬ������һ��
	}
	else if (mBulletStruct[i].y < 0 && mBulletStruct[i].dir == DIR_UP)
	{
		flag = true;
		adjust_y = 5;
	}

	// �����ȥ�ӵ��Ŀ� 4, ��Ȼ�ӵ�Խ��, �����⵼�� box_8 �±�Խ��
	else if (mBulletStruct[i].x >= CENTER_WIDTH - 4 && mBulletStruct[i].dir == DIR_RIGHT)
	{
		flag = true;
		adjust_x = -4;
	}
	else if (mBulletStruct[i].y >= CENTER_HEIGHT - 4 && mBulletStruct[i].dir == DIR_DOWN)
	{
		flag = true;
		adjust_y = -4;
	}
	if (flag)
	{
		// �趨��ը����, ������ը�������ڵĸ���,���һ�����ƫ��һ������֮���..
		mBulletStruct[i].x = SHOOTABLE_X;
		mBombS[i].canBomb = true;
		mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
		mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
		mBombS[i].counter = 0;

		MciSound::_PlaySound(S_BIN);

		return BulletShootKind::Other;
	}

	int tempi, tempj;

	// �������� 8*8 ���ӵ�����
	int b8i = bomby / BOX_SIZE;
	int b8j = bombx / BOX_SIZE;

	// ������ת���� 4*4 ��������
	int b4i = bomby / SMALL_BOX_SIZE;
	int b4j = bombx / SMALL_BOX_SIZE;

	//if (b4i > 51 || b4j > 51 || b4i < 0 || b4j < 0)
	//	printf("22222222\n");

	// �����������һ������ӵ�
	if (bms->bullet_4[b4i][b4j] == P_B_SIGN + (1 - player_id) * 10 + 0 || 
		bms->bullet_4[b4i][b4j] == P_B_SIGN + (1 - player_id) * 10 + 1 ||
		bms->bullet_4[b4i][b4j] == E_B_SIGN )
	{
		mBulletStruct[i].x = SHOOTABLE_X;
		bms->bullet_4[b4i][b4j] = WAIT_UNSIGN;		// �ȱ���м�ֵ, �ȴ������е��ӵ������ñ��
		return BulletShootKind::Other;
	}
	else if (bms->bullet_4[b4i][b4j] == WAIT_UNSIGN)
	{
		mBulletStruct[i].x = SHOOTABLE_X;
		bms->bullet_4[b4i][b4j] = _EMPTY;
		return BulletShootKind::Other;
	}

	switch (mBulletStruct[i].dir)
	{
	case DIR_LEFT:
	case DIR_RIGHT:
	{
		// ������Ӻ���һ��
		int temp[2][2] = { {0, 0}, {-1, 0} };
		for (int n = 0; n < 2; n++)
		{
			// 8*8 ����, �ж��Ƿ���ел�
			tempi = b8i + temp[n][0];
			tempj = b8j + temp[n][1];
			if (bms->box_8[tempi][tempj] == CAMP_SIGN)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].counter = 0;
				//mIsShootCamp = true;
				SignBox_8(13 * BOX_SIZE, 25 * BOX_SIZE, _EMPTY);

				/*int iy = (25 * BOX_SIZE / BOX_SIZE)*2 - 2;
				int jx = (13 * BOX_SIZE / BOX_SIZE)*2 - 2;
				for (int i = iy; i < iy + 4; i++)
				{
				for (int j = jx; j < jx + 4; j++)
				{
				printf("%d, \n", bms->box_4[i][j]);
				}
				}*/

				return BulletShootKind::Camp;
			}

			// ���Ҽ���ӵ�ͷ���ڵ�4*4���Ӻ����������ڵ��Ǹ�
			// ��� 4*4 ����, �ɴ��ж��ϰ���
			tempi = b4i + temp[n][0];
			tempj = b4j + temp[n][1];
			if (bms->box_4[tempi][tempj] == _WALL || bms->box_4[tempi][tempj] == _STONE)
			{
				// �趨��ը����, ������ը�������ڵĸ���,���һ�����ƫ��һ������֮���..
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// ָʾ i bomb ��ը
				mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;
				ClearWallOrStone(i, bombx, bomby);
				return BulletShootKind::Other;
			}
			else if (bms->box_4[tempi][tempj] >= ENEMY_SIGN /*&& bms->box_4[tempi][tempj] < ENEMY_SIGN + TOTAL_ENEMY_NUMBER*/)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// ָʾ i bomb ��ը
				mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;

				// ��ǻ����˵л��� id
				mBulletStruct[i].mKillId = bms->box_4[tempi][tempj];
				//mKillEnemyNumber[bms->box_4[tempi][tempj] % 10000 / 1000]++;	// ��¼����л��ļ������������

				//	mProp->StartShowProp(100, 100);
				return BulletShootKind::Other;
			}
			else if (bms->box_4[tempi][tempj] == PLAYER_SIGN && player_id != 0 || bms->box_4[tempi][tempj] == PLAYER_SIGN + 1 && player_id != 1)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// ָʾ i bomb ��ը
				mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;
				return (BulletShootKind)bms->box_4[tempi][tempj];
			}
		}
	}
	break;

	// ����ֻ����������ڵ���������
	case DIR_UP:
	case DIR_DOWN:
	{
		// ������Ӻ������һ������
		int temp[2][2] = { { 0, 0 },{ 0, -1 } };
		for (int n = 0; n < 2; n++)
		{
			// 8*8 ����, �ж��Ƿ���ел�
			tempi = b8i + temp[n][0];
			tempj = b8j + temp[n][1];

			if (bms->box_8[tempi][tempj] == CAMP_SIGN)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].counter = 0;
				//mIsShootCamp = true;
				SignBox_8(13 * BOX_SIZE, 25 * BOX_SIZE, _EMPTY);
				return BulletShootKind::Camp;
			}

			// ��� 4*4 �Ƿ�����ϰ�
			tempi = b4i + temp[n][0];
			tempj = b4j + temp[n][1];
			if (bms->box_4[tempi][tempj] == _WALL || bms->box_4[tempi][tempj] == _STONE)
			{
				// �趨��ը����, ������ը�������ڵĸ���,���һ�����ƫ��һ������֮���..
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// ָʾ i bomb ��ը
				mBombS[i].mBombX =( bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY =( bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;
				ClearWallOrStone(i, bombx, bomby );
				return BulletShootKind::Other;
			}
			else if (bms->box_4[tempi][tempj] >= ENEMY_SIGN/* && bms->box_4[tempi][tempj] < ENEMY_SIGN + TOTAL_ENEMY_NUMBER*/)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// ָʾ i bomb ��ը
				mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;

				// ��ǻ����˵л��� id
				mBulletStruct[i].mKillId = bms->box_4[tempi][tempj];
				//mKillEnemyNumber[bms->box_4[tempi][tempj] % 10000 / 1000]++;		// ��¼����л������������
				//mProp->StartShowProp(100, 100);
				return BulletShootKind::Other;
			}
			else if (bms->box_4[tempi][tempj] == PLAYER_SIGN && player_id != 0 || bms->box_4[tempi][tempj] == PLAYER_SIGN + 1 && player_id != 1)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// ָʾ i bomb ��ը
				mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;
				return (BulletShootKind)bms->box_4[tempi][tempj];
			}
		}
	}
		break;
	default:
		break;
	}
	return BulletShootKind::None;
}

// �ӵ������ϰ��ﱬը���øú���, ���б߽粻�ɵ���, �±��Խ��[52][52]
void PlayerBase::ClearWallOrStone(int bulletid, int bulletx, int bullety)
{
	int boxi = bullety / SMALL_BOX_SIZE;
	int boxj = bulletx / SMALL_BOX_SIZE;
	int tempx, tempy;
	switch (mBulletStruct[bulletid].dir)
	{
	case DIR_LEFT:
	case DIR_RIGHT:
	{
		bool bin_once = false;		// ���ѭ����ֻ����һ������

		// ��ͬһֱ�����ڵ��ĸ� 4*4 ����, ˳���ܱ�, �����õ��±��ж�
		int temp[4][2] = { { -2, 0 },{ -1, 0 },{ 0, 0 },{ 1, 0 } };
		for (int i = 0; i < 4; i++)
		{
			tempx = boxi + temp[i][0];
			tempy = boxj + temp[i][1];

			// ת�� tempx,tempy���ڵ� 8*8 ��������
			int n = tempx / 2;
			int m = tempy / 2;

			if (bms->box_4[tempx][tempy] == _WALL )
			{
				bms->box_4[tempx][tempy] = _CLEAR;

				// ��� 8*8 �����ڵ�4�� 4*4 ��С�����Ƿ�ȫ�������,
				bool isClear = true;
				for (int a = 2 * n; a < 2 * n + 2; a++)
				{
					for (int b = 2 * m; b < 2 * m + 2; b++)
					{
						if (bms->box_4[a][b] != _CLEAR)
							isClear = false;
					}
				}
				if (isClear)
				{
					bms->box_8[n][m] = _EMPTY;
				}
			}
			else if (mPlayerTankLevel < 3 && bms->box_4[tempx][tempy] == _STONE && bin_once == false)
			{
				MciSound::_PlaySound(S_BIN);
				bin_once = true;
			}
			else if (mPlayerTankLevel == 3 && bms->box_4[tempx][tempy] == _STONE && i % 2 == 0)	// %2==0 ��ֹ����ѭ���ж�
			{
				bms->box_8[n][m] = _EMPTY;
				for (int a = 2 * n; a < 2 * n + 2; a++)
				{
					for (int b = 2 * m; b < 2 * m + 2; b++)
					{
						bms->box_4[a][b] = _CLEAR;
					}
				}
			}
		}
	}
	break;

	case DIR_UP:
	case DIR_DOWN:
	{
		bool bin_once = false;		// ���ѭ����ֻ����һ������

		// ���ڵ��ĸ� 4*4 ����, ˳���ܱ�, �����õ��±��ж�
		int temp[4][2] = { {0, -2}, {0, -1}, {0, 0}, {0, 1} };
		for (int i = 0; i < 4; i++)
		{
			tempx = boxi + temp[i][0];
			tempy = boxj + temp[i][1];

			// ת�� tempx,tempy���ڵ� 8*8 ��������
			int n = tempx / 2;
			int m = tempy / 2;

			if (bms->box_4[tempx][tempy] == _WALL)
			{
				bms->box_4[tempx][tempy] = _CLEAR;

				// ��� 8*8 �����ڵ�4�� 4*4 ��С�����Ƿ�ȫ�������,
				bool isClear = true;
				for (int a = 2 * n; a < 2 * n + 2; a++)
				{
					for (int b = 2 * m; b < 2 * m + 2; b++)
					{
						if (bms->box_4[a][b] != _CLEAR)
							isClear = false;
					}
				}
				if (isClear)
				{
					bms->box_8[n][m] = _EMPTY;
				}
			}
			else if (mPlayerTankLevel < 3 && bms->box_4[tempx][tempy] == _STONE && bin_once == false)
			{
				MciSound::_PlaySound(S_BIN);
				bin_once = true;
			}
			else if (mPlayerTankLevel == 3 && bms->box_4[tempx][tempy] == _STONE && i % 2 == 0)	// %2==0 ��ֹ����ѭ���ж�
			{
				bms->box_8[n][m] = _EMPTY;
				for (int a = 2 * n; a < 2 * n + 2; a++)
				{
					for (int b = 2 * m; b < 2 * m + 2; b++)
					{
						bms->box_4[a][b] = _CLEAR;
					}
				}
			}

			// ��� 8*8 �����ڵ�4�� 4*4 ��С�����Ƿ�ȫ�������,
			/*bool isClear = true;
			for (int a = 2 * n; a < 2 * n + 2; a++)
			{
				for (int b = 2 * m; b < 2 * m + 2; b++)
				{
					if (bms->box_4[a][b] != _CLEAR)
						isClear = false;
				}
			}
			if (isClear)
			{
				bms->box_8[n][m] = _EMPTY;
			}*/
		}
	}
	break;

	default:
		break;
	}
}

// ������ 16 * 16 ���ĵ���������, ��̹������������ͬ
void PlayerBase::SignBox_8(int x, int y, int val)
{
	// ��̹����������ת�����Ͻ��Ǹ��� ��������
	int iy = y / BOX_SIZE - 1;
	int jx = x / BOX_SIZE - 1;
	for (int i = iy; i < iy + 2; i++)
	{
		for (int j = jx; j < jx + 2; j++)
		{
			bms->box_8[i][j] = val;
		}
	}
}

// ����̹����������, ���16�� 4*4 ����
void PlayerBase::SignBox_4(int cx, int cy, int val)
{
	// ���ҵ��� cs,cy ��ռ�ݰٷֱ����� 16 �� 4*4 �ĸ�������
	if (mTankDir == DIR_LEFT || mTankDir == DIR_RIGHT)
	{
		if (cx > (cx / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2)	// ����ǿ����ұ߽ڵ�, 
		{
			cx = (cx / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		}
		else {
			cx = (cx / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;					// �����������ϵ���߽ڵ�
		}
	}
	// ����
	else
	{
		if (cy > (cy / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2 )	// ����ǿ��������±߽ڵ�,
			cy = (cy / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		else
			cy = (cy / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;					// �����������ϵ��ϱ߽ڵ�
	}

	// ��̹����������ת�����Ͻ��Ǹ��� ��������
	int iy = cy / SMALL_BOX_SIZE - 2;
	int jx = cx / SMALL_BOX_SIZE - 2;
	for (int i = iy; i < iy + 4; i++)
	{
		for (int j = jx; j < jx + 4; j++)
		{
			bms->box_4[i][j] = val;
		}
	}
}

// ���4��8*8 ����, ���������ϽǸ�������
bool PlayerBase::CheckBox_8(int iy, int jx)
{
	for (int i = iy; i < iy + 2; i++)
	{
		for (int j = jx; j < jx + 2; j++)
		{
			if (bms->box_8[i][j] < _RIVER)		// �����һ�� 8*8 ̹�˿��Խ���
				return true;
		}
	}
	return false;
}

/*��ò��ӵ��߱���camp �ڲ�ʹ��*/
void PlayerBase::ProtectCamp(int val)
{
	for (int i = 23; i < 26; i++)
	{
		for (int j = 11; j < 15; j++)
		{
			if (i < 24 || j < 12 || j > 13)
			{
				bms->box_8[i][j] = val;			// ����Χ�� _WALL
				for (int m = 2 * i; m < 2 * i + 2; m++)
				{
					for (int n = 2 * j; n < 2 * j + 2; n++)
					{
						bms->box_4[m][n] = val;
					}
				}
			}
		}
	}
}

/*void PlayerBase::DisappearBullet(int sign)
{
	int bid = sign % 10;
	mBulletStruct[bid].x = SHOOTABLE_X;
}*/



