
// TODO: 在此处引用程序需要的其他头文件
#include "./easyx/graphics.h"
#include <list>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <Mmsystem.h>
#pragma comment ( lib, "Winmm.lib" )
using namespace std;

// GDI
#include <wingdi.h>
#pragma comment (lib, "msimg32.lib")

// GDI+ 
#ifndef ULONG_PTR
#define ULONG_PTR unsigned long*
#include <comdef.h>
#include "./gdi+/Includes/GdiPlus.h"
#include "./gdi+/Includes/GdiPlusInit.h"
using namespace Gdiplus;
#pragma comment ( lib, "./gdi+/Lib/GdiPlus.lib" )
#endif

#define WINDOW_WIDTH	256		// 窗口大小，可以由玩家修改，
#define WINDOW_HEIGHT	224
#define CANVAS_WIDTH	256		// 画布 image 大小，不会改变，左右绘图都在 image 上操作，然后一次性绘制到主窗口
#define CANVAS_HEIGHT	224
#define CENTER_WIDTH	208		// 中间黑色游戏区域
#define CENTER_HEIGHT	208
#define CENTER_X		16		// 黑色游戏区域相对左上角的坐标
#define CENTER_Y		9

// 标记, 用于 markxx[26][26]
#define _EMPTY		0		// 空地
#define _FOREST		1		// 森林
#define _ICE		2		// 冰
							// ------ 以上 < 3 坦克可以穿行
#define _WALL		3		// 墙
#define _RIVER		4		// 河流
#define _STONE		5		// 石头

#define BOX_SIZE					8		// 26*26 的格子
#define BLACK_NUMBER_SIZE			7		// 黑色数字大小

#define ENEMY_TANK_ICO_SIZE			7		// 敌机图标大小
#define PLAYER_TANK_ICO_SIZE_X		7		// 玩家图标大小
#define PLAYER_TANK_ICO_SIZE_Y		8
#define PLAYER_12_ICO_SIZE_X		14		// 1P\2P 图标大小
#define PLAYER_12_ICO_SIZE_Y		7
#define FLAG_ICO_SIZE_X				16		// 旗子大小
#define FLAG_ICO_SIZE_Y				15
#define PLAYER_LIFE_NUMBER_SIZE		7		// 玩家生命数字大小

#define DIR_LEFT	0
#define DIR_UP		1
#define DIR_RIGHT	2
#define DIR_DOWN	3
