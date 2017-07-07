#pragma once
#include "TankClass.h"

/* 对应数值
* 	#define _WALL		'1'
	#define _FOREST		'2'
	#define _ICE		'3'
	#define _RIVER		'4'
	#define _STONE		'5'
*/
struct BoxMarkStruct
{
	char box_12[26][26];		// 8*8 格子的标记
	char box_6[52][52];			// 4*4 格子的标记
};

/************* 玩家控制 *************
* 一个玩家实例化一个对象
* 各自管理自己的数据
*/

class PlayerBase
{
public:
	PlayerBase(byte player);				// player [0-1]
	~PlayerBase();
	void DrawPlayerTankIco( Graphics* );	// 绘制玩家左下坦克图标
	void DrawPlayerTank( Graphics* );		// 绘制玩家坦克
	bool PlayerControl( BoxMarkStruct* );	// 玩家控制坦克移动
private:
	byte player_id : 1;						// [0-1] 玩家
	PlayerTank* mPlayerTank;				// 坦克类

	Image* m12PImage;						// 1P\2P 图标
	int m12PImage_x, m12PImage_y;			// 图标坐标

	Image* mPlayerTankIcoImage;				// 玩家坦克图标
	int mPlayerTankIcoImage_x,				// 图标坐标
		mPlayerTankIcoImage_y;

	int mPlayerLife;						// 玩家生命, 默认3
	int mPlayerLife_x, mPlayerLife_y;		// 图标坐标
	Image* mBlackNumberImage;				// 0123456789 黑色数字

	int mTankX, mTankY;						// 玩家控制的坦克坐标
	byte mPlayerTankLevel : 2;				// [0-3] 坦克级别,获得道具后升级坦克
	byte mTankDir : 2;						// 当前坦克方向0-左,1-上,2右,3-下
	static int mDevXY[4][2];				// 四个方向的偏移量

	///////////////// 待修改,修改成根据坦克级别,移动速度不同
	int mSpeed;								// mSpeed * mDevXY 得到运动速度
	//byte mKeyCounter : 2;					// 按键计数,多长间隔响应依次按键
};