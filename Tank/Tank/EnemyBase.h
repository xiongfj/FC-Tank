#pragma once
#include "TankClass.h"
#include "struct.h"
#include "TimeClock.h"

/************** 敌机坦克 ************
* 一个敌机实例化一个对象
* 提供敌机类别(是否是道具坦克), 敌机级别[0-3]
* GameControl 内敌机被消灭, 不会将其从 EnemyList 内移除, 因为敌机子弹可能还会在运动.游戏结束分数面板显示完后释放敌机链表资源
************************************/
class EnemyBase
{
public:
	EnemyBase(TANK_KIND kind, byte level, BoxMarkStruct*);	// kind[0-1]; level [0-4]
	virtual ~EnemyBase();
	void Init();

	Star_State ShowStar(const HDC& center_hdc, int& total );		// 显示闪烁四角星, true-正在显示, false-显示完毕
	void TankMoving(const HDC& center_hdc);		// 敌机移动
	virtual void DrawTank(const HDC&) {}			// 纯绘制坦克, 子类覆绘制
	void DrawBullet(const HDC&);			// 绘制子弹, 需要与子弹移动分开,
	bool ShootBullet();			// 发射子弹
	BulletShootKind BulletMoving( );			// 子弹移动
	void Bombing(const HDC&);
	virtual bool BeKill(bool killanyway);			// 敌机被消灭, 清除 SignBox 标记, 参数表示玩家获得地雷直接kill掉BigestTank
	bool Blasting(const HDC& );		// 显示坦克爆炸图, true 标识爆炸完,GameControl 获取返回值然后将该敌机删除

	/*由 GameControl 内设置, 设置 mPause, 然后 ShootBullet() 检测停止发射子弹*/
	static void SetPause(bool);

	int GetId();				// 返回敌机 id
	TANK_KIND GetKind();		// 返回敌机类型, 是否是道具坦克
	byte GetLevel();

private:
	void SignBox_8(int, int, int value);
	void SignBox_4(int x, int y, int value);		// 标记或取消 4*4 大小的格子为坦克;

	/*
	* 标记子弹头所在的一个 4 * 4 格子
	* 参数是子弹图片左上角坐标
	*/
	void SignBullet(int, int, byte dir, int val);

	bool CheckBox_8();	// 检测某个 box_8 是否可以放置坦克, 参数是 16*16 格子的中心点, 与坦克坐标规则相同
	bool CheckMoveable();			// 
	void RejustDirPosition();		// 重新定位坦克方向, 调正坦克位置, 保持在格子上
	BulletShootKind CheckBomb();				// 移动子弹
	void ShootWhat(int, int);		// 检测射中何物

	/*在TankMoving()内调用*/
	void ShootBack();		// 回头射击

protected:
	int mEnemyId;				// 区别敌机与敌机
	TANK_KIND mEnemyTankKind;		// 敌机类别, 道具坦克和普通坦克两种, [0-1]
	byte mEnemyTankLevel : 2;	// 敌机坦克4个级别 [0-3]
	bool mDied;					// 是否被被消灭, 被击中后设置为 true, 敌机检测改值不能移动
	BoxMarkStruct* bms;			// 指向格子标记结构, 由 GameControl 传递进来

	int mTankX, mTankY;			// 坦克坐标, 坦克的中心点
	byte mTankDir : 2;			// 坦克方向
	byte mTankImageIndex : 6;	// 坦克移动切换图片
	int mStep;					// 当前方向移动的步数, 一定步数后或者遇到障碍物变换方向并重新计算;
	static int mDevXY[4][2];	// 四个方向的偏移量
	int mSpeed[4];					// mSpeed * mDevXY 得到运动速度, 下标对应 mPlayerTankLevel, 不同级别速度不一样

	static bool mPause;				// 由 GameControl 控制, 然后在发射的时候检测这个值, 如果敌机被暂停则停止发射子弹
	//bool mTankNumberReduce;		// 当四角星开始, true-坦克总数减一,然后设该值=false, 只减一次

	BulletStruct mBulletStruct;
	int mShootCounter;		// 设置一个随机数, 随机计数差后发射子弹

	BombStruct mBombS;			// 爆炸结构体
	BlastStruct mBlast;			// 坦克爆炸结构
	StarClass mStar;			// 四角星闪烁类

	TimeClock mTankTimer;		// 敌机移动速度
	TimeClock mBulletTimer;		// 子弹移动速度
	TimeClock mShootTimer;		// 发射子弹频率
	TimeClock mBombTimer;		// 子弹爆炸速度
	//TimeClock mBlastTimer;		// 坦克爆炸速度

	TimeClock mShootBackTimer;	// 坦克回头射击,然后短距离再次变向

	IMAGE mScoreImage[4];			// 100\200.. 分, 敌机爆炸完后显示的
};

// 前三种普通坦克
class CommonTank : public EnemyBase
{
public:
	CommonTank(byte level, BoxMarkStruct* bm);
	~CommonTank();
	void DrawTank(const HDC&);				// 纯绘制坦克

	TankInfo* mTank;			// 灰色坦克
};

// 前三种道具坦克
class PropTank : public EnemyBase
{
public:
	PropTank(byte level, BoxMarkStruct* bm);
	~PropTank();
	void DrawTank(const HDC&);		// 纯绘制坦克

	TankInfo* mTank[2];				// 存储灰色和红色的坦克
	byte index_counter : 6;		// 下标索引变色
};

// 第四种最大坦克 (道具是红黄灰, 普通是绿黄灰)
class BigestTank : public EnemyBase
{
public:
	BigestTank(TANK_KIND kind, BoxMarkStruct* bm);
	~BigestTank();
	void DrawTank(const HDC&);	// 纯绘制坦克
	bool BeKill(bool killanyway);

	TankInfo* mTank[4];			// 灰,红黄,绿
	byte index_counter : 6;		// 下标索引变色
	int hp;				// 这种坦克需要击中四次才会爆炸
};
