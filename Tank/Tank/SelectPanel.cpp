#include "stdafx.h"
#include "SelectPanel.h"
#include "GameControl.h"
SelectPanel::SelectPanel( HDC des_hdc, HDC image_hdc)
{
	//mGraphics = grap;
	mDes_hdc = des_hdc;
	mImage_hdc = image_hdc;
	loadimage(&mSelect_player_image, _T("./res/big/select_player.gif"));

	// 选择坦克手柄游标
	loadimage(&mSelectTankImage[0], _T("./res/big/0Player/m0-2-1.gif"));
	loadimage(&mSelectTankImage[1], _T("./res/big/0Player/m0-2-2.gif"));
	// 灰色背景
	loadimage(&mGrayBackgroundImage, _T("./res/big/bg_gray.gif"));
	loadimage(&mBlackNumberImage, _T("./res/big/black-number.gif"));

	mSelectTankPoint[0].x = 60;
	mSelectTankPoint[1].x = 60;
	mSelectTankPoint[2].x = 60;
	mSelectTankPoint[0].y = 123;
	mSelectTankPoint[1].y = 139;
	mSelectTankPoint[2].y = 156;

	Init();
}

SelectPanel::~SelectPanel()
{
}

void SelectPanel::Init()
{
	mSelect_player_image_y = WINDOW_HEIGHT;

	mSelectIndex = 1;
	mCounter = 1;
}

// 显示游戏开始控制面板
EnumSelectResult SelectPanel::ShowSelectPanel()
{
	cleardevice();		// 防止游戏失败分数面板后显示 GAMEOVER 残留屏幕

	// 选择玩家面板上升动画
	while (mSelect_player_image_y > 0)
	{
		// 如果上升过程按下回车键，直接结束动画，显示最后定格的页面
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			mSelect_player_image_y = 0;

		Sleep(55);
		mSelect_player_image_y -= 5;
		if ( mSelect_player_image_y < 0 )
			mSelect_player_image_y = 0;

		// 绘制在 mImage_hdc 上
		BitBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mSelect_player_image), 0, 0, SRCCOPY);
		
		// 将 mImage_hdc 绘制到主窗口 mDes_hdc 上
		StretchBlt(mDes_hdc, 0, mSelect_player_image_y, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0 , CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();	// 输出从 Tank.cpp BeginBatchDraw(); 开始到此处的缓存绘图，才能显示到窗口，目的：避免闪烁
	}

	int temp = 0;					// 控制按键响应速度,不能太快!
	// 玩家可以进行 上下、回车操作，选择游戏模式
	// 玩家开始选择游戏功能
	while ( true )
	{
		Sleep(40);
		mCounter++;
		BitBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mSelect_player_image), 0, 0, SRCCOPY);

		TransparentBlt(mImage_hdc, mSelectTankPoint[mSelectIndex].x, mSelectTankPoint[mSelectIndex].y, 16, 16,
			GetImageHDC(&mSelectTankImage[mCounter]), 0, 0, 16, 16, 0x000000);

		StretchBlt(mDes_hdc, 0, mSelect_player_image_y, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();

		temp++;
		if ( GetAsyncKeyState(VK_UP) & 0x8000 && temp > 3 )
		{
			temp = 0;
			mSelectIndex = ( mSelectIndex - 1 >= 0 ) ? mSelectIndex - 1 : 2;
		}
		else if ( GetAsyncKeyState(VK_DOWN) & 0x8000 && temp > 3 )
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


	// 响应玩家的选择
	switch( mSelectIndex )
	{
	case 0:
		// 重置数据, 下次才能上升
		Init();
		return OnePlayer;
	case 1:
		// 重置数据, 下次才能上升
		Init();
		return TwoPlayer;
	case 2:
		// 重置数据, 下次才能上升
		Init();
		return Custom;
	default:
		throw _T("void SelectPanel::ShowSelectPanel() 异常");
	}
	return Error;
}
