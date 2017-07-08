#pragma once
#include "TankClass.h"

/* 对应数值
* 	#defien _EMPTY		0
	#define _WALL		3
	#define _FOREST		1
	#define _ICE		2
	#define _RIVER		4
	#define _STONE		5
-------------------------------
* 对应坐标关系
box[0][0-25] : 第一行( y=0; x=[0-25] )
box[1][0-25] : 第二行( y=1; x=[0-25] )
...
box[y/BOX_SIZE][x/BOX_SIZE]
-------------------------------
*/
struct BoxMarkStruct
{
	int box_8[26][26];			// 8*8 格子的标记, 坦克移动,击中用该格子检测
	int box_4[52][52];			// 4*4 格子的标记, 墙被击中用该标记检测
};

/************* 玩家控制 *************
* 一个玩家实例化一个对象
* 各自管理自己的数据
*/

class PlayerBase
{
public:
	PlayerBase(byte player);						// player [0-1]
	~PlayerBase();
	void DrawPlayerTankIco( HDC );					// 绘制右侧面板的\2P\1P\坦克图标\剩余生命值
	void DrawPlayerTank( HDC );						// 绘制玩家坦克
	bool PlayerControl(BoxMarkStruct*);				// 玩家控制坦克移动

private:
	void ChangeDir(int new_dir);					// 更改方向
	bool CheckMoveable( byte dir, BoxMarkStruct*);	// 检测当前操作是否可以移动

private:
	byte player_id : 1;						// [0-1] 玩家
	PlayerTank* mPlayerTank;				// 坦克类
	//static BoxMarkStruct* mBms;			// 指向 main 文件内的标记结构

	IMAGE m12PImage;						// 1P\2P 图标
	int m12PImage_x, m12PImage_y;			// 图标坐标

	IMAGE mPlayerTankIcoImage;				// 玩家坦克图标
	int mPlayerTankIcoImage_x,				// 图标坐标
		mPlayerTankIcoImage_y;

	int mPlayerLife;						// 玩家生命, 默认3
	int mPlayerLife_x, mPlayerLife_y;		// 图标坐标
	IMAGE mBlackNumberImage;				// 0123456789 黑色数字

	int mTankX, mTankY;						// 坦克中心点坐标
	byte mPlayerTankLevel : 2;				// [0-3] 坦克级别,获得道具后升级坦克
	byte mTankDir : 2;						// 当前坦克方向0-左,1-上,2右,3-下
	static int mDevXY[4][2];				// 四个方向的偏移量

	///////////////// 待修改,修改成根据坦克级别,移动速度不同
	int mSpeed;								// mSpeed * mDevXY 得到运动速度
	//byte mKeyCounter : 2;					// 按键计数,多长间隔响应依次按键
};