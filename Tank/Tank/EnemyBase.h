#pragma once
#include "TankClass.h"
#include "struct.h"

/************** 敌机坦克 ************
* 一个敌机实例化一个对象
* 提供敌机类别(是否是道具坦克), 敌机级别[0-4]
* 默认生成 0-0 敌机, 即0级的道具坦克
************************************/
class EnemyBase
{
public:
	EnemyBase(TANK_KIND kind, byte level, BoxMarkStruct*);	// kind[0-1]; level [0-4]
	virtual ~EnemyBase();
	bool ShowStar(const HDC& center_hdc, int& total );		// 显示闪烁四角星, true-正在显示, false-显示完毕
	void TankMoving(const HDC& center_hdc);		// 敌机移动
	virtual void DrawTank(const HDC&) {}			// 纯绘制坦克
	bool ShootBullet();			// 发射子弹
	void BulletMoving(const HDC& );			// 子弹移动
	void Bombing(const HDC&);
	void BeKill();					// 敌机被消灭, 清除 SignBox 标记
	bool Blasting(const HDC& );		// 显示坦克爆炸图, true 标识爆炸完,GameControl 获取返回值然后将该敌机删除
	bool IsShootCamp();				// 是否击中大本营, GeamControl 内循环检测
	int  IsShootToPlayer();			// 获取被击中的玩家 id

	int GetId();				// 返回敌机 id
	TANK_KIND GetKind();		// 返回敌机类型, 是否是道具坦克

	/*  GameControl 内循环检测 PlayerBase 类内的一个值
	* 根据该值判断调用该函数
	* 函数内设置 flag = true, 所有敌机停止移动
	* 在 GameControl 内计数, 计数完成后设置为 false 敌机开始移动
	*/
	//static void SetPause(bool);			// 敌机暂停移动


private:
	void SignBox_4(int value);		// 标记或取消 4*4 大小的格子为坦克;
	void SignBox_8(int, int, int value);
	bool CheckBox_8();	// 检测某个 box_8 是否可以放置坦克, 参数是 16*16 格子的中心点, 与坦克坐标规则相同
	bool CheckMoveable();			// 
	void RejustDirPosition();		// 重新定位坦克方向, 调正坦克位置, 保持在格子上
	bool CheckBomb();				// 移动子弹
	void ShootWhat(int, int);		// 检测射中何物

protected:
	int mEnemyId;				// 区别敌机与敌机
	TANK_KIND mEnemyTankKind;		// 敌机类别, 道具坦克和普通坦克两种, [0-1]
	byte mEnemyTankLevel : 2;	// 敌机坦克4个级别 [0-3]
	bool mDied;					// 是否被被消灭, 被击中后设置为 true, 敌机检测改值不能移动
	bool mIsShootCamp;			// 是否击中大本营
	//TankInfo* mEnemyTank;		// 指向坦克详细信息
	BoxMarkStruct* bms;			// 指向格子标记结构, 由 GameControl 传递进来

	int mTankX, mTankY;			// 坦克坐标, 坦克的中心点
	byte mTankDir : 2;			// 坦克方向
	byte mTankImageIndex : 1;	// 坦克移动切换图片
	int mStep;					// 当前方向移动的步数, 一定步数后或者遇到障碍物变换方向并重新计算;
	static int mDevXY[4][2];	// 四个方向的偏移量
	int mSpeed[4];					// mSpeed * mDevXY 得到运动速度, 下标对应 mPlayerTankLevel, 不同级别速度不一样

	bool mTankNumberReduce;		// 当四角星开始, true-坦克总数减一,然后设该值=false, 只减一次

	BulletStruct mBulletStruct;
	DWORD mBulletT1, mBulletT2;	// 计数器, 多久发射一次子弹
	DWORD mBulletT;

	BombStruct mBombS;			// 爆炸结构体
	BlastStruct mBlast;			// 坦克爆炸结构

	int mShootedPlayerID;		// 被击中玩家的id
	StarClass mStar;			// 四角星闪烁类
	//static bool mIsPause;				// 玩家获取时钟道具, 通过 GameControl 传递设置该值
	//static int mPauseCounter;			// 暂停多久
};

// 前三种普通坦克
class CommonTank : public EnemyBase
{
public:
	CommonTank(byte level, BoxMarkStruct* bm);
	void DrawTank(const HDC&);				// 纯绘制坦克
	TankInfo* mTank;			// 灰色坦克
};

// 前三种道具坦克
class PropTank : public EnemyBase
{
public:
	PropTank(byte level, BoxMarkStruct* bm);
	void DrawTank(const HDC&);		// 纯绘制坦克
	TankInfo* mTank[2];				// 存储灰色和红色的坦克
	byte index_counter : 6;		// 下标索引变色
};

// 第四种最大坦克 (道具是红黄灰, 普通是绿黄灰)
class BigestTank : public EnemyBase
{
public:
	BigestTank(TANK_KIND kind, BoxMarkStruct* bm);
	void DrawTank(const HDC&);	// 纯绘制坦克
	TankInfo* mTank[4];			// 灰,红黄,绿
	byte index_counter : 6;		// 下标索引变色
};
