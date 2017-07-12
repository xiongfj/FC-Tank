#pragma once
#include "TankClass.h"
#include "struct.h"

/************* 玩家控制 *************
* 一个玩家实例化一个对象
* 各自管理自己的数据
*/

class PlayerBase
{
public:
	PlayerBase(byte player, BoxMarkStruct*);						// player [0-1]
	~PlayerBase();
	void PlayerLoop(const HDC&);							// 玩家循环
	void DrawPlayerTankIco(const HDC& );					// 绘制右侧面板的\2P\1P\坦克图标\剩余生命值
	void DrawPlayerTank( const HDC& );						// 绘制玩家坦克
	bool PlayerControl();				// 玩家控制坦克移动
	void BulletMoving(const HDC&);							// 子弹移动, 在GameControl 内循环调用
	void Bombing(const HDC&);							// 爆炸
	void GetKillEnemy(int&, int&);						// GameControl 内调用, 通过参数将 mBulletStruct.mKillId 传递进去
	bool IsShootCamp();						// 玩家是否击中大本营

private:
	void Move(int new_dir);					// 更改方向, 或移动. 同时调整坐标到格子整数处, 
	bool CheckMoveable( byte dir);			// 检测当前操作是否可以移动
	bool ShootBullet(int bullet_id);			// 发射 id 号子弹[0,1]
	bool CheckBomb(int);						// 检测可否爆炸
	void ClearWallOrStone(int, int,int);				// 擦除墙或者石头
	void SignBox_8(int x, int y, int val);						// 标记坦克所在的 8*8 的格子

private:
	byte player_id : 1;						// [0-1] 玩家
	PlayerTank* mPlayerTank;				// 坦克类
	BoxMarkStruct* bms;

	IMAGE m12PImage;						// 1P\2P 图标
	int m12PImage_x, m12PImage_y;			// 图标坐标

	IMAGE mPlayerTankIcoImage;				// 玩家坦克图标
	int   mPlayerTankIcoImage_x,			// 图标坐标
		  mPlayerTankIcoImage_y;

	int mPlayerLife;						// 玩家生命, 默认3
	int mPlayerLife_x, 						// 图标坐标
		mPlayerLife_y;

	IMAGE mBlackNumberImage;				// 0123456789 黑色数字
	int mTankX, mTankY;					// 坦克中心点坐标
	byte mPlayerTankLevel : 2;				// [0-3] 坦克级别,获得道具后升级坦克
	byte mTankDir : 2;						// 当前坦克方向0-左,1-上,2右,3-下
	static int mDevXY[4][2];				// 四个方向的偏移量
	int mSpeed[4];							// mSpeed * mDevXY 得到运动速度, 下标对应 mPlayerTankLevel, 不同级别速度不一样

	//static IMAGE mBulletImage[4];			// 四个方向子弹, 
	//static int mBulletSize[4][2];			// {{4,3},{3,4},{4,3},{3,4}} 尺寸: 上下-3*4 / 左右-4*3
	//int mBulletX[2], mBulletY[2];			// 子弹坐标, 一个玩家两对; 3/4级别的坦克可以发射两颗子弹
	//int mBulletDir[2];					// 子弹方向

	BulletStruct mBulletStruct[2];			// 两颗子弹
	//int mKillEnemyId[2];					// 记录每个子弹击中的敌机 id, #define _NONE 标识没击中
	int mBullet_1_counter;					// 子弹 1 的计数, 子弹 1 发射多久后才能发射子弹 2
	bool mMoving;							// 指示坦克是否移动, 传递到 GetTankImage() 获取移动的坦克

	BombStruct mBombS[2];					// 爆炸结构体
	BlastStruct mBlast[2];				// 坦克爆炸结构, 可以两架坦克同时爆炸,所有要两个

	bool mIsShootCamp;					// 是否击中大本营
};