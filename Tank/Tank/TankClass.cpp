#include "stdafx.h"
#include "TankClass.h" 

////////////////////////////////////////////////////////

// 敌机坦克信息
TankInfo::TankInfo(byte enemy_kind, int enemy_level, bool notuse)
{
	//mTankDir = 1;
	TCHAR c[50];
	switch(enemy_kind)
	{
	case 0:		// 道具坦克
		for ( int i = 0; i < 4; i++ )
		{
			_stprintf_s(c, L"./res/big/prop-tank/%d-%d-1.gif", enemy_level, i );
			//mTankImage[i][0] = Image::FromFile(c);
			loadimage(&mTankImage[i][0], c);
			_stprintf_s(c, L"./res/big/prop-tank/%d-%d-2.gif", enemy_level, i );
			//mTankImage[i][1] = Image::FromFile(c);
			loadimage(&mTankImage[i][1], c);
		}
		break;
	case 1:		// 普通坦克
		for ( int i = 0; i < 4; i++ )
		{
			_stprintf_s(c, L"./res/big/common-tank/%d-%d-1.gif", enemy_level, i );

			//mTankImage[i][0] = Image::FromFile(c);
			loadimage(&mTankImage[i][0], c);
			_stprintf_s(c, L"./res/big/common-tank/%d-%d-2.gif", enemy_level, i );

			//mTankImage[i][1] = Image::FromFile(c);
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

IMAGE TankInfo::GetTankImage(byte dir)
{
	return mTankImage[dir][1];
}

/////////////////////////////////////////////////////////
// 玩家坦克级别
PlayerTank::PlayerTank( byte player )
{
	for ( int i = 0; i < 4; i++ )
		mTankInfo[i] = new TankInfo(player, i);
}

PlayerTank::~PlayerTank()
{
}

IMAGE PlayerTank::GetTankImage(byte level, byte dir)
{
	return mTankInfo[level]->GetTankImage( dir );
}

/////////////////////////////////////////////////////////
// 敌机坦克控制
EnemyTank::EnemyTank()
{
	mEnemyTankKind = 0;		// 敌机类别, 道具坦克和普通坦克两种, [0-1]
	mEnemyTankLevel = 0;		// 敌机坦克5个级别 [0-4]
	mEnemyTank = new TankInfo(mEnemyTankKind, mEnemyTankLevel, true);		// 指向坦克详细信息
}

// 生成某个类别级别的敌机
EnemyTank::EnemyTank(byte kind, int level)
{
	mEnemyTankKind = kind;
	mEnemyTankLevel = level;
	mEnemyTank = new TankInfo(mEnemyTankKind, mEnemyTankLevel, true);
}

EnemyTank::~EnemyTank()
{
}





















