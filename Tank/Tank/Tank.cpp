
/***************************************
* 模拟 fc-tank 编写的 easyx 游戏
* 编译环境：Visual C++ 2019，EasyX_20200109(beta)
* EasyX 绘图库地址：https://easyx.cn
* 作者：xiongfj （837943056@qq.com）
***************************************/

#include "stdafx.h"
#include "struct.h"
#include "SelectPanel.h"
#include "GameControl.h"
#include "MciSound.h"

#define ONE_PLAYER	1		// 不能修改该值!! 已经用于数组下标!!
#define TWO_PLAYER	2

/***
* TankClass.PlayerBase 类
- 该对象在 GameControl 内实例化,存储

* TankClass.EnemyTank 类

* SelectPanel 类
- 单独显示选择面板

* GameConrol 类
- 接收 main 传递进来的数据, 存储 PlayerBase 对象链表
- 将 Graphics* 传-> PlayerBase 内玩家自行绘制

* 主函数 main()
- 定义 SelectPanel 对象, 显示玩家选择面板
- 定义 Gamecontrol 对象, 传入玩家选择结果
***/

//
void main()
{
	srand((unsigned)time(0));
	bool isCustomMap = false;		// 自定义地图
	MciSound::InitSounds();

	// 灰色背景
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	BeginBatchDraw();

	// 定义 image 画布
	IMAGE canvas_img(CANVAS_WIDTH, CANVAS_HEIGHT);

	// 获取 graphics 绘图对象
	HDC des_hdc = GetImageHDC();
	HDC canvas_hdc = GetImageHDC(&canvas_img);

	SelectPanel* selecter = new SelectPanel(des_hdc, canvas_hdc);	// 显示玩家功能选择面板
	GameControl* control = NULL;// new GameControl(des_hdc, canvas_hdc/*, &Q_boxmark*/);
	EnumSelectResult result;

	while (_kbhit() != 27)
	{
		result = selecter->ShowSelectPanel();		// 获取玩家选择结果

		if (!isCustomMap)
		{
			if (control != NULL)
				delete control;
			control = new GameControl(des_hdc, canvas_hdc);
		}

		switch (result)
		{
			case OnePlayer:
				control->AddPlayer(ONE_PLAYER);		// 一个玩家

				if (isCustomMap)
					control->GameLoop();
				else
				{
					control->LoadMap();
					control->GameLoop();
				}
				isCustomMap = false;		// 自定义地图关卡过后自动进行后面的关卡
				break;

			case TwoPlayer:
				control->AddPlayer(TWO_PLAYER);		// 两个玩家

				if (isCustomMap)
					control->GameLoop();
				else
				{
					control->LoadMap();
					control->GameLoop();
				}
				isCustomMap = false;
				break;

			case Custom:							// 玩家自定义地图
				control->CreateMap(&isCustomMap);
				break;

			default:
				return;
		}
	}
	 
	closegraph();
}

/*
* _WALL 被消除后标志为 _CLEAR = -1;
*/