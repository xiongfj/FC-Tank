#pragma once
#include "TankClass.h"

#define SHOOTABLE_X		-100	// 规定子弹坐标 x = -100 子弹可以发射
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

struct BulletStruct
{
	int x, y;					// 子弹坐标, 根据不同方向指定不同的位置代表 x,y
	int dir;					// 子弹方向
	int speed;					// 子弹速度
	IMAGE mBulletImage[4];		// 图片
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
	void BulletMoving(HDC);							// 子弹移动, 在GameControl 内循环调用

private:
	void ChangeDir(int new_dir);					// 更改方向, 同时调整坐标到格子整数处
	bool CheckMoveable( byte dir, BoxMarkStruct*);	// 检测当前操作是否可以移动
	bool ShootBullet(int bullet_id);				// 发射 id 号子弹[0,1]

private:
	byte player_id : 1;						// [0-1] 玩家
	PlayerTank* mPlayerTank;				// 坦克类

	IMAGE m12PImage;						// 1P\2P 图标
	int m12PImage_x, m12PImage_y;			// 图标坐标

	IMAGE mPlayerTankIcoImage;				// 玩家坦克图标
	int   mPlayerTankIcoImage_x,			// 图标坐标
		  mPlayerTankIcoImage_y;

	int mPlayerLife;						// 玩家生命, 默认3
	int mPlayerLife_x, 						// 图标坐标
		mPlayerLife_y;

	IMAGE mBlackNumberImage;				// 0123456789 黑色数字
	int mTankX, mTankY;						// 坦克中心点坐标
	byte mPlayerTankLevel : 2;				// [0-3] 坦克级别,获得道具后升级坦克
	byte mTankDir : 2;						// 当前坦克方向0-左,1-上,2右,3-下
	static int mDevXY[4][2];				// 四个方向的偏移量
	int mSpeed[4];							// mSpeed * mDevXY 得到运动速度, 下标对应 mPlayerTankLevel, 不同级别速度不一样

	static IMAGE mBulletImage[4];			// 四个方向子弹, 
	static int mBulletSize[4][2];			// {{4,3},{3,4},{4,3},{3,4}} 尺寸: 上下-3*4 / 左右-4*3
	int mBulletX[2], mBulletY[2];			// 子弹坐标, 一个玩家两对; 3/4级别的坦克可以发射两颗子弹
	int mBulletDir[2];						// 子弹方向
};