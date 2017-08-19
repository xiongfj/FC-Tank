#include "stdafx.h"
#include "PlayerBase.h"
#include "MciSound.h"

//----------------- PlayerBase 类静态数据

int PlayerBase::mDevXY[4][2] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1} };	// 依次左上右下
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

	mPlayerLife = 2;		// 玩家 HP
	mPlayerTankLevel = 0;

	//// 不同级别坦克移动速度系数
	//int temp[4] = { 1, 1, 1, 1 };
	//for (i = 0; i < 4; i++)
	//	mSpeed[i] = temp[i];

	Init();

	// 不同玩家数据不同
	if (player_id == 0)
		loadimage(&m12PImage, _T("./res/big/1P.gif"));		// 1P\2P图标
	else
		loadimage(&m12PImage, _T("./res/big/2P.gif"));

	// 共同的数据
	loadimage(&mPlayerTankIcoImage, _T("./res/big/playertank-ico.gif"	));	// 玩家坦克图标
	loadimage(&mBlackNumberImage,	_T("./res/big/black-number.gif"		));	// 黑色数字
		
	/*********************************
	* BulletStruct 数据初始化
	*********************************/
	// 加载子弹图片
	TCHAR buf[100];
	for (i = 0; i < 4; i++)
	{
		_stprintf_s(buf, _T("./res/big/bullet-%d.gif"), i);
		loadimage(&BulletStruct::mBulletImage[i], buf);
	}

	// 爆炸图片
	for (i = 0; i < 3; i++)
	{
		_stprintf_s(buf, _T("./res/big/bumb%d.gif"), i);
		loadimage(&BombStruct::mBombImage[i], buf);
	}

	mBombTimer.SetDrtTime(20);	// 不能设置太小..
	//mBlastTimer.SetDrtTime(36);

	// 显示分数面板
	mScorePanel = new ScorePanel(player_id);

	//mGameOverTimer.SetDrtTime(20);
	//loadimage(&mGameOverImage, _T("./res/big/gameover.gif") );
}

PlayerBase::~PlayerBase()
{
	delete mPlayerTank;
	delete mScorePanel;

	// 静态数据放置多次 delete
	/*不能这样释放, 当 delete 单个对象的时候, 其它对象还会用到这个静态数据!!!
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

	// 不同玩家数据不同
	if (player_id == 0)
	{
		m12PImage_x = 233;									// 1P\2P 坐标
		m12PImage_y = 129;
		mPlayerTankIcoImage_x = 232;						// 玩家坦克图标坐标
		mPlayerTankIcoImage_y = 137;
		mPlayerLife_x = 240;								// 玩家生命值坐标
		mPlayerLife_y = 137;
		mTankX = 4 * 16 + BOX_SIZE;							// 坦克首次出现时候的中心坐标
		mTankY = 12 * 16 + BOX_SIZE;

		mTankTimer.SetDrtTime(mMoveSpeedDev[mPlayerTankLevel]);		// 坦克移动速度, 不同级别不同玩家 不一样
		mBulletTimer.SetDrtTime(mBulletSpeedDev[mPlayerTankLevel]);

		// 玩家die 后显示右移的 GAMEOVER 字样
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

		// 玩家die 后显示左移的 GAMEOVER 字样
		/*mGameOverX = 220;
		mGameOver_Dev = -3; 
		mGameOver_end_x = 122;*/
	}

	int i = 0;
	mDied = false;		
	mTankDir = DIR_UP;		// 坦克方向

	// 子弹结构数据
	int temp_speed[4] = { 2,3,3,4 };			// 不能超过 4 !! 会跳跃格子判断.根据坦克级别分配子弹速度系数
	for (i = 0; i < 2; i++)
	{
		mBulletStruct[i].x = SHOOTABLE_X;		// x 坐标用于判断是否可以发射
		mBulletStruct[i].y = -1000;
		mBulletStruct[i].dir = DIR_UP;
		mBulletStruct[i].mKillId = 0;			// 记录击中的敌机 id

												// 根据坦克级别分配子弹速度系数
		for (int j = 0; j < 4; j++)
			mBulletStruct[i].speed[j] = temp_speed[j];
	}

	mBullet_1_counter = 9;				// 子弹 1 运动 N 个循环后才可以发射子弹 2 
	mMoving = false;

	// 爆炸图片
	for (i = 0; i < 2; i++)
	{
		mBombS[i].mBombX = -100;
		mBombS[i].mBombY = -100;
		mBombS[i].canBomb = false;
		mBombS[i].counter = 0;
	}

	// SendKillNumToScorePanel() 内使用
	mHasSendKillNumToScorePanel = false;

	// 是否击中大本营
	//mIsShootCamp = false;

	// 杀敌数
	for (i = 0; i< 4; i++)
		mKillEnemyNumber[i] = 0;

	mPause = false;
	mPauseCounter = 0;

	// 坦克是否在冰上移动
	mOnIce = false;
	mAutoMove = false;
	mAutoMove_Counter = 0;
	mRandCounter = rand() % 6 + 3;

	// 玩家被消灭后显示图片 GAMEOVER
	/*mGameOverY = 191;
	mGameOverCounter = 0;
	mShowGameOver = false;*/
}

// 绘制玩家的一些数据: 1P\2P 坦克图标 生命
void PlayerBase::DrawPlayerTankIco(const HDC& right_panel_hdc)
{
	// 绘制1P/2P
	TransparentBlt(right_panel_hdc, m12PImage_x, m12PImage_y, PLAYER_12_ICO_SIZE_X, PLAYER_12_ICO_SIZE_Y,
							GetImageHDC(&m12PImage), 0, 0, PLAYER_12_ICO_SIZE_X, PLAYER_12_ICO_SIZE_Y, 0xffffff);
	// 绘制坦克图标
	TransparentBlt(right_panel_hdc, mPlayerTankIcoImage_x, mPlayerTankIcoImage_y, PLAYER_TANK_ICO_SIZE_X, PLAYER_TANK_ICO_SIZE_Y,
									GetImageHDC(&mPlayerTankIcoImage), 0, 0, PLAYER_TANK_ICO_SIZE_X, PLAYER_TANK_ICO_SIZE_Y, 0x000000);
	// 绘制玩家生命数字
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
		SignBox_8(mTankX, mTankY, _EMPTY);		// 防止玩家绘制地图把坦克出现的位置遮挡住
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

	// 0-5不显示坦克. 6-11 显示.. 依次类推
	if (mPause && mPauseCounter++ / 10 % 2 != 0)
	{
		//printf("adas3432\n");
		if (mPauseCounter > 266)
			mPause = false;
		return;
	}

	IMAGE tank = mPlayerTank->GetTankImage(mPlayerTankLevel, mTankDir, mMoving);
	TransparentBlt(canvas_hdc, (int)(mTankX - BOX_SIZE), (int)(mTankY - BOX_SIZE), BOX_SIZE * 2, BOX_SIZE * 2, GetImageHDC(&tank), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

	// // 显示保护环
	if (mRing.canshow)
	{
		if (mRing.ShowRing(canvas_hdc, mTankX, mTankY) == false)
		{
			// 防止玩家爆炸到结束的时候,重生位置被敌机占用,然后玩家标记被擦除
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
		// 子弹在移动
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
	case 0:										// 玩家一
		if (GetAsyncKeyState('A') & 0x8000)
		{
			// 同方向移动才开启自动移动
			if (mOnIce && mTankDir == DIR_LEFT )	// 如果坦克朝向是 左
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
			// 同方向移动才开启自动移动
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
			// 同方向移动才开启自动移动
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
			// 同方向移动才开启自动移动
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
		else if (mMoving)		// 松开按键，停止播放移动声音；如果按下并快速松开，声音播放后又立即被暂停，所以听不到声音
		{
			mMoving = false;
			MciSound::PlayMovingSound(false);
		}

		// 不能加 else if, 不然移动时候无法发射子弹
		if (GetAsyncKeyState('J') & 0x8000)	// 发射子弹
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

	case 1:										// 玩家二
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			// 同方向移动才开启自动移动
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
			// 同方向移动才开启自动移动
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
			// 同方向移动才开启自动移动
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
			// 同方向移动才开启自动移动
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

		// 数字键 1 发射子弹
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
		// 子弹在移动
		if (mBulletStruct[i].x != SHOOTABLE_X)
		{
			int dir = mBulletStruct[i].dir;

			// 检测打中障碍物与否
			BulletShootKind kind = CheckBomb(i);
			if (kind == BulletShootKind::Camp || kind == BulletShootKind::Player_1 || kind == BulletShootKind::Player_2)
				return kind;
			else if (kind == BulletShootKind::Other )
				continue;

			// 先检测再取消标记
			SignBullet(mBulletStruct[i].x, mBulletStruct[i].y, dir, _EMPTY);

			mBulletStruct[i].x += mDevXY[dir][0] * mBulletStruct[i].speed[mPlayerTankLevel];
			mBulletStruct[i].y += mDevXY[dir][1] * mBulletStruct[i].speed[mPlayerTankLevel];

			// 记录子弹 1 的步数, 决定可否发射子弹 2
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

// GameControl 内调用, 通过参数将 mBulletStruct.mKillId 传递进去
void PlayerBase::GetKillEnemy(int& bullet1, int& bullet2)
{
	bullet1 = mBulletStruct[0].mKillId;
	bullet2 = mBulletStruct[1].mKillId;
	
	// 重置标志
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
	// 如果显示着保护环不会受到攻击
	if (mRing.canshow)
		return;
	
	MciSound::_PlaySound(S_PLAYER_BOMB);
	SignBox_4(mTankX, mTankY, _EMPTY);
	/*m mDied = true;*/		// 必须立即 flag , 玩家移动检测该值!!

	// 设置爆炸坐标
	//mBlast.blastx = ;
	//mBlast.blasty = ;
	//mBlast.canBlast = true;

	mBlast.SetBlasting(mTankX, mTankY);
}

// 玩家被击中爆炸
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
		// 检测是否可以重生
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

/*GameControl 内循环调用*/
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
		// 刚获得铲子道具
		if (mShovelProp_counter++ == 0)
		{
			ProtectCamp(_STONE);
		}
		else if (mShovelProp_counter > 31000 && mShovelProp_counter < 35400 )		// 显示一段时间后闪烁
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

/*GameControl 内循环调用*/
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
	return mScorePanel->show(image_hdc);// 整张画布缩放显示 image 到主窗口
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
	// 转换弹头坐标
	int hx = lx + BulletStruct::devto_head[dir][0];
	int hy = ty + BulletStruct::devto_head[dir][1];

	// 转换成 4*4 格子下标索引
	int b4i = hy / SMALL_BOX_SIZE;
	int b4j = hx / SMALL_BOX_SIZE;
	if (b4i > 51 || b4j > 51 || b4i < 0 || b4j < 0)
	{
		//printf("adad茶水间  %d, %d\n", lx, ty);
		return;
	}

	bms->bullet_4[b4i][b4j] = val;
}

//---------------------------------------------------------------- private function ---------
void PlayerBase::Reborn()
{
	/*m mDied = false;*/
	mTankX = (4 + 4 * player_id) * 16 + BOX_SIZE;				// 坦克首次出现时候的中心坐标
	mTankY = 12 * 16 + BOX_SIZE;
	SignBox_4(mTankX, mTankY, PLAYER_SIGN + player_id);		// 坦克出现, 将四角星标记改为坦克标记

	mPlayerTankLevel = 0;				// 坦克级别 [0-3]
	mTankTimer.SetDrtTime(mMoveSpeedDev[mPlayerTankLevel]);
	mBulletTimer.SetDrtTime(mBulletSpeedDev[mPlayerTankLevel]);

	mTankDir = DIR_UP;		// 坦克方向

	/*for (int i = 0; i < 2; i++)
	{
		mBulletStruct[i].x = SHOOTABLE_X;		// x 坐标用于判断是否可以发射
		mBulletStruct[i].y = -1000;
		mBulletStruct[i].dir = DIR_UP;
		mBulletStruct[i].mKillId = 0;			// 记录击中的敌机 id
	}*/

	mBullet_1_counter = 6;				// 子弹 1 运动 N 个循环后才可以发射子弹 2 
	mMoving = false;
	mRing.SetShowable(3222);			// 显示保护环
}

void PlayerBase::DispatchProp(int prop_kind)
{
	MciSound::_PlaySound(S_GETPROP);
	mProp.StopShowProp(true);

	switch (prop_kind)
	{
	case ADD_PROP:			// 加机
		MciSound::_PlaySound(S_ADDLIFE);
		mPlayerLife = mPlayerLife + 1 > 5 ? 5 : mPlayerLife + 1;
		break;
	case STAR_PROP:			// 五角星
		mPlayerTankLevel = mPlayerTankLevel + 1 > 3 ? 3 : mPlayerTankLevel + 1;
		mTankTimer.SetDrtTime(mMoveSpeedDev[mPlayerTankLevel]);
		mBulletTimer.SetDrtTime(mBulletSpeedDev[mPlayerTankLevel]);
		break;
	case TIME_PROP:			// 时钟
		mTimeProp = true;
		break;
	case  BOMB_PROP:		// 地雷
		mBombProp = true;
		break;
	case SHOVEL_PROP:		// 铲子
		mShovelProp = true;
		break;
	case  CAP_PROP:			// 帽子
		mRing.SetShowable(12000);
		break;
	default:
		break;
	}
}

/*
* 变向的同时调整坦克所在格子. 必须保证坦克中心在格子线上
 * 玩家移动计时器 mTankTimer 未到时	不能移动
 * 玩家生命值用光后					不能移动
 * 玩家坦克正在爆炸					不能移动
 * 玩家被另一个玩家击中后				不能移动

 * 移动前清除坦克 mTankX,mTankY 所在的 box_4 四个格子 = 空，表示该处没有东西存在
 * 如果是变向，那么调整 mTankX,mTankY 到正确的格子位置上
 * 检测是否可以移动
 * 如果可以移动，计算新的 mTankX, mTankY 坐标
 * 移动后标记 box_4 四个格子 = PLAYER_SIGN + player_id；表示该玩家坦克处于新位置

 * 新的坐标 mTankX, mTankY 已经更新完毕，函数返回，待循环体根据这个坐标重新绘制坦克，从而实现移动效果。
 */
void PlayerBase::Move(int new_dir)
{
	if (!mTankTimer.IsTimeOut() || mDied || mBlast.IsBlasting())
		return;

	// 如果玩家被另一个玩家击中暂停
	if (mPause)
	{
		return;
	}

	SignBox_4(mTankX, mTankY, _EMPTY);

	if (mTankDir != new_dir)
	{
		// 原左右变上下方向
		if (mTankDir == DIR_LEFT || mTankDir == DIR_RIGHT)
		{
			if (mTankX > (mTankX / BOX_SIZE) * BOX_SIZE + BOX_SIZE / 2 - 1)	// 如果是靠近格子线上的右边节点, -1是修正
				mTankX = (mTankX / BOX_SIZE + 1) * BOX_SIZE;
			else
				mTankX = (mTankX / BOX_SIZE) * BOX_SIZE;					// 靠近格子线上的左边节点
		}
		// 上下变左右
		else
		{
			if (mTankY > (mTankY / BOX_SIZE) * BOX_SIZE + BOX_SIZE / 2 - 1)	// 如果是靠近格子线上的下边节点, -1是修正
				mTankY = (mTankY / BOX_SIZE + 1) * BOX_SIZE;
			else
				mTankY = (mTankY / BOX_SIZE) * BOX_SIZE;					// 靠近格子线上的上边节点
		}

		// 更改方向, 必须先调正位置后才能设置方向!!
		mTankDir = new_dir;
	}
	else								// 移动
	{
		if (CheckMoveable())
		{
			mTankX += mDevXY[mTankDir][0];// *mSpeed[mPlayerTankLevel];
			mTankY += mDevXY[mTankDir][1];// *mSpeed[mPlayerTankLevel];
		}
	}
	SignBox_4(mTankX, mTankY, PLAYER_SIGN + player_id);
}

/* 判断当前方向可否移动
	box_8[i][j]
	i = y / BOX_SIZE		// y 坐标对应的是 i 值
	j = x / BOX_SIZE		// x 坐标对应的是 j 值
	---------
	| 1 | 2	|
	----a----
	| 3 | 4	|
	---------
* 如果 (x,y) 在 a 点, 转换后的 i,j 属于格子 4
* 如果 x 值在 a 点左边, 则转换后的 j 属于 1或3; 右边则属于 2或4
* 如果 y 值在 a 点以上, 则转换后的 i 属于 1或2; 以下则属于 3或4
** 如果 tempx,tempy 跨越了格子又遇到障碍, 那么就将 mTankX 或 mTankY 调整到格子线上,
*/
bool PlayerBase::CheckMoveable()
{
	// 坦克中心坐标
	int tempx = mTankX + mDevXY[mTankDir][0];// *mSpeed[mPlayerTankLevel];
	int tempy = mTankY + mDevXY[mTankDir][1];// *mSpeed[mPlayerTankLevel];

	// 游戏是运行在一个 208*208 的画布上的，所以实际游戏区域大小是 208*208
	// 如果新坐标 tempx < 8 即坦克中心坐标 < 8 ，说明此时已经越界（因为是中心点），就是说坦克已经移动到边界了
	if (tempx < BOX_SIZE || tempy < BOX_SIZE || tempy > CENTER_WIDTH - BOX_SIZE || tempx > CENTER_HEIGHT - BOX_SIZE)
	{
		// 如果遇到障碍物,将坦克坐标调整到格子线上. 不然坦克和障碍物会有几个像素点间隔
		switch (mTankDir)
		{
		case DIR_LEFT:	mTankX = (mTankX / BOX_SIZE) * BOX_SIZE;	break;	// mTankX 与 tempx 之间跨越了格子, 将坦克放到mTankX所在的格子线上
		case DIR_UP:	mTankY = (mTankY / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_RIGHT: mTankX = (tempx  / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_DOWN:	mTankY = (tempy  / BOX_SIZE) * BOX_SIZE;	break;
		default:													break;
		}
		return false;
	}
	// 转换像素点所在的 xy[26][26] 下标
	int index_i = tempy / BOX_SIZE;
	int index_j = tempx / BOX_SIZE;

	// 四个方向坦克中心点相对于要检测的两个 8*8 格子的下标偏移量
	int dev[4][2][2] = { {{-1,-1},{0,-1}},  {{-1,-1},{-1,0}},  {{-1,1},{0,1}}, { {1,-1},{1,0}} };

	// 8*8 障碍物格子检测
	int temp1 = bms->box_8[index_i + dev[mTankDir][0][0]][index_j + dev[mTankDir][0][1]];
	int temp2 = bms->box_8[index_i + dev[mTankDir][1][0]][index_j + dev[mTankDir][1][1]];

	// prop_8道具格子检测
	int curi = mTankY / BOX_SIZE;	// 当前坦克所在的坐标, 不是下一步的坐标, 用于判断道具
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

	// 检测坦克 4*4 格子
	// 四个方向需要检测的两个 4*4 的格子与坦克中心所在 4*4 格子的下标偏移量
	int  dev_4[4][4][2] = { {{-2,-2},{1,-2},{-1,-2},{0,-2}}, {{-2,-2},{-2,1},{-2,-1},{-2,0}},
							{{-2,2},{1,2},{-1,2},{0,2}}, {{2,-2},{2,1},{2,-1},{2,0}} };
	// 转换成 [52][52] 下标
	int index_4i = tempy / SMALL_BOX_SIZE;
	int index_4j = tempx / SMALL_BOX_SIZE;
	
	// -1, 0, 1, 2 都可以移动
	bool tank1 = bms->box_4[index_4i + dev_4[mTankDir][0][0]][index_4j + dev_4[mTankDir][0][1]] <= _ICE;
	bool tank2 = bms->box_4[index_4i + dev_4[mTankDir][1][0]][index_4j + dev_4[mTankDir][1][1]] <= _ICE;
	bool tank3 = bms->box_4[index_4i + dev_4[mTankDir][2][0]][index_4j + dev_4[mTankDir][2][1]] <= _ICE;
	bool tank4 = bms->box_4[index_4i + dev_4[mTankDir][3][0]][index_4j + dev_4[mTankDir][3][1]] <= _ICE;

	// 遇到障碍物调节坐标
	if (temp1 > 2 || temp2 > 2)
	{
		// 如果遇到障碍物,将坦克坐标调整到格子线上. 不然坦克和障碍物会有几个像素点间隔
		switch (mTankDir)
		{
		case DIR_LEFT:	mTankX = (mTankX / BOX_SIZE) * BOX_SIZE;	break;	// mTankX 与 tempx 之间跨越了格子, 将坦克放到mTankX所在的格子线上
		case DIR_UP:	mTankY = (mTankY / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_RIGHT: mTankX = (tempx / BOX_SIZE) * BOX_SIZE;		break;
		case DIR_DOWN:	mTankY = (tempy / BOX_SIZE) * BOX_SIZE;		break;
		default:													break;
		}
		return false;
	}
	// 遇到玩家不用调节
	else if (!tank1 || !tank2 || !tank3 || !tank4)
		return false;

	//. 如果是第一次进入 _ICE 上面
	if ( !mOnIce && (temp1 == _ICE || temp2 == _ICE))
		mOnIce = true;
	if (mOnIce && temp1 != _ICE && temp2 != _ICE)
		mOnIce = false;
	return true;
}

// 发射子弹
bool PlayerBase::ShootBullet( int bullet_id )
{
	switch (bullet_id)
	{
		case 0:
			// 1号子弹发射失败, 爆炸未完成前不能发射
			if (mBulletStruct[0].x != SHOOTABLE_X || mBombS[0].canBomb == true)		
				return false;

			// 子弹发射点坐标
			mBulletStruct[0].x = (int)(mTankX + BulletStruct::devto_tank[mTankDir][0]);
			mBulletStruct[0].y = (int)(mTankY + BulletStruct::devto_tank[mTankDir][1]);
			mBulletStruct[0].dir = mTankDir;
			mBullet_1_counter = 6;

			SignBullet(mBulletStruct[0].x, mBulletStruct[0].y, mBulletStruct[0].dir, P_B_SIGN + player_id * 10 + bullet_id );
			//_PlayerSound(NULL, L"shoot", L"shoot");
			return true;

		case 1:
			// 2 号子弹发射失败
			if (mPlayerTankLevel < 2 || mBulletStruct[1].x != SHOOTABLE_X || mBullet_1_counter > 0 || mBombS[1].canBomb == true)
				return false;

			// 子弹发射点坐标
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

	// 子弹头接触到障碍物的那个点, 左右方向点在上, 上下方向点在右
	int bombx = mBulletStruct[i].x + BulletStruct::devto_head[dir][0];
	int bomby = mBulletStruct[i].y + BulletStruct::devto_head[dir][1];

	bool flag = false;
	int adjust_x = 0, adjust_y = 0;		// 修正爆照图片显示的坐标

	// 不能用 bombx 代替 mBulletStruct[i].x,否则会覆盖障碍物的检测
	if (mBulletStruct[i].x < 0 && mBulletStruct[i].dir == DIR_LEFT)
	{
		flag = true;
		adjust_x = 5;					// 将爆炸图片向右移一点
	}
	else if (mBulletStruct[i].y < 0 && mBulletStruct[i].dir == DIR_UP)
	{
		flag = true;
		adjust_y = 5;
	}

	// 必须减去子弹的宽 4, 不然子弹越界, 后面检测导致 box_8 下标越界
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
		// 设定爆炸参数, 修正爆炸中心所在的格子,左右或上下偏移一个格子之类的..
		mBulletStruct[i].x = SHOOTABLE_X;
		mBombS[i].canBomb = true;
		mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
		mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
		mBombS[i].counter = 0;

		MciSound::_PlaySound(S_BIN);

		return BulletShootKind::Other;
	}

	int tempi, tempj;

	// 坐标所在 8*8 格子的索引
	int b8i = bomby / BOX_SIZE;
	int b8j = bombx / BOX_SIZE;

	// 将坐标转换成 4*4 格子索引
	int b4i = bomby / SMALL_BOX_SIZE;
	int b4j = bombx / SMALL_BOX_SIZE;

	//if (b4i > 51 || b4j > 51 || b4i < 0 || b4j < 0)
	//	printf("22222222\n");

	// 如果击中另外一个玩家子弹
	if (bms->bullet_4[b4i][b4j] == P_B_SIGN + (1 - player_id) * 10 + 0 || 
		bms->bullet_4[b4i][b4j] == P_B_SIGN + (1 - player_id) * 10 + 1 ||
		bms->bullet_4[b4i][b4j] == E_B_SIGN )
	{
		mBulletStruct[i].x = SHOOTABLE_X;
		bms->bullet_4[b4i][b4j] = WAIT_UNSIGN;		// 先标记中间值, 等待被击中的子弹擦除该标记
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
		// 自身格子和上一个
		int temp[2][2] = { {0, 0}, {-1, 0} };
		for (int n = 0; n < 2; n++)
		{
			// 8*8 格子, 判断是否击中敌机
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

			// 左右检测子弹头所在的4*4格子和它上面相邻的那个
			// 检测 4*4 格子, 由此判断障碍物
			tempi = b4i + temp[n][0];
			tempj = b4j + temp[n][1];
			if (bms->box_4[tempi][tempj] == _WALL || bms->box_4[tempi][tempj] == _STONE)
			{
				// 设定爆炸参数, 修正爆炸中心所在的格子,左右或上下偏移一个格子之类的..
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// 指示 i bomb 爆炸
				mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;
				ClearWallOrStone(i, bombx, bomby);
				return BulletShootKind::Other;
			}
			else if (bms->box_4[tempi][tempj] >= ENEMY_SIGN /*&& bms->box_4[tempi][tempj] < ENEMY_SIGN + TOTAL_ENEMY_NUMBER*/)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// 指示 i bomb 爆炸
				mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;

				// 标记击中了敌机的 id
				mBulletStruct[i].mKillId = bms->box_4[tempi][tempj];
				//mKillEnemyNumber[bms->box_4[tempi][tempj] % 10000 / 1000]++;	// 记录消灭敌机的级别种类的数量

				//	mProp->StartShowProp(100, 100);
				return BulletShootKind::Other;
			}
			else if (bms->box_4[tempi][tempj] == PLAYER_SIGN && player_id != 0 || bms->box_4[tempi][tempj] == PLAYER_SIGN + 1 && player_id != 1)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// 指示 i bomb 爆炸
				mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;
				return (BulletShootKind)bms->box_4[tempi][tempj];
			}
		}
	}
	break;

	// 上下只检测左右相邻的两个格子
	case DIR_UP:
	case DIR_DOWN:
	{
		// 自身格子和左边那一个格子
		int temp[2][2] = { { 0, 0 },{ 0, -1 } };
		for (int n = 0; n < 2; n++)
		{
			// 8*8 格子, 判断是否击中敌机
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

			// 检测 4*4 是否击中障碍
			tempi = b4i + temp[n][0];
			tempj = b4j + temp[n][1];
			if (bms->box_4[tempi][tempj] == _WALL || bms->box_4[tempi][tempj] == _STONE)
			{
				// 设定爆炸参数, 修正爆炸中心所在的格子,左右或上下偏移一个格子之类的..
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// 指示 i bomb 爆炸
				mBombS[i].mBombX =( bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY =( bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;
				ClearWallOrStone(i, bombx, bomby );
				return BulletShootKind::Other;
			}
			else if (bms->box_4[tempi][tempj] >= ENEMY_SIGN/* && bms->box_4[tempi][tempj] < ENEMY_SIGN + TOTAL_ENEMY_NUMBER*/)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// 指示 i bomb 爆炸
				mBombS[i].mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][0]) * SMALL_BOX_SIZE;
				mBombS[i].mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct[i].dir][1]) * SMALL_BOX_SIZE;
				mBombS[i].counter = 0;

				// 标记击中了敌机的 id
				mBulletStruct[i].mKillId = bms->box_4[tempi][tempj];
				//mKillEnemyNumber[bms->box_4[tempi][tempj] % 10000 / 1000]++;		// 记录消灭敌机的种类的数量
				//mProp->StartShowProp(100, 100);
				return BulletShootKind::Other;
			}
			else if (bms->box_4[tempi][tempj] == PLAYER_SIGN && player_id != 0 || bms->box_4[tempi][tempj] == PLAYER_SIGN + 1 && player_id != 1)
			{
				mBulletStruct[i].x = SHOOTABLE_X;
				mBombS[i].canBomb = true;				// 指示 i bomb 爆炸
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

// 子弹击中障碍物爆炸调用该函数, 击中边界不可调用, 下标会越界[52][52]
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
		bool bin_once = false;		// 多次循环中只播放一次声音

		// 在同一直线相邻的四个 4*4 格子, 顺序不能变, 后面用到下标判断
		int temp[4][2] = { { -2, 0 },{ -1, 0 },{ 0, 0 },{ 1, 0 } };
		for (int i = 0; i < 4; i++)
		{
			tempx = boxi + temp[i][0];
			tempy = boxj + temp[i][1];

			// 转到 tempx,tempy所在的 8*8 格子索引
			int n = tempx / 2;
			int m = tempy / 2;

			if (bms->box_4[tempx][tempy] == _WALL )
			{
				bms->box_4[tempx][tempy] = _CLEAR;

				// 检测 8*8 格子内的4个 4*4 的小格子是否全部被清除,
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
			else if (mPlayerTankLevel == 3 && bms->box_4[tempx][tempy] == _STONE && i % 2 == 0)	// %2==0 防止多余循环判断
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
		bool bin_once = false;		// 多次循环中只播放一次声音

		// 相邻的四个 4*4 格子, 顺序不能变, 后面用到下标判断
		int temp[4][2] = { {0, -2}, {0, -1}, {0, 0}, {0, 1} };
		for (int i = 0; i < 4; i++)
		{
			tempx = boxi + temp[i][0];
			tempy = boxj + temp[i][1];

			// 转到 tempx,tempy所在的 8*8 格子索引
			int n = tempx / 2;
			int m = tempy / 2;

			if (bms->box_4[tempx][tempy] == _WALL)
			{
				bms->box_4[tempx][tempy] = _CLEAR;

				// 检测 8*8 格子内的4个 4*4 的小格子是否全部被清除,
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
			else if (mPlayerTankLevel == 3 && bms->box_4[tempx][tempy] == _STONE && i % 2 == 0)	// %2==0 防止多余循环判断
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

			// 检测 8*8 格子内的4个 4*4 的小格子是否全部被清除,
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

// 参数是 16 * 16 中心点像素坐标, 与坦克中心坐标相同
void PlayerBase::SignBox_8(int x, int y, int val)
{
	// 右坦克中心索引转到左上角那个的 格子索引
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

// 根据坦克中心坐标, 标记16个 4*4 格子
void PlayerBase::SignBox_4(int cx, int cy, int val)
{
	// 左右调整 cs,cy 到占据百分比最多的 16 个 4*4 的格子中心
	if (mTankDir == DIR_LEFT || mTankDir == DIR_RIGHT)
	{
		if (cx > (cx / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2)	// 如果是靠近右边节点, 
		{
			cx = (cx / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		}
		else {
			cx = (cx / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;					// 靠近格子线上的左边节点
		}
	}
	// 上下
	else
	{
		if (cy > (cy / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2 )	// 如果是靠近格子下边节点,
			cy = (cy / SMALL_BOX_SIZE + 1) * SMALL_BOX_SIZE;
		else
			cy = (cy / SMALL_BOX_SIZE) * SMALL_BOX_SIZE;					// 靠近格子线上的上边节点
	}

	// 右坦克中心索引转到左上角那个的 格子索引
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

// 检测4个8*8 格子, 参数是左上角格子索引
bool PlayerBase::CheckBox_8(int iy, int jx)
{
	for (int i = iy; i < iy + 2; i++)
	{
		for (int j = jx; j < jx + 2; j++)
		{
			if (bms->box_8[i][j] < _RIVER)		// 如果有一个 8*8 坦克可以进来
				return true;
		}
	}
	return false;
}

/*获得铲子道具保护camp 内部使用*/
void PlayerBase::ProtectCamp(int val)
{
	for (int i = 23; i < 26; i++)
	{
		for (int j = 11; j < 15; j++)
		{
			if (i < 24 || j < 12 || j > 13)
			{
				bms->box_8[i][j] = val;			// 鸟巢周围是 _WALL
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



