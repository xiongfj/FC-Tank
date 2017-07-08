#include "stdafx.h"
#include "PlayerBase.h"

//BoxMarkStruct* PlayerBase::mBms = NULL;
int PlayerBase::mDevXY[4][2] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1} };	// 依次左上右下

PlayerBase::PlayerBase(byte player)
{
	player_id = player;
	mPlayerTank = new PlayerTank(player_id);
	//mBms = bms;												// 格子标记结构指针, 指向 main 文件定义的一个结构体

	// 不同玩家数据不同
	if (player_id == 0)
	{
		loadimage(&m12PImage, _T("./res/big/1P.gif"));		// 1P\2P图标
		m12PImage_x = 233;									// 1P\2P 坐标
		m12PImage_y = 129;
		mPlayerTankIcoImage_x = 232;						// 玩家坦克图标坐标
		mPlayerTankIcoImage_y = 137;
		mPlayerLife_x = 240;								// 玩家生命值坐标
		mPlayerLife_y = 137;
		mTankX = 4 * 16 + BOX_SIZE;							// 坦克首次出现时候的中心坐标
		mTankY = 7 * 16 + BOX_SIZE;
	}
	else
	{
		loadimage(&m12PImage, _T("./res/big/2P.gif"));
		m12PImage_x = 233;
		m12PImage_y = 153;
		mPlayerTankIcoImage_x = 232;
		mPlayerTankIcoImage_y = 161;
		mPlayerLife_x = 240;
		mPlayerLife_y = 161;
		mTankX = 8 * 16 + BOX_SIZE;
		mTankY = 12 * 16 + BOX_SIZE;
	}

	// 共同的数据
	loadimage(&mPlayerTankIcoImage, _T("./res/big/playertank-ico.gif"	));	// 玩家坦克图标
	loadimage(&mBlackNumberImage,	_T("./res/big/black-number.gif"		));	// 黑色数字
	mPlayerLife = 2;
	mPlayerTankLevel = 3;
	mTankDir = DIR_LEFT;
	mSpeed = 3;
	//mKeyCounter = 3;
}

PlayerBase::~PlayerBase()
{
}

// 绘制玩家的一些数据: 1P\2P 坦克图标 生命
void PlayerBase::DrawPlayerTankIco(HDC right_panel_hdc)
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

//
void PlayerBase::DrawPlayerTank(HDC canvas_hdc)
{
	IMAGE tank = mPlayerTank->GetTankImage(mPlayerTankLevel, mTankDir);
	TransparentBlt(canvas_hdc, mTankX - BOX_SIZE, mTankY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2, GetImageHDC(&tank), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
}

//
bool PlayerBase::PlayerControl(BoxMarkStruct* bms)
{
	if (GetAsyncKeyState(27) & 0x8000)
		return false;

	switch (player_id)
	{
	case 0:										// 玩家一
		if (GetAsyncKeyState('A') & 0x8000)
		{
			if (mTankDir != DIR_LEFT)			// 仅转向
			{
				ChangeDir(DIR_LEFT);
			}
			else								// 移动
			{
				if (CheckMoveable(DIR_LEFT, bms))
				{
					mTankX += mDevXY[DIR_LEFT][0] * mSpeed;
					mTankY += mDevXY[DIR_LEFT][1] * mSpeed;
				}
			}
		}
		else if (GetAsyncKeyState('W') & 0x8000)
		{
			if (mTankDir != DIR_UP)			// 仅转向
			{
				ChangeDir(DIR_UP);
			}
			else								// 移动
			{
				if (CheckMoveable(DIR_UP, bms))
				{
					mTankX += mDevXY[DIR_UP][0] * mSpeed;
					mTankY += mDevXY[DIR_UP][1] * mSpeed;
				}
			}
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			if (mTankDir != DIR_RIGHT)		// 仅转向
			{
				ChangeDir(DIR_RIGHT);
			}
			else								// 移动
			{
				if (CheckMoveable(DIR_RIGHT, bms))
				{
					mTankX += mDevXY[DIR_RIGHT][0] * mSpeed;
					mTankY += mDevXY[DIR_RIGHT][1] * mSpeed;
				}
			}
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			if (mTankDir != DIR_DOWN)			// 仅转向
			{
				ChangeDir(DIR_DOWN);
			}
			else								// 移动
			{
				if (CheckMoveable(DIR_DOWN, bms))
				{
					mTankX += mDevXY[DIR_DOWN][0] * mSpeed;
					mTankY += mDevXY[DIR_DOWN][1] * mSpeed;
				}
			}
		}
		break;

	case 1:										// 玩家二
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			if (mTankDir != DIR_LEFT)			// 仅转向
			{
				ChangeDir(DIR_LEFT);
			}
			else								// 移动
			{
				if (CheckMoveable(DIR_LEFT, bms))
				{
					mTankX += mDevXY[DIR_LEFT][0] * mSpeed;
					mTankY += mDevXY[DIR_LEFT][1] * mSpeed;
				}
			}
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			if (mTankDir != DIR_UP)			// 仅转向
			{
				ChangeDir(DIR_UP);
			}
			else								// 移动
			{
				if (CheckMoveable(DIR_UP, bms))
				{
					mTankX += mDevXY[DIR_UP][0] * mSpeed;
					mTankY += mDevXY[DIR_UP][1] * mSpeed;
				}
			}
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			if (mTankDir != DIR_RIGHT)		// 仅转向
			{
				ChangeDir(DIR_RIGHT);
			}
			else								// 移动
			{
				if (CheckMoveable(DIR_RIGHT, bms))
				{
					mTankX += mDevXY[DIR_RIGHT][0] * mSpeed;
					mTankY += mDevXY[DIR_RIGHT][1] * mSpeed;
				}
			}
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			if (mTankDir != DIR_DOWN)			// 仅转向
			{
				ChangeDir(DIR_DOWN);
			}
			else								// 移动
			{
				if (CheckMoveable(DIR_DOWN, bms))
				{
					mTankX += mDevXY[DIR_DOWN][0] * mSpeed;
					mTankY += mDevXY[DIR_DOWN][1] * mSpeed;
				}
			}
		}
		break;
	default:
		break;
	}

	return true;
}

//---------------------------------------------------------------- private function ---------

// 变向的同时调整坦克所在格子. 必须保证坦克中心在格子线上
void PlayerBase::ChangeDir(int new_dir)
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

	// 更改方向
	mTankDir = new_dir;
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
* 如果 (x,y) 在 a 点出, 转换后的 i,j 属于格子 4
* 如果 x 值在 a 点左边, 则转换后的 j 属于 1或3; 右边则属于 2或4
* 如果 y 值在 a 点以上, 则转换后的 i 属于 1或2; 以下则属于 3或4
*/
bool PlayerBase::CheckMoveable(byte dir, BoxMarkStruct* bms)
{
	// 坦克中心坐标
	int tempx = mTankX + mDevXY[mTankDir][0] * mSpeed;
	int tempy = mTankY + mDevXY[mTankDir][1] * mSpeed;

	if (tempx < BOX_SIZE || tempy < BOX_SIZE || tempy > CENTER_WIDTH - BOX_SIZE || tempx > CENTER_HEIGHT - BOX_SIZE)
	{
		// 如果遇到障碍物,将坦克坐标调整到格子线上. 不然坦克和障碍物会有几个像素点间隔
		switch (dir)
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

	int dev[4][2][2] = { {{-1,-1},{0,-1}},  {{-1,-1},{-1,0}},  {{-1,1},{0,1}}, { {1,-1},{1,0}} };

	if (bms->box_8[index_i + dev[dir][0][0]][index_j + dev[dir][0][1]] > 2 ||
		bms->box_8[index_i + dev[dir][1][0]][index_j + dev[dir][1][1]] > 2 )
	{
		// 如果遇到障碍物,将坦克坐标调整到格子线上. 不然坦克和障碍物会有几个像素点间隔
		switch (dir)
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
