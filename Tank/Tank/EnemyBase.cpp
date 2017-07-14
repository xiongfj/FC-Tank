#include "stdafx.h"
#include "EnemyBase.h"

/////////////////////////////////////////////////////////
// 敌机坦克控制

int EnemyBase::mDevXY[4][2] = { { -1, 0 },{ 0, -1 },{ 1, 0 },{ 0, 1 } };	// 依次左上右下
//bool EnemyBase::isOuting = false;
//bool EnemyBase::mIsPause = false;
//int EnemyBase::mPauseCounter = 0;

// 生成某个类别级别的敌机
EnemyBase::EnemyBase(TANK_KIND kind, byte level, BoxMarkStruct* b)
{
	mEnemyTankKind = kind;
	mEnemyTankLevel = level;
	mDied = false;
	mIsShootCamp = false;
	bms = b;

	int tempx[3] = {BOX_SIZE, 13 * BOX_SIZE, 25 * BOX_SIZE};	// 坦克随机出现的三个位置 x 坐标
	mTankX = tempx[rand() % 3];
	mTankY = BOX_SIZE;
	mTankDir = DIR_DOWN;
	mTankImageIndex = 0;

	mStep = rand() % 200;					// 当前方向随机移动的步数

	// 不同级别坦克移动速度系数
	int temp[4] = { 1, 1, 1, 1 };
	for (int i = 0; i < 4; i++)
		mSpeed[i] = temp[i];

	mTankNumberReduce = true;				// 坦克总数减一标志

	// 子弹初始化
	// .. 其它数据在 PlayerBase 加载了
	mBulletStruct.x = SHOOTABLE_X;
	mBulletStruct.y = -100;
	mBulletStruct.dir = DIR_DOWN;
	for (int i = 0; i < 4; i++)
		mBulletStruct.speed[i] = 3;		// 不能超过 4
	mBulletStruct.mKillId = 0;			// 记录击中玩家坦克的id
	
	//mBulletT1 = timeGetTime();
	//mBulletT2 = timeGetTime();
	//mBulletT = rand() % 111 + 320;		// 发射子弹的间隔时间 4000 

	// 爆炸图片
	mBombS.mBombX = -100;
	mBombS.mBombY = -100;
	mBombS.canBomb = false;
	mBombS.counter = 0;

	// 存储子弹击中玩家,玩家的id
	mShootedPlayerID = 0;

	// 敌机移动时间间隔
	mTankTimer.SetDrtTime(50);

	// 子弹移动时间间隔
	mBulletTimer.SetDrtTime(40);

	// 发射子弹频率
	mShootTimer.SetDrtTime(30);

	// 子弹爆炸速度
	mBombTimer.SetDrtTime(37);

	// 坦克爆炸速率
	mBlastTimer.SetDrtTime(37);
}

EnemyBase::~EnemyBase()
{
	//if (mEnemyTank != NULL)
		//delete mEnemyTank;
}

// 显示坦克
bool EnemyBase::ShowStar(const HDC& center_hdc, int& remainnumber)
{
	// 坦克已经出现,不用闪烁,直接返回
	if (mStar.mIsOuted == true)
		return STOP_SHOW_STAR;

	// 一段时间后才显示四角星, 之前留空
	if (mStar.mTankOutAfterCounter-- > 0)
		return SHOWING_STAR;

	// 如果坦克出现的位置被暂用了, 等待下一个随机循环在出现
	if (CheckBox_8() == false)
	{
		mStar.mTankOutAfterCounter = rand() % 100 + 10;
		return SHOWING_STAR;
	}

	// 四角星出现, 剩余坦克数-1;
	if (mTankNumberReduce)
	{
		mEnemyId = TOTAL_ENEMY_NUMBER - remainnumber;
		remainnumber -= 1;
		mTankNumberReduce = false;

		// 标记为 STAR_SIGN = 2000, 2000 属于坦克不能穿行的标志
		SignTank_8(mTankX, mTankY, STAR_SIGN);
	}

	// 开始闪烁四角星
	if (mStar.mStarCounter++ % 2 == 0)
	{
		if (mStar.mStarIndex + mStar.mStarIndexDev < 0)
		{
			mStar.mStarIndex = 1;
			mStar.mStarIndexDev = 1;
		}
		else if (mStar.mStarIndex + mStar.mStarIndexDev > 3)
		{
			mStar.mStarIndex = 2;
			mStar.mStarIndexDev = -1;
		}
		else
		{
			mStar.mStarIndex += mStar.mStarIndexDev;
		}
		if (mStar.mStarCounter == 35)
		{
			mStar.mIsOuted = true;						// 结束闪烁, TankMoving() 函数开始循环, 坦克开始移动
			//mShootTimer.Timer1();	//注释后坦克一出现一般都会立刻发射子弹??
			SignTank_8(mTankX, mTankY, ENEMY_SIGN + mEnemyId);		// 坦克出现, 将四角星标记改为坦克标记
			return STOP_SHOW_STAR;
		}
	}

	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&StarClass::mStarImage[mStar.mStarIndex]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000 );

	return SHOWING_STAR;
}

void EnemyBase::TankMoving(const HDC& center_hdc)
{
	if (!mStar.mIsOuted || mDied || mTankTimer.IsTimeOut() == false )
		return;
	
	//mBulletT2 = timeGetTime();

	// 移动前取消标记
	SignTank_8(mTankX, mTankY, _EMPTY);

	// 重定向
	if (mStep-- < 0)
		RejustDirPosition();
	
	// 可移动
	if (CheckMoveable())
	{
		//SignBox_8(0);
		mTankX += mDevXY[mTankDir][0] * mSpeed[mEnemyTankLevel];
		mTankY += mDevXY[mTankDir][1] * mSpeed[mEnemyTankLevel];
		//SignBox_8(ENEMY_SIGN + mEnemyId);
	}

	// 不可移动,重定向
	else
	{
		RejustDirPosition();
	}

	// 在新位置重新标记
	SignTank_8(mTankX, mTankY, ENEMY_SIGN + mEnemyId);
}
/*
void EnemyBase::DrawTank(const HDC& center_hdc)
{
	if (!mStar.mIsOuted || mDied)
		return;
	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&mEnemyTank->GetTankImage(mTankDir, mTankImageIndex++)), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	
}*/

// 
void EnemyBase::DrawBullet(const HDC& center_hdc)
{
	// 如果子弹没有移动或者敌机死亡
	if (mBulletStruct.x == SHOOTABLE_X || mDied)
		return;
	int dir = mBulletStruct.dir;

	TransparentBlt(center_hdc, mBulletStruct.x, mBulletStruct.y, BulletStruct::mBulletSize[dir][0],
		BulletStruct::mBulletSize[dir][1], GetImageHDC(&BulletStruct::mBulletImage[dir]),
		0, 0, BulletStruct::mBulletSize[dir][0], BulletStruct::mBulletSize[dir][1], 0x000000);
}

//
bool EnemyBase::ShootBullet()
{
	if (mBulletStruct.x != SHOOTABLE_X || !mShootTimer.IsTimeOut() || mDied || mStar.mIsOuted == false )
		return false;

	// 子弹发射点坐标
	mBulletStruct.x = mTankX + BulletStruct::devto_tank[mTankDir][0];
	mBulletStruct.y = mTankY + BulletStruct::devto_tank[mTankDir][1];
	mBulletStruct.dir = mTankDir;

	return true;
}

//
void EnemyBase::BulletMoving(const HDC& center_hdc)
{
	// 如果子弹没有移动或者敌机死亡
	if (mBulletStruct.x == SHOOTABLE_X || mDied || !mBulletTimer.IsTimeOut() )
		return;
	
	// 如果玩家吃到暂停道具
	//if (CheckPause())
	//	return;

	// 如果子弹在爆炸
	if (CheckBomb())
		return;

	int dir = mBulletStruct.dir;
	mBulletStruct.x += mDevXY[dir][0] * mBulletStruct.speed[mEnemyTankLevel];
	mBulletStruct.y += mDevXY[dir][1] * mBulletStruct.speed[mEnemyTankLevel];
	/*
	TransparentBlt(center_hdc, mBulletStruct.x, mBulletStruct.y, BulletStruct::mBulletSize[dir][0],
		BulletStruct::mBulletSize[dir][1], GetImageHDC(&BulletStruct::mBulletImage[dir]),
		0, 0, BulletStruct::mBulletSize[dir][0], BulletStruct::mBulletSize[dir][1], 0x000000);
		*/
}

//
void EnemyBase::Bombing(const HDC & center_hdc)
{
	int index[3] = { 0,1,2 };
	if (mBombS.canBomb)
	{
		TransparentBlt(center_hdc, mBombS.mBombX - BOX_SIZE, mBombS.mBombY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&BombStruct::mBombImage[index[mBombS.counter % 3]]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
		if (mBombTimer.IsTimeOut())
		{
			if ( mBombS.counter++ == 3 )
				mBombS.canBomb = false;
		}
	}
}

// 有Gamecontrol内检测, 然后调用
void EnemyBase::BeKill()
{
	// 如果敌机还没有出现
	if (mStar.mIsOuted == false)
		return;

	mDied = true;
	SignTank_8(mTankX, mTankY, _EMPTY);

	// 设置爆炸坐标
	mBlast.blastx = mTankX;
	mBlast.blasty = mTankY;
	mBlast.canBlast = true;
}

// 显示坦克爆炸效果, GameControl 内循环检测
bool EnemyBase::Blasting(const HDC& center_hdc)
{
	int index[6] = {0,1,2,3,4,2};
	if (mBlast.canBlast)
	{
		TransparentBlt(center_hdc, mBlast.blastx - BOX_SIZE * 2, mBlast.blasty - BOX_SIZE * 2, BOX_SIZE * 4, BOX_SIZE * 4,
			GetImageHDC(&BlastStruct::image[index[mBlast.counter % 6]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
		if (mBlastTimer.IsTimeOut())
		{
			if (mBlast.counter++ == 6)
			{
				mBlast.canBlast = false;
				return true;
			}
		}
	}
	return false;
}

bool EnemyBase::IsShootCamp()
{
	return mIsShootCamp;
}

// 返回被击中玩家 id 或者 0 没有击中
int EnemyBase::IsShootToPlayer()
{
	int temp = mShootedPlayerID;
	mShootedPlayerID = 0;			// 获取后归零! 不然一直被标记击中该玩家
	return temp;
}

int EnemyBase::GetId()
{
	return mEnemyId;
}

TANK_KIND EnemyBase::GetKind()
{
	return mEnemyTankKind;
}

//----------------- 私有函数 ------------------------
/*
void EnemyBase::SetPause(bool val)
{
	mIsPause = val;
}*/

// 标记或取消坦克所在的 4*4 = 16 个格子
void EnemyBase::SignBox_4(int value)
{
	// box_4[i][j] 对应索引
	int ix = mTankY / (BOX_SIZE / 2) - 2;		// -2 是从中心点右下第一个格子移到左上角那个格子
	int jy = mTankX / (BOX_SIZE / 2) - 2;
	for (int i = ix; i < ix + 4; i++)
	{
		for (int j = jy; j < jy + 4; j++)
		{
			bms->box_4[i][j] = value;
		}
	}
}

// x,y 是 16*16 中心点坐标
void EnemyBase::SignBox_8(int x, int y, int value)
{
	// 右坦克中心索引转到左上角那个的 格子索引
	int iy = y / BOX_SIZE - 1;
	int jx = x / BOX_SIZE - 1;
	for (int i = iy; i < iy + 2; i++)
	{
		for (int j = jx; j < jx + 2; j++)
		{
			bms->box_8[i][j] = value;
		}
	}
}

void EnemyBase::SignTank_8(int x, int y, int val)
{
	// 右坦克中心索引转到左上角那个的 格子索引
	int iy = y / BOX_SIZE - 1;
	int jx = x / BOX_SIZE - 1;
	for (int i = iy; i < iy + 2; i++)
	{
		for (int j = jx; j < jx + 2; j++)
		{
			bms->tank_8[i][j] = val;
		}
	}
}

// 检测某个16*16位置可以放坦克吗, x,y 16*16的中心点
bool EnemyBase::CheckBox_8()
{
	int iy = mTankY / BOX_SIZE - 1;
	int jx = mTankX / BOX_SIZE - 1;
	for (int i = iy; i < iy + 2; i++)
	{
		for (int j = jx; j < jx + 2; j++)
		{
			if (bms->box_8[i][j] != STAR_SIGN && bms->box_8[i][j] != _EMPTY ||  
				bms->tank_8[i][j] >= PLAYER_SIGN && bms->tank_8[i][j] <= ENEMY_SIGN + TOTAL_ENEMY_NUMBER )
				return false;
		}
	}
	return true;
}

//
bool EnemyBase::CheckMoveable()
{
	// 坦克中心坐标
	int tempx = mTankX + mDevXY[mTankDir][0] * mSpeed[mEnemyTankLevel];
	int tempy = mTankY + mDevXY[mTankDir][1] * mSpeed[mEnemyTankLevel];

	if (tempx < BOX_SIZE || tempy < BOX_SIZE || tempy > CENTER_WIDTH - BOX_SIZE || tempx > CENTER_HEIGHT - BOX_SIZE)
	{
		//SignBox_8(_EMPTY);
		// 如果遇到障碍物,将坦克坐标调整到格子线上. 不然坦克和障碍物会有几个像素点间隔
		switch (mTankDir)
		{
		case DIR_LEFT:	mTankX = (mTankX / BOX_SIZE) * BOX_SIZE;	break;	// mTankX 与 tempx 之间跨越了格子, 将坦克放到mTankX所在的格子线上
		case DIR_UP:	mTankY = (mTankY / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_RIGHT: mTankX = (tempx / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_DOWN:	mTankY = (tempy / BOX_SIZE) * BOX_SIZE;	break;
		default:														break;
		}
		//SignBox_8(ENEMY_SIGN + mEnemyId);
		return false;
	}
	// 转换像素点所在的 xy[26][26] 下标
	int index_i = (int)tempy / BOX_SIZE;
	int index_j = (int)tempx / BOX_SIZE;

	int dev[4][2][2] = { { { -1,-1 },{ 0,-1 } },{ { -1,-1 },{ -1,0 } },{ { -1,1 },{ 0,1 } },{ { 1,-1 },{ 1,0 } } };

	// 如果遇到障碍物或其它敌机,或者其它标志..
	if (bms->box_8[index_i + dev[mTankDir][0][0]][index_j + dev[mTankDir][0][1]] > 2 ||
		bms->tank_8[index_i + dev[mTankDir][1][0]][index_j + dev[mTankDir][0][1]] != ENEMY_SIGN + mEnemyId &&
		bms->tank_8[index_i + dev[mTankDir][1][0]][index_j + dev[mTankDir][0][1]] >= ENEMY_SIGN ||
		bms->box_8[index_i + dev[mTankDir][1][0]][index_j + dev[mTankDir][1][1]] > 2 ||
		bms->tank_8[index_i + dev[mTankDir][1][0]][index_j + dev[mTankDir][1][1]] != ENEMY_SIGN + mEnemyId &&
		bms->tank_8[index_i + dev[mTankDir][1][0]][index_j + dev[mTankDir][1][1]] >= ENEMY_SIGN)
	{
		//SignBox_8(_EMPTY);
		// 如果遇到障碍物,将坦克坐标调整到格子线上. 不然坦克和障碍物会有几个像素点间隔
		switch (mTankDir)
		{
		case DIR_LEFT:	mTankX = (mTankX / BOX_SIZE) * BOX_SIZE;	break;	// mTankX 与 tempx 之间跨越了格子, 将坦克放到mTankX所在的格子线上
		case DIR_UP:	mTankY = (mTankY / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_RIGHT: mTankX = (tempx  / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_DOWN:	mTankY = (tempy  / BOX_SIZE) * BOX_SIZE;	break;
		default:													break;
		}
		//SignBox_8(ENEMY_SIGN + mEnemyId);
		return false;
	}
	return true;
}

//
void EnemyBase::RejustDirPosition()
{
	mStep = rand() % 250;

	// 需要重新标记, 更正位置可能会改变所在的 4*4 格子
	//SignBox_8(0);

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

	// 重定向, 必须调正位置后才能设置方向
	mTankDir = rand() % 4;
	//SignBox_8(ENEMY_SIGN + mEnemyId);
}

//
bool EnemyBase::CheckBomb()
{
	int dir = mBulletStruct.dir;

	// 子弹头接触到障碍物的那个点, 左右方向点在上, 上下方向点在右
	int bombx = mBulletStruct.x + BulletStruct::devto_head[dir][0];
	int bomby = mBulletStruct.y + BulletStruct::devto_head[dir][1];

	bool flag = false;
	int adjust_x = 0, adjust_y = 0;		// 修正爆照图片显示的坐标

	// 不能用 bombx 代替 mBulletStruct[i].x,否则会覆盖障碍物的检测
	if (mBulletStruct.x < 0 && dir == DIR_LEFT)
	{
		flag = true;
		adjust_x = 5;					// 将爆炸图片向右移一点
	}
	else if (mBulletStruct.y < 0 && dir == DIR_UP)
	{
		flag = true;
		adjust_y = 5;
	}

	// 必须减去子弹的宽 4, 不然子弹越界, 后面检测导致 box_8 下标越界
	else if (mBulletStruct.x >= CENTER_WIDTH - 4 && dir == DIR_RIGHT)
	{
		flag = true;
		adjust_x = -4;
	}
	else if (mBulletStruct.y >= CENTER_HEIGHT - 4 && dir == DIR_DOWN)
	{
		flag = true;
		adjust_y = -4;
	}
	if (flag)
	{
		// 设定爆炸参数, 修正爆炸中心所在的格子,左右或上下偏移一个格子之类的..
		mBulletStruct.x = SHOOTABLE_X;
		mBombS.canBomb = true;
		mBombS.mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[dir][0]) * SMALL_BOX_SIZE;
		mBombS.mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[dir][1]) * SMALL_BOX_SIZE;
		mBombS.counter = 0;
		return true;
	}

	int tempi, tempj;

	// 4*4 格子索引
	int bi = bomby / SMALL_BOX_SIZE;
	int bj = bombx / SMALL_BOX_SIZE;

	// 坐标所在 8*8 格子的索引
	int b8i = bomby / BOX_SIZE;
	int b8j = bombx / BOX_SIZE;

	switch (dir)
	{
		// 左右检测子弹头所在的4*4格子和它上面相邻的那个
	case DIR_LEFT:
	case DIR_RIGHT:
	{
		int temp[2][2] = { { 0, 0 },{ -1, 0 } };
		for (int n = 0; n < 2; n++)
		{
			// 8*8 格子, 判断是否击中玩家
			tempi = b8i + temp[n][0];
			tempj = b8j + temp[n][1];
			if (bms->tank_8[tempi][tempj] == PLAYER_SIGN || bms->tank_8[tempi][tempj] == PLAYER_SIGN + 1)
			{
				mBulletStruct.x = SHOOTABLE_X;
				mBombS.canBomb = true;				// 指示 i bomb 爆炸
				mBombS.mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][0]) * SMALL_BOX_SIZE;
				mBombS.mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][1]) * SMALL_BOX_SIZE;
				mBombS.counter = 0;

				mShootedPlayerID = bms->tank_8[tempi][tempj];
				return true;
			}
			else if (bms->box_8[tempi][tempj] == CAMP_SIGN)
			{
				mBombS.counter = 0;
				mBulletStruct.x = SHOOTABLE_X;
				mIsShootCamp = true;
				SignBox_8(13 * BOX_SIZE, 25 * BOX_SIZE, _EMPTY);
				return true;
			}

			// 4*4 检测
			tempi = bi + temp[n][0];
			tempj = bj + temp[n][1];
			if (bms->box_4[tempi][tempj] == _WALL || bms->box_4[tempi][tempj] == _STONE )
			{
				// 设定爆炸参数, 修正爆炸中心所在的格子,左右或上下偏移一个格子之类的..
				mBulletStruct.x = SHOOTABLE_X;
				mBombS.canBomb = true;				// 指示 i bomb 爆炸
				mBombS.mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][0]) * SMALL_BOX_SIZE;
				mBombS.mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][1]) * SMALL_BOX_SIZE;
				mBombS.counter = 0;
				ShootWhat(bombx, bomby);
				return true;
			}
		}
	}
	break;

	// 上下只检测左右相邻的两个格子
	case DIR_UP:
	case DIR_DOWN:
	{
		int temp[2][2] = { { 0, 0 },{ 0, -1 } };
		for (int n = 0; n < 2; n++)
		{
			// 8*8 格子, 判断是否击中
			tempi = b8i + temp[n][0];
			tempj = b8j + temp[n][1];
			if (bms->tank_8[tempi][tempj] == PLAYER_SIGN || bms->tank_8[tempi][tempj] == PLAYER_SIGN + 1)
			{
				mBulletStruct.x = SHOOTABLE_X;
				mBombS.canBomb = true;				// 指示 i bomb 爆炸
				mBombS.mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][0]) * SMALL_BOX_SIZE;
				mBombS.mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][1]) * SMALL_BOX_SIZE;
				mBombS.counter = 0;

				mShootedPlayerID = bms->tank_8[tempi][tempj];
				return true;
			}
			else if (bms->box_8[tempi][tempj] == CAMP_SIGN)
			{
				mBombS.counter = 0;
				mBulletStruct.x = SHOOTABLE_X;
				mIsShootCamp = true;
				SignBox_8(13 * BOX_SIZE, 25 * BOX_SIZE, _EMPTY);
				return true;
			}

			// 4*4 检测
			tempi = bi + temp[n][0];
			tempj = bj + temp[n][1];
			if (bms->box_4[tempi][tempj] == _WALL || bms->box_4[tempi][tempj] == _STONE)
			{
				// 设定爆炸参数, 修正爆炸中心所在的格子,左右或上下偏移一个格子之类的..
				mBulletStruct.x = SHOOTABLE_X;
				mBombS.canBomb = true;				// 指示 i bomb 爆炸
				mBombS.mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][0]) * SMALL_BOX_SIZE;
				mBombS.mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][1]) * SMALL_BOX_SIZE;
				mBombS.counter = 0;
				ShootWhat(bombx, bomby);
				return true;
			}
		}
	}
	break;
	default:
		break;
	}
	return false;
}

void EnemyBase::ShootWhat(int bulletx, int bullety)
{
	int boxi = bullety / SMALL_BOX_SIZE;
	int boxj = bulletx / SMALL_BOX_SIZE;
	int tempx, tempy;
	switch (mBulletStruct.dir)
	{
	case DIR_LEFT:
	case DIR_RIGHT:
	{
		// 相邻的四个 4*4 格子, 顺序不能变, 后面用到下标判断
		int temp[4][2] = { { -2, 0 },{ -1, 0 },{ 0, 0 },{ 1, 0 } };
		for (int i = 0; i < 4; i++)
		{
			tempx = boxi + temp[i][0];
			tempy = boxj + temp[i][1];
			if (bms->box_4[tempx][tempy] == _WALL )
				bms->box_4[tempx][tempy] = _CLEAR;

			// 转到 tempx,tempy所在的 8*8 格子索引
			int n = tempx / 2;
			int m = tempy / 2;

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
	}
	break;

	case DIR_UP:
	case DIR_DOWN:
	{
		// 相邻的四个 4*4 格子, 顺序不能变, 后面用到下标判断
		int temp[4][2] = { { 0, -2 },{ 0, -1 },{ 0, 0 },{ 0, 1 } };
		for (int i = 0; i < 4; i++)
		{
			tempx = boxi + temp[i][0];
			tempy = boxj + temp[i][1];
			if (bms->box_4[tempx][tempy] == _WALL )
				bms->box_4[tempx][tempy] = _CLEAR;

			// 转到 tempx,tempy所在的 8*8 格子索引
			int n = tempx / 2;
			int m = tempy / 2;

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
	}
	break;

	default:
		break;
	}
}




CommonTank::CommonTank( byte level, BoxMarkStruct* bms) :
	EnemyBase(TANK_KIND::COMMON, level, bms)
{
	mTank = new TankInfo(GRAY_TANK, level, true);
}

// 
void CommonTank::DrawTank(const HDC& center_hdc)
{
	if (!mStar.mIsOuted || mDied)
		return;
	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&mTank->GetTankImage(mTankDir, mTankImageIndex++)), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
}

//////////////////////////////////////////////////////////////////////////////


PropTank::PropTank(byte level, BoxMarkStruct* bms) :
	EnemyBase(TANK_KIND::PROP, level, bms)
{
	mTank[0] = new TankInfo(GRAY_TANK, level, true);
	mTank[1] = new TankInfo(RED_TANK, level, true);
}

// 
void PropTank::DrawTank(const HDC& center_hdc)
{
	if (!mStar.mIsOuted || mDied)
		return;
	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&mTank[ index_counter++ % 2 ]->GetTankImage(mTankDir, mTankImageIndex++)), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
}

//////////////////////////////////////////////////////////////

BigestTank::BigestTank(TANK_KIND kind, BoxMarkStruct * bms):
	EnemyBase(kind, 3, bms)
{
	mTank[GRAY_TANK] = new TankInfo(GRAY_TANK, 3, true);
	mTank[YELLOW_TANK] = new TankInfo(YELLOW_TANK, 3, true);
	mTank[RED_TANK] = new TankInfo(RED_TANK, 3, true);
	mTank[GREEN_TANK] = new TankInfo(GREEN_TANK, 3, true);
}

void BigestTank::DrawTank(const HDC & center_hdc)
{
	if (!mStar.mIsOuted || mDied)
		return;

	// 道具坦克和普通坦克变色区别
	TankInfo* temp = NULL;
	switch (mEnemyTankKind)
	{
	case TANK_KIND::PROP:
		if (index_counter++ % 3)
			temp = mTank[RED_TANK];
		else
			temp = mTank[GRAY_TANK];
		break;
	case TANK_KIND::COMMON:
		if (index_counter++ % 3)
			temp = mTank[GREEN_TANK];
		else
			temp = mTank[GRAY_TANK];
		break;
	}

	if (temp == NULL)
	{
		printf("错误!. EnemyBase.cpp");
		return;
	}
	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&temp->GetTankImage(mTankDir, mTankImageIndex++)), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
}
