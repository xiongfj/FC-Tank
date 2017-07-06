#include "stdafx.h"
#include "SelectPanel.h"
#include "GameControl.h"
SelectPanel::SelectPanel(Graphics* grap, HDC des_hdc, HDC image_hdc)
{
	mGraphics = grap;
	mDes_hdc = des_hdc;
	mImage_hdc = image_hdc;
	Init();
}

SelectPanel::~SelectPanel()
{
}

void SelectPanel::Init()
{
	// 背景选择图片
	mSelect_player_image_y = WINDOW_HEIGHT;
	mSelect_player_image = Image::FromFile( L"./res/big/select_player.bmp" );

	// 选择坦克手柄游标
	mSelectTankImage[0] = Image::FromFile( L"./res/big/0Player/m0-2-1.png" );
	mSelectTankImage[1] = Image::FromFile( L"./res/big/0Player/m0-2-2.png" );
	mSelectTankPoint[0].x = 60;
	mSelectTankPoint[1].x = 60;
	mSelectTankPoint[2].x = 60;
	mSelectTankPoint[0].y = 123;
	mSelectTankPoint[1].y = 137;
	mSelectTankPoint[2].y = 153;
	mSelectIndex = 1;
	mCounter = 1;

	// 灰色背景
	mGrayBackgroundImage = Image::FromFile(L"./res/big/bg_gray.png");
	// 黑色背景
	//mBlackBackgroundImage = Image::FromFile(L"./res/big/bg_black.png");
	// STAGE 字样
	mCurrentStageImage = Image::FromFile(L"./res/big/stage.png");
	// 当前关卡
	//mCurrentStage = 2;
	// 黑色 1234567890 数字
	mBlackNumberImage = Image::FromFile(L"./res/big/black-number.png");
}

//
EnumSelectResult SelectPanel::ShowSelectPanel()
{
	// 选择玩家面板上升动画
	while (true)
	{
		Sleep(19);
		mSelect_player_image_y -= 35;
		if ( mSelect_player_image_y < 0 )
			mSelect_player_image_y = 0;
		// 绘制在 mImage_hdc 上
		mGraphics->DrawImage(mSelect_player_image, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT );
		
		// 将 mImage_hdc 绘制到主窗口 mDes_hdc 上
		StretchBlt(mDes_hdc, 0, mSelect_player_image_y, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();

		if ( mSelect_player_image_y == 0 )
			break; 
	}

	int temp = 0;					// 控制按键响应速度,不能太快!
	// 玩家开始选择游戏功能
	while ( true )
	{
		Sleep(40);
		mCounter++;
		mGraphics->DrawImage(mSelect_player_image, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT );
		mGraphics->DrawImage(mSelectTankImage[mCounter], mSelectTankPoint[mSelectIndex].x, mSelectTankPoint[mSelectIndex].y ,
				15, 13 );
		StretchBlt(mDes_hdc, 0, mSelect_player_image_y, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();

		temp++;
		if ( GetAsyncKeyState('W') & 0x8000 && temp > 3 )
		{
			temp = 0;
			mSelectIndex = ( mSelectIndex - 1 >= 0 ) ? mSelectIndex - 1 : 2;
		}
		else if ( GetAsyncKeyState('S') & 0x8000 && temp > 3 )
		{
			temp = 0;
			mSelectIndex = ( mSelectIndex + 1 < 3 ) ? mSelectIndex + 1 : 0;
		}
		else if ( GetAsyncKeyState(VK_RETURN) & 0x8000 && temp > 3 )
		{
			temp = 0;
			break;
		}
	}

	switch( mSelectIndex )
	{
	case 0:
		ShowStage();
		return OnePlayer;
	case 1:
		ShowStage();
		return TwoPlayer;
	case 2:
		return Custom;
	default:
		throw _T("void SelectPanel::ShowSelectPanel() 异常");
	}
	return Error;
}

void SelectPanel::ShowStage()
{
	// 灰色背景
	mGraphics->DrawImage(mGrayBackgroundImage, 0, 0, CANVAS_WIDTH + 10, CANVAS_HEIGHT + 10);	// +10 去掉由于拉伸的边缘变色
	mGraphics->DrawImage(mCurrentStageImage, 97, 100, 40, 11);								// "STAGE" 字样

	// [1-9] 关卡，单个数字
	if ( GameControl::mCurrentStage < 10 )
		mGraphics->DrawImage(mBlackNumberImage, Rect( 157, 102, 8, 8), 8 * GameControl::mCurrentStage, 0, 8, 8, UnitPixel); // Gdiplus::Unit::UnitPixel
	else	// 10,11,12 .. 双位数关卡
	{
		mGraphics->DrawImage(mBlackNumberImage, Rect( 157, 102, 8, 8), 8 * (GameControl::mCurrentStage / 10), 0, 8, 8, UnitPixel);
		mGraphics->DrawImage(mBlackNumberImage, Rect( 163, 102, 8, 8), 8 * (GameControl::mCurrentStage % 10), 0, 8, 8, UnitPixel);
	}
	StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
	FlushBatchDraw();

	Sleep(300);
}