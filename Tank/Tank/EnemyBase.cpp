#include "stdafx.h"
#include "EnemyBase.h"
#include "MciSound.h"

/////////////////////////////////////////////////////////
// 敌机坦克控制

bool EnemyBase::mPause = false;
int EnemyBase::mDevXY[4][2] = { { -1, 0 },{ 0, -1 },{ 1, 0 },{ 0, 1 } };	// 依次左上右下

// 生成某个类别级别的敌机
EnemyBase::EnemyBase(TANK_KIND kind, byte level, BoxMarkStruct* b)
{
	mEnemyTankKind = kind;
	mEnemyTankLevel = level;
	mDied = false;
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

	//mTankNumberReduce = true;				// 坦克总数减一标志

	// 子弹初始化
	// .. 其它数据在 PlayerBase 加载了
	mBulletStruct.x = SHOOTABLE_X;
	mBulletStruct.y = -100;
	mBulletStruct.dir = DIR_DOWN;
	for (int i = 0; i < 4; i++)
		mBulletStruct.speed[i] = 3;		// 不能超过 4
	mBulletStruct.mKillId = 0;			// 记录击中玩家坦克的id

	mShootCounter = rand() % 100 + 100;	// 随机间隔发射子弹
	
	// 爆炸图片
	mBombS.mBombX = -100;
	mBombS.mBombY = -100;
	mBombS.canBomb = false;
	mBombS.counter = 0;

	// 不同级别敌机移动时间间隔
	int movelevel[4] = { 30, 15, 27, 25};
	mTankTimer.SetDrtTime(movelevel[mEnemyTankLevel]);

	// 子弹移动时间间隔
	mBulletTimer.SetDrtTime(30);

	// 发射子弹频率
	mShootTimer.SetDrtTime( rand() % 1000 + 700 );

	// 子弹爆炸速度
	mBombTimer.SetDrtTime(37);

	// 坦克爆炸速率
	//mBlastTimer.SetDrtTime(37);

	// 设置回头射击频率
	mShootBackTimer.SetDrtTime( rand()%5000 + 9000 );

	loadimage(&mScoreImage[0], _T("./res/big/100.gif"));
	loadimage(&mScoreImage[1], _T("./res/big/200.gif"));
	loadimage(&mScoreImage[2], _T("./res/big/300.gif"));
	loadimage(&mScoreImage[3], _T("./res/big/400.gif"));
}

EnemyBase::~EnemyBase()
{
	//if (mEnemyTank != NULL)
		//delete mEnemyTank;
}

void EnemyBase::Init()
{
}

// 显示坦克
Star_State EnemyBase::ShowStar(const HDC& center_hdc, int& remainnumber)
{
	// 坦克已经出现,不用闪烁,直接返回
	/*if (mStar.IsStop() == true)
		return Star_State::Tank_Out;

	// 一段时间后才显示四角星, 之前留空
	if (mStar.mTankOutAfterCounter-- > 0)
		return Star_State::Star_Showing;

	// 如果坦克出现的位置被暂用了, 等待下一个随机循环在出现
	if (CheckBox_8() == false)
	{
		// 重新选个随机位置
		int tempx[3] = { BOX_SIZE, 13 * BOX_SIZE, 25 * BOX_SIZE };
		mTankX = tempx[rand() % 3];

		mStar.mTankOutAfterCounter = rand() % 100 + 10;
		return Star_State::Star_Showing;
	}*/


	Star_State result = mStar.EnemyShowStar(center_hdc, mTankX, mTankY, bms);
	switch (result)
	{
		// 当前正在计时, 未显示
		case Star_State::Star_Timing:
			break;

		// 当前位置显示四角星失败
		case Star_State::Star_Failed:
			{
				// 重新选个随机位置
				int tempx[3] = { BOX_SIZE, 13 * BOX_SIZE, 25 * BOX_SIZE };
				mTankX = tempx[rand() % 3];
			}
			break;

		// 四角星开始出现
		case Star_State::Star_Out:
			SignBox_4(mTankX, mTankY, STAR_SIGN);	// 标记为 STAR_SIGN = 2000, 2000 属于坦克不能穿行的标志
			break;

		// 四角星正在出现
		case Star_State::Star_Showing:
			break;

		// 四角星停止
		case Star_State::Star_Stop:
			// 四角星消失. 敌机出现, 剩余坦克数-1;
			mEnemyId = TOTAL_ENEMY_NUMBER - remainnumber;
			remainnumber -= 1;

			SignBox_4(mTankX, mTankY, ENEMY_SIGN + 1000 * mEnemyTankLevel + 100 * mEnemyTankKind + mEnemyId);		// 坦克出现, 将四角星标记改为坦克标记
			break;

		// 坦克已经出现
		case Star_State::Tank_Out:
			break;
	}
	return result;
}

void EnemyBase::TankMoving(const HDC& center_hdc)
{
	if (!mStar.IsStop() || mDied || mTankTimer.IsTimeOut() == false )
		return;
	
	// 移动前取消标记
	SignBox_4(mTankX, mTankY, _EMPTY);

	// 内部计时, 一定时差后回头射击
	ShootBack();

	// 重定向
	if (mStep-- < 0)
		RejustDirPosition();
	
	// 可移动
	if (CheckMoveable())
	{
		mTankX += mDevXY[mTankDir][0] * mSpeed[mEnemyTankLevel];
		mTankY += mDevXY[mTankDir][1] * mSpeed[mEnemyTankLevel];
	}

	// 不可移动,重定向
	else
	{
		RejustDirPosition();
	}

	// 在新位置重新标记
	SignBox_4(mTankX, mTankY, ENEMY_SIGN + mEnemyTankLevel * 1000 + mEnemyTankKind * 100 + mEnemyId);
}

// 
void EnemyBase::DrawBullet(const HDC& center_hdc)
{
	// 如果子弹没有移动或者敌机死亡
	if (mBulletStruct.x == SHOOTABLE_X /*|| mDied*/)
		return;
	int dir = mBulletStruct.dir;

	TransparentBlt(center_hdc, mBulletStruct.x, mBulletStruct.y, BulletStruct::mBulletSize[dir][0],
		BulletStruct::mBulletSize[dir][1], GetImageHDC(&BulletStruct::mBulletImage[dir]),
		0, 0, BulletStruct::mBulletSize[dir][0], BulletStruct::mBulletSize[dir][1], 0x000000);
}

//
bool EnemyBase::ShootBullet()
{
	if ( mPause || mBulletStruct.x != SHOOTABLE_X || mShootTimer.IsTimeOut() == false || mDied || mStar.IsStop() == false )
		return false;

	// 子弹发射点坐标
	mBulletStruct.x = mTankX + BulletStruct::devto_tank[mTankDir][0];
	mBulletStruct.y = mTankY + BulletStruct::devto_tank[mTankDir][1];
	mBulletStruct.dir = mTankDir;

	SignBullet(mBulletStruct.x, mBulletStruct.y, mBulletStruct.dir, E_B_SIGN);
	return true;
}

//
BulletShootKind EnemyBase::BulletMoving()
{
	// 如果子弹没有移动或者敌机死亡
	if (mBulletStruct.x == SHOOTABLE_X/* || mDied*/ || !mBulletTimer.IsTimeOut() )
		return BulletShootKind::None;
	
	// 如果子弹在爆炸
	BulletShootKind result = CheckBomb();
	switch (result)
	{
	case BulletShootKind::Camp:
	case BulletShootKind::Other:
	case BulletShootKind::Player_1:
	case BulletShootKind::Player_2:
		return result;

	case BulletShootKind::None:
		break;
	default:
		break;
	}

	int dir = mBulletStruct.dir;
	SignBullet(mBulletStruct.x, mBulletStruct.y, dir, _EMPTY );

	mBulletStruct.x += mDevXY[dir][0] * mBulletStruct.speed[mEnemyTankLevel];
	mBulletStruct.y += mDevXY[dir][1] * mBulletStruct.speed[mEnemyTankLevel];

	SignBullet(mBulletStruct.x, mBulletStruct.y, dir, E_B_SIGN );
	return BulletShootKind::None;
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
			if (mBombS.counter++ >= 3)
			{
				mBombS.counter = 0;
				mBombS.canBomb = false;
			}
		}
	}
}

/* 有Gamecontrol内检测, 然后调用
* BigestTank.class 需要覆盖这个方法, 
* 因为它需要射击四次才能杀死
*/
bool EnemyBase::BeKill(bool killanyway)
{
	// 如果敌机还没有出现
	if (mStar.IsStop() == false || mBlast.IsBlasting() || mDied == true)
		return false;

	MciSound::_PlaySound(S_ENEMY_BOMB);
	mDied = true;
	SignBox_4(mTankX, mTankY, _EMPTY);

	// 设置开始爆炸参数
	mBlast.SetBlasting(mTankX, mTankY);

	return true;
}

// 显示坦克爆炸效果, GameControl 内循环检测
bool EnemyBase::Blasting(const HDC& center_hdc)
{
	switch (mBlast.EnemyBlasting(center_hdc, &mScoreImage[mEnemyTankLevel]))
	{
	case BlastState::NotBlast:
		break;

	case BlastState::Blasting:
		break;

	case BlastState::BlastEnd:
		return true;

	default:
		break;
	}
	return false;
}

//.bool EnemyBase::IsShootCamp()
//.{
	//.return mIsShootCamp;
//.}

//
void EnemyBase::SetPause(bool val)
{
	mPause = val;
}
/*.
// 返回被击中玩家 id 或者 0 没有击中
int EnemyBase::IsShootToPlayer()
{
	int temp = mShootedPlayerID;
	mShootedPlayerID = -1;			// 获取后归NONE! 不然一直被标记击中该玩家, 不能与玩家0,1 数值相同!!
	return temp;
}
*/
int EnemyBase::GetId()
{
	return mEnemyId;
}

TANK_KIND EnemyBase::GetKind()
{
	return mEnemyTankKind;
}

byte EnemyBase::GetLevel()
{
	return mEnemyTankLevel;
}

//----------------- 私有函数 ------------------------
/*
void EnemyBase::SetPause(bool val)
{
	mIsPause = val;
}*/

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

// 标记或取消坦克所在的 4*4 = 16 个格子
void EnemyBase::SignBox_4(int cx, int cy, int val)
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
		if (cy > (cy / SMALL_BOX_SIZE) * SMALL_BOX_SIZE + SMALL_BOX_SIZE / 2)	// 如果是靠近格子下边节点,
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

void EnemyBase::SignBullet(int lx, int ty, byte dir, int val)
{
	// 转换弹头坐标
	int hx = lx + BulletStruct::devto_head[dir][0];
	int hy = ty + BulletStruct::devto_head[dir][1];

	// 转换成 4*4 格子下标索引
	int b4i = hy / SMALL_BOX_SIZE;
	int b4j = hx / SMALL_BOX_SIZE;
	if (b4i > 51 || b4j > 51 || b4i < 0 || b4j < 0)
	{
		//printf("adad茶水间%d, %d\n", b4i, b4j);
		return;
	}

	bms->bullet_4[b4i][b4j] = val;
}

// 检测某个16*16位置可以放坦克吗, x,y 16*16的中心点
bool EnemyBase::CheckBox_8()
{
	// 获取坦克左上角的 4*4 下标 
	int iy = mTankY / SMALL_BOX_SIZE - 2;
	int jx = mTankX / SMALL_BOX_SIZE - 2;
	for (int i = iy; i < iy + 4; i++)
	{
		for (int j = jx; j < jx + 4; j++)
		{
			// 检测四角星, 玩家,敌机,
			if (bms->box_4[i][j] != STAR_SIGN && bms->box_4[i][j] > _FOREST)
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

	// 检测 8*8 障碍物
	int dev[4][2][2] = { { { -1,-1 },{ 0,-1 } },{ { -1,-1 },{ -1,0 } },{ { -1,1 },{ 0,1 } },{ { 1,-1 },{ 1,0 } } };

	// 检测坦克 4*4 格子
	// 四个方向需要检测的两个 4*4 的格子与坦克中心所在 4*4 格子的下标偏移量
	int  dev_4[4][4][2] = { { { -2,-2 },{ 1,-2 },{ -1,-2 },{ 0,-2 } },{ { -2,-2 },{ -2,1 },{ -2,-1 },{ -2,0 } },
							{ { -2, 2 },{ 1, 2 },{ -1, 2 },{ 0, 2 } },{ { 2, -2 },{ 2, 1 },{ 2, -1 },{ 2, 0 } } };

	// 转换成 [52][52] 下标
	int index_4i = tempy / SMALL_BOX_SIZE;
	int index_4j = tempx / SMALL_BOX_SIZE;

	// -1, 0, 1, 2 都可以移动
	bool tank1 = bms->box_4[index_4i + dev_4[mTankDir][0][0]][index_4j + dev_4[mTankDir][0][1]] <= _ICE;
	bool tank2 = bms->box_4[index_4i + dev_4[mTankDir][1][0]][index_4j + dev_4[mTankDir][1][1]] <= _ICE;
	bool tank3 = bms->box_4[index_4i + dev_4[mTankDir][2][0]][index_4j + dev_4[mTankDir][2][1]] <= _ICE;
	bool tank4 = bms->box_4[index_4i + dev_4[mTankDir][3][0]][index_4j + dev_4[mTankDir][3][1]] <= _ICE;

	// 遇到障碍物
	if (bms->box_8 [index_i + dev[mTankDir][0][0]][index_j + dev[mTankDir][0][1]] > 2 ||
		bms->box_8 [index_i + dev[mTankDir][1][0]][index_j + dev[mTankDir][1][1]] > 2 )
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
	// 遇到坦克不用调节
	else if (!tank1 || !tank2 || !tank3 || !tank4)
		return false;
	return true;
}

//
void EnemyBase::RejustDirPosition()
{
	mStep = rand() % 250;

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

	/* 重定向, 必须调正位置后才能设置方向
	* 设置坦克向下移动的几率大些*/
	if (mTankDir == DIR_LEFT || mTankDir == DIR_RIGHT)
	{
		bool val = rand() % 100 < 70;
		if (val)
			mTankDir = DIR_DOWN;
		else
			mTankDir = rand() % 4;
	}
	else
		mTankDir = rand() % 4;
}

//
BulletShootKind EnemyBase::CheckBomb()
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
		return BulletShootKind::Other;
	}

	int tempi, tempj;

	// 坐标所在 8*8 格子的索引
	int b8i = bomby / BOX_SIZE;
	int b8j = bombx / BOX_SIZE;

	// 4*4 格子索引
	int bi = bomby / SMALL_BOX_SIZE;
	int bj = bombx / SMALL_BOX_SIZE;

	//if ( bi>51 || bj > 51 || bi < 0 || bj < 0)
	//printf("%d, %d\n", bi, bj);

	// 如果击中玩家子弹
	if (bms->bullet_4[bi][bj] == P_B_SIGN + 0 * 10 + 0 ||
		bms->bullet_4[bi][bj] == P_B_SIGN + 0 * 10 + 1 ||
		bms->bullet_4[bi][bj] == P_B_SIGN + 1 * 10 + 0 ||
		bms->bullet_4[bi][bj] == P_B_SIGN + 1 * 10 + 1 )
	{
		mBulletStruct.x = SHOOTABLE_X;
		bms->bullet_4[bi][bj] = WAIT_UNSIGN;		// 先标记中间值, 等待被击中的子弹检测到该值后,再擦除该标记
		return BulletShootKind::Other;
	}
	else if (bms->bullet_4[bi][bj] == WAIT_UNSIGN)
	{
		mBulletStruct.x = SHOOTABLE_X;
		bms->bullet_4[bi][bj] = _EMPTY;
		return BulletShootKind::Other;
	}

	switch (dir)
	{
	// 左右检测子弹头所在的4*4格子和它上面相邻的那个
	case DIR_LEFT:
	case DIR_RIGHT:
	{
		int temp[2][2] = { { 0, 0 },{ -1, 0 } };
		for (int n = 0; n < 2; n++)
		{
			// 8*8 格子, 判断是否击鸟巢
			tempi = b8i + temp[n][0];
			tempj = b8j + temp[n][1];
			if (bms->box_8[tempi][tempj] == CAMP_SIGN)
			{
				mBombS.counter = 0;
				mBulletStruct.x = SHOOTABLE_X;
				//.mIsShootCamp = true;
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

			// 4*4 检测
			tempi = bi + temp[n][0];
			tempj = bj + temp[n][1];
			if (bms->box_4[tempi][tempj] == _WALL || bms->box_4[tempi][tempj] == _STONE )
			{
				// 设定爆炸参数, 修正爆炸中心所在的格子,
				mBulletStruct.x = SHOOTABLE_X;
				mBombS.canBomb = true;				// 指示 i bomb 爆炸
				mBombS.mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][0]) * SMALL_BOX_SIZE;
				mBombS.mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][1]) * SMALL_BOX_SIZE;
				mBombS.counter = 0;
				ShootWhat(bombx, bomby);
				return BulletShootKind::Other;
			}
			// 4*4 玩家格子
			else if (bms->box_4[tempi][tempj] == PLAYER_SIGN || bms->box_4[tempi][tempj] == PLAYER_SIGN + 1)
			{
				mBulletStruct.x = SHOOTABLE_X;
				mBombS.canBomb = true;				// 指示 i bomb 爆炸
				mBombS.mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][0]) * SMALL_BOX_SIZE;
				mBombS.mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][1]) * SMALL_BOX_SIZE;
				mBombS.counter = 0;
				return (BulletShootKind)bms->box_4[tempi][tempj];
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
			// 8*8 格子, 判断是否击中鸟巢
			tempi = b8i + temp[n][0];
			tempj = b8j + temp[n][1];
			if (bms->box_8[tempi][tempj] == CAMP_SIGN)
			{
				mBombS.counter = 0;
				mBulletStruct.x = SHOOTABLE_X;
				//.mIsShootCamp = true;
				SignBox_8(13 * BOX_SIZE, 25 * BOX_SIZE, _EMPTY);
				return BulletShootKind::Camp;
			}

			// 4*4 检测
			tempi = bi + temp[n][0];
			tempj = bj + temp[n][1];
			if (bms->box_4[tempi][tempj] == _WALL || bms->box_4[tempi][tempj] == _STONE)
			{
				// 设定爆炸参数, 修正爆炸中心所在的格子
				mBulletStruct.x = SHOOTABLE_X;
				mBombS.canBomb = true;				// 指示 i bomb 爆炸
				mBombS.mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][0]) * SMALL_BOX_SIZE;
				mBombS.mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][1]) * SMALL_BOX_SIZE;
				mBombS.counter = 0;
				ShootWhat(bombx, bomby);
				return BulletShootKind::Other;
			}
			// 4*4 玩家小格子
			else if (bms->box_4[tempi][tempj] == PLAYER_SIGN || bms->box_4[tempi][tempj] == PLAYER_SIGN + 1)
			{
				mBulletStruct.x = SHOOTABLE_X;
				mBombS.canBomb = true;				// 指示 i bomb 爆炸
				mBombS.mBombX = (bombx / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][0]) * SMALL_BOX_SIZE;
				mBombS.mBombY = (bomby / SMALL_BOX_SIZE + BulletStruct::bomb_center_dev[mBulletStruct.dir][1]) * SMALL_BOX_SIZE;
				mBombS.counter = 0;
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

			// 4 个 4*4 组成的 8*8 格子被清除完
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

/*
* 设置敌机回头运动较小的一段距离
*/
void EnemyBase::ShootBack()
{
	if (!mShootBackTimer.IsTimeOut())
		return;

	int back_dir[4] = {DIR_RIGHT, DIR_DOWN, DIR_LEFT, DIR_UP};

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

	mStep = rand() % 30 + 30;
	mTankDir = back_dir[mTankDir];
}

////////////////////////////////////////////////////////

CommonTank::CommonTank( byte level, BoxMarkStruct* bms) :
	EnemyBase(TANK_KIND::COMMON, level, bms)
{
	mTank = new TankInfo(GRAY_TANK, level, true);
}

CommonTank::~CommonTank()
{
	delete mTank;
	printf("~CommonTank::CommonTank() .. \n");
}

// 
void CommonTank::DrawTank(const HDC& center_hdc)
{
	if (!mStar.mIsOuted || mDied)
		return;
	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&mTank->GetTankImage(mTankDir, mTankImageIndex++ / 3 % 2)), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
}

//////////////////////////////////////////////////////////////////////////////


PropTank::PropTank(byte level, BoxMarkStruct* bms) :
	EnemyBase(TANK_KIND::PROP, level, bms)
{
	mTank[0] = new TankInfo(GRAY_TANK, level, true);
	mTank[1] = new TankInfo(RED_TANK, level, true);
}

PropTank::~PropTank()
{
	// 不能直接 delete[] mTank??
	for (int i = 0; i < 2; i++)
		delete mTank[i];
	printf("~PropTank::PropTank()..\n");
}

// 
void PropTank::DrawTank(const HDC& center_hdc)
{
	if (!mStar.mIsOuted || mDied)
		return; //printf("%d..\n", mTankImageIndex);
	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&mTank[ index_counter++ / 6 % 2 ]->GetTankImage(mTankDir, mTankImageIndex++ / 3 % 2)), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
}

//////////////////////////////////////////////////////////////

BigestTank::BigestTank(TANK_KIND kind, BoxMarkStruct * bms):
	EnemyBase(kind, 3, bms)
{
	mTank[GRAY_TANK] = new TankInfo(GRAY_TANK, 3, true);
	mTank[YELLOW_TANK] = new TankInfo(YELLOW_TANK, 3, true);
	mTank[RED_TANK] = new TankInfo(RED_TANK, 3, true);
	mTank[GREEN_TANK] = new TankInfo(GREEN_TANK, 3, true);

	hp = 4;
}

BigestTank::~BigestTank()
{
	for (int i = 0; i < 4; i++)
		delete mTank[i];
	printf("~BigestTank::BigestTank()\n");
}

void BigestTank::DrawTank(const HDC & center_hdc)
{
	if (!mStar.mIsOuted || mDied)
		return;

	// 道具坦克和普通坦克变色区别
	TankInfo* temp[2] = { mTank[GRAY_TANK], mTank[GRAY_TANK] };

	switch (mEnemyTankKind)
	{
	case TANK_KIND::PROP:
		switch(hp)
		{
			case 4:
				temp[0] = mTank[RED_TANK];
				temp[1] = mTank[GRAY_TANK];
				break;
			case 3:
				temp[0] = mTank[RED_TANK];
				temp[1] = mTank[YELLOW_TANK];
				break;
			case 2:
				temp[0] = mTank[YELLOW_TANK];
				temp[1] = mTank[GRAY_TANK];
				break;
			default:
				break;
		}
		break;
	case TANK_KIND::COMMON:
		switch (hp)
		{
		case 4:
			temp[0] = mTank[GREEN_TANK];
			temp[1] = mTank[GRAY_TANK];
			break;
		case 3:
			temp[0] = mTank[GRAY_TANK];
			temp[1] = mTank[YELLOW_TANK];
			break;
		case 2:
			temp[0] = mTank[YELLOW_TANK];
			temp[1] = mTank[GREEN_TANK];
			break;
		default:
			break;
		}
		break;
	}

	if (temp == NULL)
	{
		printf("错误!. EnemyBase.cpp");
		return;
	}
	TransparentBlt(center_hdc, (int)mTankX - BOX_SIZE, (int)mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&temp[index_counter++ / 7 % 2]->GetTankImage(mTankDir, mTankImageIndex++ / 3 % 2)), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
}

bool BigestTank::BeKill(bool killanyway)
{
	if (mStar.mIsOuted == false || mBlast.IsBlasting() || mDied == true)
		return false;

	MciSound::_PlaySound(S_BIN);
	if (--hp <= 0 || killanyway)
	{
		hp = 0;
		return this->EnemyBase::BeKill(killanyway);
	}
	return false;
}
