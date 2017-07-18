#include "stdafx.h"
#include "TankClass.h" 
#include "struct.h"

/////////////////////////////f///////////////////////////

// 敌机坦克信息
TankInfo::TankInfo(byte tank_color, int enemy_level, bool notuse)
{
	//mTankDir = 1;
	TCHAR c[100];
	switch(tank_color)
	{
	case GRAY_TANK:		// 灰色坦克
		for ( int i = 0; i < 4; i++ )
		{
			_stprintf_s(c, L"./res/big/gray-tank/%d-%d-1.gif", enemy_level + 1, i + 1 );
			loadimage(&mTankImage[i][0], c);
			_stprintf_s(c, L"./res/big/gray-tank/%d-%d-2.gif", enemy_level + 1, i + 1 );
			loadimage(&mTankImage[i][1], c);
		}
		break;
	case RED_TANK:		// 红色坦克
		for ( int i = 0; i < 4; i++ )
		{
			_stprintf_s(c, L"./res/big/red-tank/%d-%d-1.gif", enemy_level + 1, i + 1);
			loadimage(&mTankImage[i][0], c);
			_stprintf_s(c, L"./res/big/red-tank/%d-%d-2.gif", enemy_level + 1, i + 1);
			loadimage(&mTankImage[i][1], c);
		}
		break;
	case GREEN_TANK:
		for (int i = 0; i < 4; i++)
		{
			_stprintf_s(c, L"./res/big/other-tank/g%d-%d-1.gif", enemy_level + 1, i + 1);
			loadimage(&mTankImage[i][0], c);
			_stprintf_s(c, L"./res/big/other-tank/g%d-%d-2.gif", enemy_level + 1, i + 1);
			loadimage(&mTankImage[i][1], c);
		}
		break;
	case YELLOW_TANK:
		for (int i = 0; i < 4; i++)
		{
			_stprintf_s(c, L"./res/big/other-tank/y%d-%d-1.gif", enemy_level + 1, i + 1);
			loadimage(&mTankImage[i][0], c);
			_stprintf_s(c, L"./res/big/other-tank/y%d-%d-2.gif", enemy_level + 1, i + 1);
			loadimage(&mTankImage[i][1], c);
		}
		break;
	default:
		throw _T("异常 TankInfo::TankInfo(byte enemy_kind, int enemy_level)");
	}
}

// 玩家坦克信息
TankInfo::TankInfo(byte player, byte level)
{
	switch(player)
	{
	case 0:
	case 1:
		{
			TCHAR c[50];
			for ( int i = 0; i < 4; i++ )
			{
				_stprintf_s(c, L"./res/big/%dPlayer/m%d-%d-1.gif", player, level, i);
				//mTankImage[i][0] = Image::FromFile(c);
				loadimage(&mTankImage[i][0], c);
				_stprintf_s(c, L"./res/big/%dPlayer/m%d-%d-2.gif", player, level, i );
				loadimage(&mTankImage[i][1], c);
				//mTankImage[i][1] = Image::FromFile(c);
			}
		}
		break;
	default:
		throw _T("数值越界, TankClass.cpp-> TankInfo construct function");
	}
}

TankInfo::~TankInfo()
{
}

IMAGE TankInfo::GetTankImage(byte dir, int index)
{
	return mTankImage[dir][index];
}

/////////////////////////////////////////////////////////
// 玩家坦克级别
PlayerTank::PlayerTank( byte player )
{
	mMoveIndex = 0;
	for ( int i = 0; i < 4; i++ )
		mTankInfo[i] = new TankInfo(player, i);
}

PlayerTank::~PlayerTank()
{
	for (int i = 0; i < 4; i++)
		delete mTankInfo[i];

	//delete[] mTankInfo;
	printf("PlayerTank::~PlayerTank() \n");
}

// 定义随机索引, 每三次跟换一个动作
IMAGE PlayerTank::GetTankImage(byte level, byte dir, bool moving)
{							// 只会赋值一次
	mMoveIndex = moving ? mMoveIndex + 1 : 0;
	return mTankInfo[level]->GetTankImage( dir, mMoveIndex );
}





















