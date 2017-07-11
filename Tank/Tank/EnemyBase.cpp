#include "stdafx.h"
#include "EnemyBase.h"

/////////////////////////////////////////////////////////
// 敌机坦克控制

int EnemyBase::mDevXY[4][2] = { { -1, 0 },{ 0, -1 },{ 1, 0 },{ 0, 1 } };	// 依次左上右下
//bool EnemyBase::isOuting = false;

// 生成某个类别级别的敌机
EnemyBase::EnemyBase(byte kind, int level, BoxMarkStruct* b)
{
	mEnemyTankKind = kind;
	mEnemyTankLevel = level;
	mEnemyTank = new TankInfo(mEnemyTankKind, mEnemyTankLevel, true);
	bms = b;

	int tempx[3] = {BOX_SIZE, 13 * BOX_SIZE, 25 * BOX_SIZE};	// 坦克随机出现的三个位置 x 坐标
	mTankX = tempx[rand() % 3];
	mTankY = BOX_SIZE;
	mTankDir = DIR_DOWN;

	TCHAR buf[100];
	for (int i = 0; i < 4; i++)
	{
		_stprintf_s(buf, _T("./res/big/star%d.gif"), i);
		loadimage(&mStarImage[i], buf);
	}
	mStarIndexDev = -1;						// 控制索引下标的增加还是减少
	mStarIndex = 3;							// star 图片索引下标
	mStarCounter = 0;						// 多少次更换 star 图片
	mTankOutAfterCounter = rand() % 400 + 10;
	mIsOuted = false;						// 坦克是否已经出现

	mStep = rand() % 200;					// 当前方向随机移动的步数

	// 不同级别坦克移动速度系数
	int temp[4] = { 1, 1, 1, 1 };
	for (int i = 0; i < 4; i++)
		mSpeed[i] = temp[i];

	mTankNumberReduce = true;				// 坦克总数减一标志
}

EnemyBase::~EnemyBase()
{
	//if (mEnemyTank != NULL)
		//delete mEnemyTank;
}

// 显示坦克
bool EnemyBase::ShowStar(const HDC& center_hdc, int& total)
{
	// 坦克已经出现,不用闪烁,直接返回
	if (mIsOuted == true)
		return STOP_SHOW_STAR;

	// 一段时间后才显示四角星, 之前留空
	if (mTankOutAfterCounter-- > 0)
		return SHOWING_STAR;

	// 四角星出现, 剩余坦克数-1;
	if (mTankNumberReduce)
	{
		total -= 1;
		mTankNumberReduce = false;
	}

	// 开始闪烁四角星
	if (mStarCounter++ % 2 == 0)
	{
		if (mStarIndex + mStarIndexDev < 0)
		{
			mStarIndex = 1;
			mStarIndexDev = 1;
		}
		else if (mStarIndex + mStarIndexDev > 3)
		{
			mStarIndex = 2;
			mStarIndexDev = -1;
		}
		else
		{
			mStarIndex += mStarIndexDev;
		}
		if (mStarCounter == 35)
		{
			mIsOuted = true;				// 结束闪烁, TankMoving() 函数开始循环, 坦克开始移动
			return STOP_SHOW_STAR;
		}
	}

	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&mStarImage[mStarIndex]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000 );

	return SHOWING_STAR;
}

void EnemyBase::TankMoving(const HDC& center_hdc)
{
	if (!mIsOuted)
		return;

	// 重定向
	if (mStep-- < 0)
		RejustDirPosition();

	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&mEnemyTank->GetTankImage(mTankDir)), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	
	// 可移动
	if (CheckMoveable())
	{
		UnSignBox_4();
		mTankX += mDevXY[mTankDir][0] * mSpeed[mEnemyTankLevel];
		mTankY += mDevXY[mTankDir][1] * mSpeed[mEnemyTankLevel];
		SignBox_4(ENEMY_SIGN);
	}

	// 不可移动,重定向
	else
	{
		RejustDirPosition();
	}
}

//----------------- 私有函数 ------------------------

// 标记或取消坦克所在的 4*4 = 16 个格子
void EnemyBase::SignBox_4(int value)
{
	// box_4[i][j] 对应索引
	int ix = (int)(2 * mTankY) / BOX_SIZE - 2;		// -2 是从中心点右下第一个格子移到左上角那个格子
	int jy = (int)(2 * mTankX) / BOX_SIZE - 2;
	for (int i = ix; i < ix + 4; i++)
	{
		for (int j = jy; j < jy + 4; j++)
		{
			bms->box_4[i][j] = value;
		}
	}
}

// 取消标记
void EnemyBase::UnSignBox_4()
{
	int ix = 2 * (int)mTankY / BOX_SIZE - 2;
	int jy = 2 * (int)mTankX / BOX_SIZE - 2;
	for (int i = ix; i < ix + 4; i++)
	{
		for (int j = jy; j < jy + 4; j++)
		{
			bms->box_4[i][j] = 0;
		}
	}
}

//
bool EnemyBase::CheckSignBox(int x, int y)
{
	int ix = x / (BOX_SIZE / 2) - 2;
	int jy = y / (BOX_SIZE / 2) - 2;

	for (int i = ix; i < ix + 4; i++)
	{
		for (int j = jy; j < jy + 4; j++)
		{
			if (bms->box_4[i][j] != 0)
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
		// 如果遇到障碍物,将坦克坐标调整到格子线上. 不然坦克和障碍物会有几个像素点间隔
		switch (mTankDir)
		{
		case DIR_LEFT:	mTankX = (mTankX / BOX_SIZE) * BOX_SIZE;	break;	// mTankX 与 tempx 之间跨越了格子, 将坦克放到mTankX所在的格子线上
		case DIR_UP:	mTankY = (mTankY / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_RIGHT: mTankX = (tempx / BOX_SIZE) * BOX_SIZE;	break;
		case DIR_DOWN:	mTankY = (tempy / BOX_SIZE) * BOX_SIZE;	break;
		default:														break;
		}
		return false;
	}
	// 转换像素点所在的 xy[26][26] 下标
	int index_i = (int)tempy / BOX_SIZE;
	int index_j = (int)tempx / BOX_SIZE;

	int dev[4][2][2] = { { { -1,-1 },{ 0,-1 } },{ { -1,-1 },{ -1,0 } },{ { -1,1 },{ 0,1 } },{ { 1,-1 },{ 1,0 } } };

	if (bms->box_8[index_i + dev[mTankDir][0][0]][index_j + dev[mTankDir][0][1]] > 2 ||
		bms->box_8[index_i + dev[mTankDir][1][0]][index_j + dev[mTankDir][1][1]] > 2)
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
	return true;
}

void EnemyBase::RejustDirPosition()
{
	mStep = rand() % 50;

	// 需要重新标记, 更正位置可能会改变所在的 4*4 格子
	UnSignBox_4();

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
	SignBox_4(ENEMY_SIGN);
}
