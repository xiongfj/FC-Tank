#include "stdafx.h"
#include "SelectPanel.h"
#include "GameControl.h"
SelectPanel::SelectPanel( HDC des_hdc, HDC image_hdc)
{
	//mGraphics = grap;
	mDes_hdc = des_hdc;
	mImage_hdc = image_hdc;
	loadimage(&mSelect_player_image, _T("./res/big/select_player.gif"));

	// ѡ��̹���ֱ��α�
	loadimage(&mSelectTankImage[0], _T("./res/big/0Player/m0-2-1.gif"));
	loadimage(&mSelectTankImage[1], _T("./res/big/0Player/m0-2-2.gif"));
	// ��ɫ����
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

// ��ʾ��Ϸ��ʼ�������
EnumSelectResult SelectPanel::ShowSelectPanel()
{
	cleardevice();		// ��ֹ��Ϸʧ�ܷ���������ʾ GAMEOVER ������Ļ

	// ѡ����������������
	while (mSelect_player_image_y > 0)
	{
		// ����������̰��»س�����ֱ�ӽ�����������ʾ��󶨸��ҳ��
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			mSelect_player_image_y = 0;

		Sleep(55);
		mSelect_player_image_y -= 5;
		if ( mSelect_player_image_y < 0 )
			mSelect_player_image_y = 0;

		// ������ mImage_hdc ��
		BitBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mSelect_player_image), 0, 0, SRCCOPY);
		
		// �� mImage_hdc ���Ƶ������� mDes_hdc ��
		StretchBlt(mDes_hdc, 0, mSelect_player_image_y, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0 , CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();	// ����� Tank.cpp BeginBatchDraw(); ��ʼ���˴��Ļ����ͼ��������ʾ�����ڣ�Ŀ�ģ�������˸
	}

	int temp = 0;					// ���ư�����Ӧ�ٶ�,����̫��!
	// ��ҿ��Խ��� ���¡��س�������ѡ����Ϸģʽ
	// ��ҿ�ʼѡ����Ϸ����
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


	// ��Ӧ��ҵ�ѡ��
	switch( mSelectIndex )
	{
	case 0:
		// ��������, �´β�������
		Init();
		return OnePlayer;
	case 1:
		// ��������, �´β�������
		Init();
		return TwoPlayer;
	case 2:
		// ��������, �´β�������
		Init();
		return Custom;
	default:
		throw _T("void SelectPanel::ShowSelectPanel() �쳣");
	}
	return Error;
}
