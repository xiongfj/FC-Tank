#pragma once
#include "TankClass.h"

#define SHOOTABLE_X		-100	// 规定子弹坐标 x = -100 子弹可以发射

struct BulletStruct
{
	int x, y;					// 子弹坐标, 根据不同方向指定不同的位置代表 x,y, 指定 x=SHOOTABLE_X 的时候可以发射子弹
	int dir;					// 子弹方向
	int speed[4];				// 子弹速度, 根据坦克级别给予不同速度系数
	
	static IMAGE mBulletImage[4];		// 图片
	static int mBulletSize[4][2];		// {{4,3},{3,4},{4,3},{3,4}} 尺寸: 上下-3*4 / 左右-4*3
	static int devto_tank[4][2];		// 规定子弹的坐标相对于tank中心点的偏移量
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
	void DrawPlayerTankIco(const HDC& );					// 绘制右侧面板的\2P\1P\坦克图标\剩余生命值
	void DrawPlayerTank( const HDC& );						// 绘制玩家坦克
	bool PlayerControl(BoxMarkStruct*);				// 玩家控制坦克移动
	void BulletMoving(const HDC&);							// 子弹移动, 在GameControl 内循环调用

private:
	void Move(int new_dir, BoxMarkStruct*);			// 更改方向, 或移动. 同时调整坐标到格子整数处, 
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

	//static IMAGE mBulletImage[4];			// 四个方向子弹, 
	//static int mBulletSize[4][2];			// {{4,3},{3,4},{4,3},{3,4}} 尺寸: 上下-3*4 / 左右-4*3
	//int mBulletX[2], mBulletY[2];			// 子弹坐标, 一个玩家两对; 3/4级别的坦克可以发射两颗子弹
	//int mBulletDir[2];					// 子弹方向

	
	BulletStruct mBulletStruct[2];			// 两颗子弹
	int mBullet_1_counter;					// 子弹 1 的计数, 子弹 1 发射多久后才能发射子弹 2
	bool mMoving;							// 指示坦克是否移动, 传递到 GetTankImage() 获取移动的坦克

	IMAGE mBombImage[3];					// 子弹爆炸图
	int mBombX, mBombY;						// 爆炸点中心坐标
};