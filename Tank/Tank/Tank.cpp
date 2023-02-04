
/***************************************
* ģ�� fc-tank ��д�� easyx ��Ϸ
* ���뻷����Visual C++ 2019��EasyX_20200109(beta)
* EasyX ��ͼ���ַ��https://easyx.cn
* ���ߣ�xiongfj ��837943056@qq.com��
***************************************/

#include "stdafx.h"
#include "struct.h"
#include "SelectPanel.h"
#include "GameControl.h"
#include "MciSound.h"

#define ONE_PLAYER	1		// �����޸ĸ�ֵ!! �Ѿ����������±�!!
#define TWO_PLAYER	2

/***
* TankClass.PlayerBase ��
- �ö����� GameControl ��ʵ����,�洢

* TankClass.EnemyTank ��

* SelectPanel ��
- ������ʾѡ�����

* GameConrol ��
- ���� main ���ݽ���������, �洢 PlayerBase ��������
- �� Graphics* ��-> PlayerBase ��������л���

* ������ main()
- ���� SelectPanel ����, ��ʾ���ѡ�����
- ���� Gamecontrol ����, �������ѡ����
***/

//
void main()
{
	srand((unsigned)time(0));
	bool isCustomMap = false;		// �Զ����ͼ
	MciSound::InitSounds();

	// ��ɫ����
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	BeginBatchDraw();

	// ���� image ����
	IMAGE canvas_img(CANVAS_WIDTH, CANVAS_HEIGHT);

	// ��ȡ graphics ��ͼ����
	HDC des_hdc = GetImageHDC();
	HDC canvas_hdc = GetImageHDC(&canvas_img);

	SelectPanel* selecter = new SelectPanel(des_hdc, canvas_hdc);	// ��ʾ��ҹ���ѡ�����
	GameControl* control = NULL;// new GameControl(des_hdc, canvas_hdc/*, &Q_boxmark*/);
	EnumSelectResult result;

	while (_kbhit() != 27)
	{
		result = selecter->ShowSelectPanel();		// ��ȡ���ѡ����

		if (!isCustomMap)
		{
			if (control != NULL)
				delete control;
			control = new GameControl(des_hdc, canvas_hdc);
		}

		switch (result)
		{
			case OnePlayer:
				control->AddPlayer(ONE_PLAYER);		// һ�����

				if (isCustomMap)
					control->GameLoop();
				else
				{
					control->LoadMap();
					control->GameLoop();
				}
				isCustomMap = false;		// �Զ����ͼ�ؿ������Զ����к���Ĺؿ�
				break;

			case TwoPlayer:
				control->AddPlayer(TWO_PLAYER);		// �������

				if (isCustomMap)
					control->GameLoop();
				else
				{
					control->LoadMap();
					control->GameLoop();
				}
				isCustomMap = false;
				break;

			case Custom:							// ����Զ����ͼ
				control->CreateMap(&isCustomMap);
				break;

			default:
				return;
		}
	}
	 
	closegraph();
}

/*
* _WALL ���������־Ϊ _CLEAR = -1;
*/