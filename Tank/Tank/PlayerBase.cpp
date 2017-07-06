#include "stdafx.h"
#include "PlayerBase.h"

int PlayerBase::mDevXY[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};	// 依次左上右下

PlayerBase::PlayerBase( byte player )
{
	player_id = player;
	mPlayerTank = new PlayerTank(player_id);

	// 不同玩家数据不同
	if (player_id == 0)
	{
		m12PImage = Image::FromFile(L"./res/big/1p.png");	// 1P\2P图标
		m12PImage_x = 350;									// 1P\2P 坐标
		m12PImage_y = 190;
		mPlayerTankIcoImage_x = 348;						// 玩家坦克图标坐标
		mPlayerTankIcoImage_y = 203;
		mPlayerLife_x = 361;								// 玩家生命值坐标
		mPlayerLife_y = 203;
		mTankX =  4 * 24 + CENTER_X;						// 坦克首次出现时候的坐标
		mTankY = 7 * 24 + CENTER_Y;
	}
	else
	{
		m12PImage = Image::FromFile(L"./res/big/2p.png");
		m12PImage_x = 350;
		m12PImage_y = 226;
		mPlayerTankIcoImage_x = 348;
		mPlayerTankIcoImage_y = 239;
		mPlayerLife_x = 361;
		mPlayerLife_y = 239;
		mTankX =  8 * 24 + CENTER_X;
		mTankY = 12 * 24 + CENTER_Y;
	}

	// 共同的数据
	mPlayerTankIcoImage = Image::FromFile(L"./res/big/playertank-ico.png");		// 玩家坦克图标
	mPlayerLife = 2;
	mBlackNumberImage = Image::FromFile(L"./res/big/black-number.png");
	mPlayerTankLevel = 0;
	mTankDir = DIR_LEFT;
	mSpeed = 4;
	mKeyCounter = 3;
}

PlayerBase::~PlayerBase()
{
}

void PlayerBase::DrawPlayerTankIco( Graphics* graphics )
{
	// 绘制1P/2P
	graphics->DrawImage( m12PImage, m12PImage_x, m12PImage_y, PLAYER_12_ICO_X, PLAYER_12_ICO_Y ); 
	// 绘制坦克图标
	graphics->DrawImage( mPlayerTankIcoImage, mPlayerTankIcoImage_x, mPlayerTankIcoImage_y, PLAYER_TANK_ICO_X, PLAYER_TANK_ICO_Y );
	// 绘制玩家生命数字
	graphics->DrawImage( mBlackNumberImage, Rect(mPlayerLife_x, mPlayerLife_y, 11, 11), 11 * mPlayerLife, 0, 11, 11, UnitPixel);
}

void PlayerBase::DrawPlayerTank( Graphics* graphics )
{
	Image* tank = mPlayerTank->GetTankImage( mPlayerTankLevel, mTankDir );
	graphics->DrawImage( tank, mTankX, mTankY, tank->GetWidth(), tank->GetHeight() );
}

bool PlayerBase::PlayerControl( BoxMarkStruct* bms )
{
	if ( GetAsyncKeyState(27) & 0x8000 )
		return false;

	switch( player_id )
	{
	case 0:										// 玩家一
		//if ( mKeyCounter++ != 3 )
		//	break;
		if ( GetAsyncKeyState('A') & 0x8000 )
		{
			if ( mTankDir != DIR_LEFT )			// 仅转向
			{
				mTankDir = DIR_LEFT;
			}
			else								// 移动
			{
				mTankX += mDevXY[DIR_LEFT][0] * mSpeed;
				mTankY += mDevXY[DIR_LEFT][1] * mSpeed;
			}
		}
		else if ( GetAsyncKeyState('W') & 0x8000 )
		{
			if ( mTankDir != DIR_UP )			// 仅转向
			{
				mTankDir = DIR_UP;
			}
			else								// 移动
			{
				mTankX += mDevXY[DIR_UP][0] * mSpeed;
				mTankY += mDevXY[DIR_UP][1] * mSpeed;
			}
		}
		else if ( GetAsyncKeyState('D') & 0x8000 )
		{
			if ( mTankDir != DIR_RIGHT )		// 仅转向
			{
				mTankDir = DIR_RIGHT;
			}
			else								// 移动
			{
				mTankX += mDevXY[DIR_RIGHT][0] * mSpeed;
				mTankY += mDevXY[DIR_RIGHT][1] * mSpeed;
			}
		}
		else if ( GetAsyncKeyState('S') & 0x8000 )
		{
			if ( mTankDir != DIR_DOWN )			// 仅转向
			{
				mTankDir = DIR_DOWN;
			}
			else								// 移动
			{
				mTankX += mDevXY[DIR_DOWN][0] * mSpeed;
				mTankY += mDevXY[DIR_DOWN][1] * mSpeed;
			}
		}
		break;

	case 1:										// 玩家二
		//if ( mKeyCounter++ != 3 )
		//	break;
		if ( GetAsyncKeyState(VK_LEFT) & 0x8000 )
		{
			if ( mTankDir != DIR_LEFT )			// 仅转向
			{
				mTankDir = DIR_LEFT;
			}
			else								// 移动
			{
				mTankX += mDevXY[DIR_LEFT][0] * mSpeed;
				mTankY += mDevXY[DIR_LEFT][1] * mSpeed;
			}
		}
		else if ( GetAsyncKeyState(VK_UP) & 0x8000 )
		{
			if ( mTankDir != DIR_UP )			// 仅转向
			{
				mTankDir = DIR_UP;
			}
			else								// 移动
			{
				mTankX += mDevXY[DIR_UP][0] * mSpeed;
				mTankY += mDevXY[DIR_UP][1] * mSpeed;
			}
		}
		else if ( GetAsyncKeyState(VK_RIGHT) & 0x8000 )
		{
			if ( mTankDir != DIR_RIGHT )		// 仅转向
			{
				mTankDir = DIR_RIGHT;
			}
			else								// 移动
			{
				mTankX += mDevXY[DIR_RIGHT][0] * mSpeed;
				mTankY += mDevXY[DIR_RIGHT][1] * mSpeed;
			}
		}
		else if ( GetAsyncKeyState(VK_DOWN) & 0x8000 )
		{
			if ( mTankDir != DIR_DOWN )			// 仅转向
			{
				mTankDir = DIR_DOWN;
			}
			else								// 移动
			{
				mTankX += mDevXY[DIR_DOWN][0] * mSpeed;
				mTankY += mDevXY[DIR_DOWN][1] * mSpeed;
			}
		}
		break;
	default:
		break;
	}

	return true;
}