#include "stdafx.h"
#include "GameControl.h"
#include "typeinfo.h"

int GameControl::mCurrentStage = 1;	// [1-35]
GameControl::GameControl( HDC des_hdc, HDC image_hdc/*, BoxMarkStruct* bms*/)
{
	mDes_hdc = des_hdc;
	mImage_hdc = image_hdc;
	mCenterImage.Resize( CENTER_WIDTH, CENTER_HEIGHT );
	mCenter_hdc = GetImageHDC(&mCenterImage);
	mBoxMarkStruct = new BoxMarkStruct();

	loadimage( &mBlackBackgroundImage, _T("./res/big/bg_black.gif"		));		// 黑色背景
	loadimage( &mGrayBackgroundImage , _T("./res/big/bg_gray.gif"		));		// 灰色背景
	loadimage( &mStoneImage			 , _T("./res/big/stone.gif"			));		// 12*12的石头
	loadimage( &mForestImage		 , _T("./res/big/forest.gif"		));		// 树林
	loadimage( &mIceImage			 , _T("./res/big/ice.gif"			));		// 冰块
	loadimage( &mRiverImage[0]		 , _T("./res/big/river-0.gif"		));		// 河流
	loadimage( &mRiverImage[1]		 , _T("./res/big/river-1.gif"		));		//
	loadimage( &mWallImage			 , _T("./res/big/wall.gif"			));		// 泥墙
	loadimage( &mCamp[0]			 , _T("./res/big/camp0.gif"			));		// 大本营
	loadimage( &mCamp[1]			 , _T("./res/big/camp1.gif"			));		// 
	loadimage( &mEnemyTankIcoImage	 , _T("./res/big/enemytank-ico.gif"	));		// 敌机图标
	loadimage( &mFlagImage			 , _T("./res/big/flag.gif"			));		// 旗子
	loadimage( &mCurrentStageImage	 , _T("./res/big/stage.gif"			));
	loadimage( &mBlackNumberImage	 , _T("./res/big/black-number.gif"	));		// 0123456789 黑色数字
	loadimage( &mGameOverImage		 , _T("./res/big/gameover.gif"		));

	// 自定义绘制地图
	loadimage(&mCreateMapTankImage, _T("./res/big/0Player/m0-1-2.gif"));

	Init();
}

GameControl::~GameControl()
{
}

void GameControl::Init()
{
	mOutedEnemyTankNumber = 0;													// 已经出现在地图上的敌机数量,最多显示6架
	mRemainEnemyTankNumber = 20;												// 剩余未出现的敌机数量
	mKillEnemyNum = 0;
	mCampDie = false;															// 标志大本营是否被击中

	mEnemyPause = false;			// 敌机暂停与否
	mEnemyPauseCounter = 0;			// 敌机暂停多久

	mTimer.SetDrtTime(14);
	mCampTimer.SetDrtTime(23);

	// GameOver 图片
	mGameOverX = -100;
	mGameOverY = -100;
	mGameOverFlag = false;
	mGameOverTimer.SetDrtTime(10);

	// 绘制地图坦克游标的坐标
	mCMTImageX = BOX_SIZE;
	mCMTImageY = BOX_SIZE;

	// 每次进入地图制作之前都检测之前是否有制作地图
	mHasCustomMap = false;

	// 关卡结束显示分数面板
	mShowScorePanel = false;

	// 胜利
	mWin = false;
	mWinCounter = 0;
}

// 存储玩家进链表
void GameControl::AddPlayer(int player_num)
{
	for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
	{
		delete *itor;
	}

	// 清空原来数据
	PlayerList.clear();

	for (int i = 0; i < player_num; i++)
		//PlayerList.Add(new PlayerBase(i, mBoxMarkStruct/*, &mProp*/) );
		PlayerList.push_back(new PlayerBase(i, mBoxMarkStruct));
}

/*
* 读取data数据绘制地图,
* 显示敌机数量\玩家生命\关卡\等信息
*/
void GameControl::LoadMap()
{
	// 读取地图文件数据
	FILE* fp = NULL;
	if (0 != fopen_s(&fp, "./res/data/map.txt", "rb"))
		throw _T("读取地图数据文件异常.");
	fseek(fp, sizeof(Map) * (mCurrentStage - 1), SEEK_SET);
	fread(&mMap, sizeof(Map), 1, fp);
	fclose(fp);

	InitSignBox();
}

// 玩家自己创建地图
bool GameControl::CreateMap()
{
	int i, j, x = 0, y = 0;
	int tempx, tempy;
	bool flag = true;
	int keys[4] = {VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN};		// 下标必须与 DIR_LEFT 等对应
	int dev[4][2] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1} };	// 游标坦克移动分量
	int twinkle_counter = 0;	// 坦克游标闪烁计数
	int lastx = mCMTImageX, lasty = mCMTImageY;			// 记录坦克上次的坐标, 如果坦克移动不会变换 sign_order 图形
	bool M_down = false;		// 检测 M 是否按下, 一直按下 M 不会切换地图, 只按下的那一次切换

	// 14 中情况
	int sign_order[14][4] = { {_ICE, _ICE, _ICE, _ICE},		// 四个格子都是冰, 依次左上右上左下 ..
		{ _FOREST,	_FOREST,	_FOREST,	_FOREST},
		{ _RIVER,	_RIVER,		_RIVER,		_RIVER },
		{ _STONE,	_STONE ,	_STONE ,	_STONE },
		{ _STONE ,	_STONE ,	_EMPTY,		_EMPTY},
		{ _STONE ,	_EMPTY,		_STONE ,	_EMPTY},
		{ _EMPTY,	_EMPTY,		_STONE ,	_STONE },
		{ _EMPTY,	_STONE ,	_EMPTY,		_STONE },
		{ _WALL,	_WALL ,		_WALL ,		_WALL },
		{ _WALL ,	_WALL ,		_EMPTY,		_EMPTY},
		{ _WALL ,	_EMPTY,		_WALL,		_EMPTY},
		{ _EMPTY,	_EMPTY,		_WALL ,		_WALL },
		{ _EMPTY,	_WALL ,		_EMPTY,		_WALL },
		{ _EMPTY,	_EMPTY ,	_EMPTY ,	_EMPTY } };
	
	int cur_index = 13;		// 对应上面数组

	// 清除或保留上次绘制的地图
	if ( !mHasCustomMap )
		ClearSignBox();

	// 按键速度
	TimeClock click;
	click.SetDrtTime(90);

	// 灰色背景
	StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);

	while ( flag )
	{
		Sleep(4);

		if (click.IsTimeOut())
		{
			for (i = 0; i < 4; i++)
			{
				if (GetAsyncKeyState(keys[i]) & 0x8000)
				{
					lastx = mCMTImageX;
					lasty = mCMTImageY;
					tempx = mCMTImageX + dev[i][0] * BOX_SIZE * 2;
					tempy = mCMTImageY + dev[i][1] * BOX_SIZE * 2;
					if (tempx >= BOX_SIZE && tempx <= BOX_SIZE * 25 && tempy >= BOX_SIZE && tempy <= BOX_SIZE * 25)
					{
						mCMTImageX += dev[i][0] * BOX_SIZE * 2;
						mCMTImageY += dev[i][1] * BOX_SIZE * 2;
					}
				}
			}

			// J,K 键一个顺序,一个逆序
			SHORT J_KEY = GetAsyncKeyState('J') & 0x8000;
			SHORT K_KEY = GetAsyncKeyState('K') & 0x8000;

			// 放置障碍物
			if (J_KEY || K_KEY)
			{
				if (mCMTImageX == lastx && mCMTImageY == lasty)
				{
					if (J_KEY)
						cur_index = cur_index + 1 > 13 ? 0 : cur_index + 1;
					else if (K_KEY)
						cur_index = cur_index - 1 < 0 ? 13 : cur_index - 1;
				}
				else
				{
					lastx = mCMTImageX;
					lasty = mCMTImageY;
				}

				// 更改 16*16 的地图
				i = mCMTImageY / BOX_SIZE - 1;
				j = mCMTImageX / BOX_SIZE - 1;
				mBoxMarkStruct->box_8[i][j] = sign_order[cur_index][0];
				mBoxMarkStruct->box_8[i][j + 1] = sign_order[cur_index][1];
				mBoxMarkStruct->box_8[i + 1][j] = sign_order[cur_index][2];
				mBoxMarkStruct->box_8[i + 1][j + 1] = sign_order[cur_index][3];
			}

			if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			{
				// 标记 8*8 格子内部的 4*4 格子
				for (i = 0; i < 26; i++)
				{
					for (j = 0; j < 26; j++)
					{
						if (mBoxMarkStruct->box_8[i][j] != _EMPTY)
							SignBox_4(i, j, mBoxMarkStruct->box_8[i][j]);
					}
				}
				break;
			}

			if (GetAsyncKeyState(27) & 0x8000)
				break;

			/*M键功能: 不会连续更换地图if (GetAsyncKeyState('M') & 0x8000 && M_down == false)
			{
				M_down = true;
				if (mCMTImageX == lastx && mCMTImageY == lasty)
				{
					cur_index = cur_index + 1 > 13 ? 0 : cur_index + 1;
				}
				else
				{
					lastx = mCMTImageX;
					lasty = mCMTImageY;
				}

				// 更改 16*16 的地图
				i = mCMTImageY / BOX_SIZE - 1;
				j = mCMTImageX / BOX_SIZE - 1;
				mBoxMarkStruct->box_8[i][j] = sign_order[cur_index][0];
				mBoxMarkStruct->box_8[i][j + 1] = sign_order[cur_index][1];
				mBoxMarkStruct->box_8[i + 1][j] = sign_order[cur_index][2];
				mBoxMarkStruct->box_8[i + 1][j + 1] = sign_order[cur_index][3];
			}else if ( !GetAsyncKeyState('M') & 0x8000 )
				M_down = false;*/
		}

		// 黑色背景
		StretchBlt(mCenter_hdc, 0, 0, CENTER_WIDTH, CENTER_HEIGHT, GetImageHDC(&mBlackBackgroundImage), 0, 0, CENTER_WIDTH, CENTER_HEIGHT, SRCCOPY);
		
		for (int i = 0; i < 26; i++)
		{
			for (int j = 0; j < 26; j++)
			{
				x = j * BOX_SIZE;// +CENTER_X;
				y = i * BOX_SIZE;// +CENTER_Y;
				switch (mBoxMarkStruct->box_8[i][j])
				{
				case _WALL:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mWallImage), 0, 0, SRCCOPY);
					break;
				case _FOREST:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mForestImage), 0, 0, SRCCOPY);
					break;
				case _ICE:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mIceImage), 0, 0, SRCCOPY);
					break;
				case _RIVER:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mRiverImage[0]), 0, 0, SRCCOPY);
					break;
				case _STONE:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mStoneImage), 0, 0, SRCCOPY);
					break;
				default:
					break;
				}
			}
		}


		// 大本营
		TransparentBlt(mCenter_hdc, BOX_SIZE * 12, BOX_SIZE * 24, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&mCamp[0]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

		// 坦克游标
		if(twinkle_counter++ / 28 % 2 == 0)
			TransparentBlt(mCenter_hdc, mCMTImageX - BOX_SIZE, mCMTImageY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&mCreateMapTankImage), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

		// 将中心画布印到主画布 mImage_hdc 上
		BitBlt(mImage_hdc, CENTER_X, CENTER_Y, CENTER_WIDTH, CENTER_HEIGHT, mCenter_hdc, 0, 0, SRCCOPY);
		// 整张画布缩放显示 image 到主窗口
		StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();
	}
	mHasCustomMap = true;
	return true;
}

void GameControl::GameLoop()
{
	ShowStage();
	GameResult result = GameResult::Victory;

	while (result != GameResult::Fail)
	{
		result = StartGame();
		Sleep(1);
	}
}

//
GameResult GameControl::StartGame()
{
	// 主绘图操作时间
	if (mTimer.IsTimeOut())
	{
		// 胜利或者失败 显示分数面板
		if (mShowScorePanel)
		{
			BitBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&ScorePanel::background), 0, 0, SRCCOPY);
			for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
			{
				if (!(*itor)->ShowScorePanel(mImage_hdc))
				{
					if (mWin)
					{
						Init();

						for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
							(*itor)->Init();
						EnemyList.clear();

						mCurrentStage++;
						LoadMap();
						ShowStage();
					}
					else
					{
						return GameResult::Fail;
					}
					break;
				}
			}

			// 整张画布缩放显示 image 到主窗口
			StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
			FlushBatchDraw();
			return GameResult::Victory;
		}

		AddEnemy();

		// 更新右边面板的数据, 待判断, 因为不需要经常更新 mImage_hdc
		RefreshRightPanel();

		// 更新中心游戏区域: mCenter_hdc
		RefreshCenterPanel();

		// 将中心画布印到主画布 mImage_hdc 上
		BitBlt( mImage_hdc, CENTER_X, CENTER_Y, CENTER_WIDTH, CENTER_HEIGHT, mCenter_hdc, 0, 0, SRCCOPY );
		// 整张画布缩放显示 image 到主窗口
		StretchBlt( mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY );
		FlushBatchDraw();
	}

	// 数据变化, 不能涉及绘图操作
	RefreshData();

	return GameResult::Victory;
}

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////// 私有函数,本类使用 //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void GameControl::ShowStage()
{
	// 灰色背景
	//mGraphics->DrawImage(mGrayBackgroundImage, 0, 0, CANVAS_WIDTH + 10, CANVAS_HEIGHT + 10);	// +10 去掉由于拉伸的边缘变色
	StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT,
		GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);

	TransparentBlt(mImage_hdc, 97, 103, 39, 7, GetImageHDC(&mCurrentStageImage), 0, 0, 39, 7, 0xffffff);

	// [1-9] 关卡，单个数字
	if (mCurrentStage < 10)
		TransparentBlt(mImage_hdc, 157, 103, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE,
			GetImageHDC(&mBlackNumberImage), BLACK_NUMBER_SIZE * mCurrentStage, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
	else	// 10,11,12 .. 双位数关卡
	{
		TransparentBlt(mImage_hdc, 157, 103, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE,
			GetImageHDC(&mBlackNumberImage), BLACK_NUMBER_SIZE * (mCurrentStage / 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);

		TransparentBlt(mImage_hdc, 157, 103, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE,
			GetImageHDC(&mBlackNumberImage), BLACK_NUMBER_SIZE * (mCurrentStage % 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
	}
	StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
	FlushBatchDraw();

	Sleep(300);
}

//
void GameControl::ClearSignBox()
{
	// 初始化标记各种格子
	int x = 0, y = 0;
	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			mBoxMarkStruct->prop_8[i][j] = _EMPTY;
			mBoxMarkStruct->box_8[i][j] = _EMPTY;	// 26*26
			SignBox_4(i, j, _EMPTY);		// 标记 26*26 和 52*52 格子
		}
	}

	// 标记大本营
	for (int i = 23; i < 26; i++)
	{
		for (int j = 11; j < 15; j++)
		{
			if (i >= 24 && j >= 12 && j <= 13)
				mBoxMarkStruct->box_8[i][j] = CAMP_SIGN;
			else
				mBoxMarkStruct->box_8[i][j] = _WALL;			// 鸟巢周围是 _WALL
		}
	}
}

//
void GameControl::InitSignBox()
{
	// 初始化标记各种格子
	int x = 0, y = 0;
	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			mBoxMarkStruct->prop_8[i][j] = _EMPTY;
			mBoxMarkStruct->box_8[i][j] = mMap.buf[i][j] - '0';	// 26*26
			SignBox_4(i, j, mMap.buf[i][j] - '0');		// 标记 26*26 和 52*52 格子
		}
	}

	// 标记大本营
	for (int i = 23; i < 26; i++)
	{
		for (int j = 11; j < 15; j++)
		{
			if (i >= 24 && j >= 12 && j <= 13)
				mBoxMarkStruct->box_8[i][j] = CAMP_SIGN;
			else
				mBoxMarkStruct->box_8[i][j] = _WALL;			// 鸟巢周围是 _WALL
		}
	}
}

// 待修改, 添加的敌机种类需要修改
void GameControl::AddEnemy()
{
	if (EnemyList.size() >= 6 || TOTAL_ENEMY_NUMBER - mOutedEnemyTankNumber <= 0)
		return;

	int level;
	TANK_KIND kind;
	if (mOutedEnemyTankNumber < 8)
		level = 0;
	else if (mOutedEnemyTankNumber < 13)
		level = 1;
	else if (mOutedEnemyTankNumber < 17)
		level = 2;
	else
		level = 3;

	// 每个5架除一架道具坦克
	if (mOutedEnemyTankNumber % 5 == 4)
		kind = TANK_KIND::PROP;
	else
		kind = TANK_KIND::COMMON;

	switch (level)
	{
	case 0:
	case 1:
	case 2:
		mOutedEnemyTankNumber++;
		switch (kind)
		{
			case 0: EnemyList.push_back((new PropTank(level, mBoxMarkStruct))); break;
			case 1: EnemyList.push_back((new CommonTank(level, mBoxMarkStruct))); break;
			default: break;
		}
		break;

	case 3:
		EnemyList.push_back(new BigestTank((TANK_KIND)kind, mBoxMarkStruct));
		mOutedEnemyTankNumber++;
		break;
	default:
		break;
	}
}

// 提供8*8 的索引, 标记里面4个 4*4 的格子
void GameControl::SignBox_4(int i, int j, int sign_val)
{
	int temp_i[4] = { 2 * i, 2 * i + 1, 2 * i, 2 * i + 1 };
	int temp_j[4] = { 2 * j, 2 * j, 2 * j + 1, 2 * j + 1 };

	for ( int i = 0; i < 4; i++ )
		mBoxMarkStruct->box_4[ temp_i[i] ][ temp_j[i] ] = sign_val;
}

// 数据更新, 不涉及绘图操作!!
bool GameControl::RefreshData()
{
	if (GetAsyncKeyState(27) & 0x8000)
		return false;

	// 检测玩家是否获得 '时钟' 静止道具
	if (PlayerBase::IsGetTimeProp())
	{
		mEnemyPause = true;
		mEnemyPauseCounter = 0;
		EnemyBase::SetPause(true);
	}

	// 玩家获得地雷道具
	for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
	{
		if ((*itor)->IsGetBombProp())
		{
			for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
			{
				if ((*EnemyItor)->BeKill(true))
				{
					mKillEnemyNum++;

					//printf("%d \n", mKillEnemyNum);

					// 玩家记录消灭的敌机数量
					(*itor)->AddKillEnemyNum((*EnemyItor)->GetLevel());

					if (mKillEnemyNum == 20)
					{
						mWinCounter = 0;
						mWin = true;
					}
				}
			}
		}
	}

	// 玩家, 不能包含绘图操作! 内含计时器
	for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
	{
		// 鸟巢被消灭玩家停止移动
		if (!mGameOverFlag)
			(*itor)->PlayerControl();

		// 玩家子弹击中结果
		BulletShootKind kind = (*itor)->BulletMoving(mCenter_hdc);
		switch (kind)
		{
		case BulletShootKind::Camp:
			mGameOverX = CENTER_WIDTH / 2 - GAMEOVER_WIDTH / 2;
			mGameOverY = CENTER_HEIGHT;
			mGameOverFlag = true;
			break;

		// 遍历被击中的玩家 然后暂停它
		case BulletShootKind::Player_1:
			for (list<PlayerBase*>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
			{
				if ((*i)->GetID() == 0)
					(*i)->SetPause();
			}
			break;

		// 遍历被击中的玩家 然后暂停它
		case BulletShootKind::Player_2:
			for (list<PlayerBase*>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
			{
				if ((*i)->GetID() == 1)
					(*i)->SetPause();
			}
			break;
		default:
			break;
		}
	}

	// 敌机, 此处不能包含计绘图操作, 内含计时器, 不然那会导致计时器与主计时器不一致,导致失帧
	for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
	{
		(*EnemyItor)->ShootBullet();

		int result = (*EnemyItor)->BulletMoving();
		switch (result)
		{
		case BulletShootKind::Player_1:
		case BulletShootKind::Player_2:

			for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
			{
				if ((*itor)->GetID() + PLAYER_SIGN == result)
				{
					(*itor)->BeKill();
					break;
				}
			}
			break;
		case BulletShootKind::Camp:
			mGameOverX = CENTER_WIDTH / 2 - GAMEOVER_WIDTH / 2;
			mGameOverY = CENTER_HEIGHT;
			mGameOverFlag = true; 
			break;

		default:
			break;
		}

		// 如果敌机暂停
		if (mEnemyPause == false)
		{
			(*EnemyItor)->TankMoving(mCenter_hdc);
		}
		else if (mEnemyPauseCounter++ > 14300)
		{
			mEnemyPause = false;
			mEnemyPauseCounter = 0;;
			(*EnemyItor)->SetPause(false);
		}
	}

	return true;
}

void GameControl::RefreshRightPanel()
{
	// 灰色背景
	StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);

	// 显示敌机数量图标
	int x[2] = {233, 241};
	int n, index;
	for ( int i = 0; i < mRemainEnemyTankNumber; i++ )
	{
		n = i / 2;
		index = i % 2;

		TransparentBlt( mImage_hdc, x[index], 19 + n * 8, ENEMY_TANK_ICO_SIZE, ENEMY_TANK_ICO_SIZE, 
			GetImageHDC(&mEnemyTankIcoImage), 0, 0, ENEMY_TANK_ICO_SIZE, ENEMY_TANK_ICO_SIZE, 0xffffff );	// 注意这个图标有黑色部分
	}
	
	// 玩家1P\2P\坦克图标\生命数
	/*for (ListNode<PlayerBase*>* p = PlayerList.First(); p != NULL; p = p->pnext)
	{
		p->data->DrawPlayerTankIco(mImage_hdc);
	}*/
	for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
	{
		(*itor)->DrawPlayerTankIco(mImage_hdc);
	}

	// 旗子
	TransparentBlt(mImage_hdc, 232, 177, FLAG_ICO_SIZE_X, FLAG_ICO_SIZE_Y,
		GetImageHDC(&mFlagImage), 0, 0, FLAG_ICO_SIZE_X, FLAG_ICO_SIZE_Y, 0xffffff);	// 注意图标内有黑色部分

	// 关卡
	if (mCurrentStage < 10)
		TransparentBlt(mImage_hdc, 238, 193, 7, 7, GetImageHDC(&mBlackNumberImage),
			BLACK_NUMBER_SIZE * mCurrentStage, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
	else	// 10,11,12 .. 双位数关卡
	{
		TransparentBlt(mImage_hdc, 233, 193, 7, 7, GetImageHDC(&mBlackNumberImage),
			BLACK_NUMBER_SIZE * (mCurrentStage / 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
		TransparentBlt(mImage_hdc, 241, 193, 7, 7, GetImageHDC(&mBlackNumberImage),
			BLACK_NUMBER_SIZE * (mCurrentStage % 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
	}
}

// 更新中间游戏区域
void GameControl::RefreshCenterPanel()
{
		BitBlt(mCenter_hdc, 0, 0, CENTER_WIDTH, CENTER_HEIGHT, GetImageHDC(&mBlackBackgroundImage), 0, 0, SRCCOPY);// 中心黑色背景游戏区

	   // 四角星闪烁控制
		for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
		{
			// 一个四角星动画结束后再执行下一个
			if ((*EnemyItor)->ShowStar(mCenter_hdc, mRemainEnemyTankNumber) == SHOWING_STAR)
			{
				break;
			}
		}

		/* 开始根据数据文件绘制地图
		* 划分为 BOX_SIZE x BOX_SIZE 的格子
		* x坐标： j*BOX_SIZE
		* y坐标： i*BOX_SIZE
		*/
		int x = 0, y = 0;
		for (int i = 0; i < 26; i++)
		{
			for (int j = 0; j < 26; j++)
			{
				x = j * BOX_SIZE;// +CENTER_X;
				y = i * BOX_SIZE;// +CENTER_Y;
				switch (mBoxMarkStruct->box_8[i][j])
				{
				case _WALL:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mWallImage), 0, 0, SRCCOPY);
					break;
				case _ICE:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mIceImage), 0, 0, SRCCOPY);
					break;
				case _RIVER:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mRiverImage[0]), 0, 0, SRCCOPY);
					break;
				case _STONE:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mStoneImage), 0, 0, SRCCOPY);
					break;
				default:
					break;
				}
			}
		}

		// 检测被销毁的障碍物, 绘制黑色图片擦除
		for (int i = 0; i < 52; i++)
		{
			for (int j = 0; j < 52; j++)
			{
				if (mBoxMarkStruct->box_4[i][j] == _CLEAR)
				{
					BitBlt(mCenter_hdc, j * SMALL_BOX_SIZE, i * SMALL_BOX_SIZE, SMALL_BOX_SIZE, SMALL_BOX_SIZE,
						GetImageHDC(&mBlackBackgroundImage), 0, 0, SRCCOPY);
				}
			}
		}

		for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
		{
			(*itor) ->ShowStar(mCenter_hdc);
			(*itor)->DrawPlayerTank(mCenter_hdc);		// 坦克
			(*itor)->DrawBullet(mCenter_hdc);
			CheckKillEnemy(*itor);
			if ((*itor)->IsShootCamp())
			{
				if (mBlast.canBlast == false)
				{
					int index[17] = { 0,0,0,1,1,2,2,3,3,4,4,4,4,3,2,1,0 };
					TransparentBlt(mCenter_hdc, 11 * BOX_SIZE, 23 * BOX_SIZE, BOX_SIZE * 4, BOX_SIZE * 4,
						GetImageHDC(&BlastStruct::image[index[mBlast.counter % 17]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
					if (mCampTimer.IsTimeOut() && mBlast.counter++ == 17)
						mBlast.canBlast = true;
					mCampDie = true;
				}
			}
		}

		// 敌机
		for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
		{
			(*EnemyItor)->DrawTank(mCenter_hdc);
			(*EnemyItor)->DrawBullet(mCenter_hdc);
		}

		// 森林
		for (int i = 0; i < 26; i++)
		{
			for (int j = 0; j < 26; j++)
			{
				x = j * BOX_SIZE;// +CENTER_X;
				y = i * BOX_SIZE;// +CENTER_Y;
				if (mBoxMarkStruct->box_8[i][j] == _FOREST)
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mForestImage), 0, 0, SRCCOPY);
			}
		}

		// 敌机子弹\坦克爆炸图, 不能重合
		for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
		{
			(*EnemyItor)->Bombing(mCenter_hdc);

			// 爆炸完毕, 移除敌机
			if ((*EnemyItor)->Blasting(mCenter_hdc))
			{
				delete *EnemyItor;
				EnemyList.erase(EnemyItor);
				break;
			}

			// 如果该敌机击中大本营
			if (/*(*EnemyItor)->IsShootCamp()*/mGameOverFlag)
			{
				if (mBlast.canBlast == false)
				{
					int index[17] = { 0,0,0,1,1,2,2,3,3,4,4,4,4,3,2,1,0 };
					TransparentBlt(mCenter_hdc, 11 * BOX_SIZE, 23 * BOX_SIZE, BOX_SIZE * 4, BOX_SIZE * 4,
						GetImageHDC(&BlastStruct::image[index[mBlast.counter % 17]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
					if ( mCampTimer.IsTimeOut() && mBlast.counter++ == 17)
						mBlast.canBlast = true;
					mCampDie = true;
				}
			}
		}

		// 玩家子弹
		/*for (ListNode<PlayerBase*>* p = PlayerList.First(); p != NULL; p = p->pnext)
		{
			p->data->Bombing(mCenter_hdc);

			// 爆炸完成后
			if (p->data->Blasting(mCenter_hdc))
			{
			}
		}*/
		for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
		{
			(*itor)->Bombing(mCenter_hdc);
			if ((*itor)->Blasting(mCenter_hdc))
			{

			}
		}

		// 道具闪烁, 内部自定义时钟
		PlayerBase::ShowProp(mCenter_hdc);

		// 大本营
		if (!mCampDie)		// 如果没爆炸
		{
			TransparentBlt(mCenter_hdc, BOX_SIZE * 12, BOX_SIZE * 24, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&mCamp[0]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
		}
		else if (mBlast.canBlast)	// 如果爆炸完毕, 显示被摧毁的camp
		{
			TransparentBlt(mCenter_hdc, BOX_SIZE * 12, BOX_SIZE * 24, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&mCamp[1]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
		}

		IsWinOver();
		IsGameOver();
}

// 读取PlayerBase 内的数据, 消灭敌机
void GameControl::CheckKillEnemy(PlayerBase* pb)
{
	int bullet[2] = {0, 0};
	pb->GetKillEnemy(bullet[0], bullet[1]);		// 获取玩家击中的敌机id, 存储进 bullet[2] 内

	for (int i = 0; i < 2; i++)
	{
		if (bullet[i] >= ENEMY_SIGN )
		{
			for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
			{
				if ((*EnemyItor)->GetId() == bullet[i] % 100)		// 100xx 后两位是 id
				{
					// 如果消灭敌机
					if ((*EnemyItor)->BeKill(false))
					{
						mKillEnemyNum++;
						//printf("%d---\n", mKillEnemyNum);
						if ((int)TANK_KIND::PROP == bullet[i] % 1000 / 100)		// 获取百分位的敌机种类
							PlayerBase::SetShowProp();

						// 玩家记录消灭的敌机数量
						pb->AddKillEnemyNum((*EnemyItor)->GetLevel());
					}
					if (mKillEnemyNum == 20)
					{
						mWinCounter = 0;
						mWin = true;
					}
					break;
				}
			}
		}
	}
}
void GameControl::IsGameOver()
{
	if (!mGameOverFlag)
		return;

	TransparentBlt(mCenter_hdc, mGameOverX, mGameOverY, GAMEOVER_WIDTH, GAMEOVER_HEIGHT,
		GetImageHDC(&mGameOverImage), 0, 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, 0x000000);

	if (mGameOverTimer.IsTimeOut() && mGameOverY >= CENTER_HEIGHT * 0.45)
		mGameOverY -= 2;
	else if (mGameOverY <= CENTER_HEIGHT * 0.45)
	{
		//mGameOverFlag = false;
		mShowScorePanel = true;
		mWin = false;
		for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
		{
			(*itor)->SendKillNumToScorePanel();
		}
	}
}

// 如果敌机都被消灭, 隔 mWinCounter 后跳转到分数面板
void GameControl::IsWinOver()
{
	if (mWin && mWinCounter++ > 210 && !mGameOverFlag)
	{
		mShowScorePanel = true;
		for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
		{
			(*itor)->SendKillNumToScorePanel();
		}
	}
}
/*.
void GameControl::CheckKillPlayer(list<EnemyBase*>::iterator enemyItor)
{
	int id = (*enemyItor)->IsShootToPlayer();
	if (id == -1)
		return;

	for (ListNode<PlayerBase*>* p = PlayerList.First(); p != NULL; p = p->pnext)
	{
		if (p->data->GetID() + PLAYER_SIGN == id)
		{
			//printf("ads121 -   %d\n", id);
			p->data->BeKill();
			break;
		}
	}
}*/
