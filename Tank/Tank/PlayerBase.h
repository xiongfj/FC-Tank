#pragma once
#include "TankClass.h"
#include "struct.h"
#include "TimeClock.h"

/************* 玩家控制 *************
* 一个玩家实例化一个对象
* 各自管理自己的数据
*/

class PlayerBase
{
public:
	PlayerBase(byte player, BoxMarkStruct* /*, PropClass**/);						// player [0-1]
	~PlayerBase();

	void Init();

	/*
	* GameControl 内循环调用
	*/
	void DrawPlayerTankIco(const HDC& );

	/* 玩家坦克闪烁出现*/
	bool ShowStar(const HDC& center_hdc);

	/*
	* GameControl 内循环调用
	* 纯绘制坦克操作
	*/
	void DrawPlayerTank( const HDC& );

	/*绘制子弹,需要与移动分开, 不然主循环内会失帧, 因为不一定每次绘图都会移动*/
	void DrawBullet(const HDC&);

	/*
	* GameControl 内循环调用
	* 检测按键
	*/
	bool PlayerControl();

	/*
	* GameControl 内循环调用,
	* mBulletStruct[i].x = SHOOTABLE_X 子弹才运动
	*/
	BulletShootKind BulletMoving(const HDC&);

	/*
	* GameControl 内循环调用
	* 函数内是否显示爆炸效果由 检测子弹击中 的函数决定
	*/
	void Bombing(const HDC&);

	/*
	* GameControl 内调用, 通过参数获取被击中的敌机 id
	* a : 1 号子弹击中敌机的 id
	* b : 2 号子弹击中敌机的 id
	*/
	void GetKillEnemy(int& a, int& b);

	/*
	* GameControl 内循环检测该值
	* 检测玩家是否击中大本营
	* 函数内的成员 mIsShootCamp 在检测子弹击中的函数中赋值
	*/
	bool IsShootCamp();

	/*
	* 在 GameControl 内检测调用
	* GameControl 内循环调用 EnemyBase->IsShootToPlayer() 获取被击中玩家的 id
	* 然后调用对应玩家的 BeKill 函数
	* BeKill() 控制爆炸图显示
	*/
	void BeKill();

	/*
	* GameControl 内循环调用
	* BeKill 被调用后设置一个 flag = true,
	* 该函数检测这个 flag 然后显示爆炸图
	*/
	bool Blasting(const HDC&);

	int GetID();

	/*GameControl 内调用, 检测玩家是否获得时间道具*/
	static bool IsGetTimeProp();
	bool IsGetBombProp();

	/*在 gameControl 内循环调用 检测并显示闪烁道具*/
	static void ShowProp(const HDC&);

	/*GameCnotrol 内循环调用, 函数内调用成员ScorePanel 函数显示结果*/
	bool ShowScorePanel(const HDC&);

	/*GameControl 内 IsGameOver() 内调用*/
	//void SendKillNumToScorePanel();

	/*GameControl 内检测子弹击中结果判断调用,
	设置玩家暂停移动*/
	void SetPause();

	/*
	* GameControl 内 CheckKillEnemy() 内调用
	* 如果成功消灭道具敌机, 就显示道具
	*/
	static void SetShowProp();

	/*GameControl 内CheckKillEnemy() 内调用,
	记录玩家消灭不同级别类型的敌机的数量*/
	void AddKillEnemyNum(byte enemy_level);

	/*GameControl 内每一关结束显示分数面板的时候调用*/
	void ResetScorePanelData();
private:

	/*如果有生命重新出生*/
	void Reborn();

	/*获取到道具, 坦克移动内检测调用*/
	void DispatchProp(int prop_kind);
	/*
	* PlayerControl() 内调用
	* 判断并更改方向或者移动
	* 同时调整坐标到格子整数处, 
	*/
	void Move(int new_dir);	

	/*
	* Move() 内调用
	* 检测当前方向移动的可能性
	*/
	bool CheckMoveable();

	/*
	* PlayerControl() 内调用
	* 发射 id 号子弹[0,1]
	* 按下 J 或 1 键调用
	*/
	bool ShootBullet(int bullet_id);

	/*
	* BulletMoving() 内调用
	* 检测子弹移动过程是否击中东西
	*/
	BulletShootKind CheckBomb(int);

	/*
	* CheckBomb() 内击中障碍物时调用
	* id  : 子弹 id
	* x,y : 子弹凸出点的坐标, 根据不同方向位置不一样
	*/
	void ClearWallOrStone(int id, int x, int y);

	/*
	* 标记 4 个 8*8 的格子
	* x,y : 16*16 格子的中心点坐标
	* val : 需要标记的值
	*/
	void SignBox_8(int x, int y, int val);
	void SignTank_8(int, int, int);

	// 根据坦克中心坐标, 标记16个 4*4 格子
	void SignBox_4(int, int, int);

	/*
	* 坦克重生时调用检测重生位置是否被敌机占用
	* 检测某个中心位置可以放置坦克与否
	* 参数必须是 8*8 格子线上的点
	*/
	bool CheckBox_4(int cx, int cy);

private:
	bool mDied;								// 生命是否用完,死亡
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

	BulletStruct mBulletStruct[2];			// 两颗子弹
	int mBullet_1_counter;					// 子弹 1 的计数, 子弹 1 发射多久后才能发射子弹 2
	bool mMoving;							// 指示坦克是否移动, 传递到 GetTankImage() 获取移动的坦克

	BombStruct mBombS[2];					// 爆炸结构体
	BlastStruct mBlast;				// 坦克爆炸结构, 

	bool mIsShootCamp;				// 是否击中大本营

	StarClass mStar;				// 坦克出现前的四角星闪烁
	RingClass mRing;				// 保护圈类

	static PropClass* mProp;		// 道具类

	/*GameControl 内循环检测该值, 然后在设置 EnemyBase 停止移动*/
	static bool mTimeProp;			// 记录是否获得 时钟道具
	bool mBombProp;				// 地雷道具, 逻辑与 mTimeProp 相同
		
	TimeClock mTankTimer;			// 坦克移动计时器
	TimeClock mBulletTimer;			// 子弹移动速度
	TimeClock mBombTimer;	//bug?		// 子弹爆炸速度
	TimeClock mBlastTimer;			// 坦克爆炸速度

	ScorePanel* mScorePanel;		// 关卡结束后显示分数结果

	int mKillEnemyNumber[4];		// 四种敌机类型,杀敌数量

	bool mPause;			// 被另外一个玩家击中, 暂停一段时间
	int mPauseCounter;		// Move() 内计数停止移动, DrawTank() 内取模计数,实现坦克闪烁

	bool mHasSendKillNumToScorePanel;		// 放置 胜利后集中大本营或其它.多次发送数据到分数面板
};