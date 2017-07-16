#pragma once
#include "TimeClock.h"

#define WINDOW_WIDTH	512		// 窗口大小，可以由玩家修改，
#define WINDOW_HEIGHT	448
#define CANVAS_WIDTH	256		// 画布 image 大小，不会改变，左右绘图都在 image 上操作，然后一次性绘制到主窗口
#define CANVAS_HEIGHT	224
#define CENTER_WIDTH	208		// 中间黑色游戏区域
#define CENTER_HEIGHT	208
#define CENTER_X		16		// 黑色游戏区域相对左上角的坐标
#define CENTER_Y		9

// 标记, 用于 markxx[26][26]
#define _CLEAR		-1		// 标记障碍物被消除的格子, 用于扫描时与 0 区分开, 然后绘制黑色图片, 避免对 0 也进行多余操作
#define _EMPTY		0		// 空地
#define _FOREST		1		// 森林
#define _ICE		2		// 冰
// ------ 以上 < 3 坦克可以穿行
#define _WALL		3		// 墙
#define _RIVER		4		// 河流
#define _STONE		5		// 石头

#define PLAYER_SIGN	100
#define CAMP_SIGN	200			// 大本营标记
#define ENEMY_SIGN	1000		// 4*4 或 8*8 格子标记, 加上敌机自身 id再标记
#define STAR_SIGN	2000		// 敌机出现四角星标记, 此时敌机不能进来

// prop_8 标记
#define PROP_SIGN	3000		// 道具在 prop_8 的标记

#define BOX_SIZE					8		// 26*26 的格子
#define SMALL_BOX_SIZE				4		// 52*52 格子大小
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

#define GRAY_TANK	0		// 灰色坦克
#define RED_TANK	1		// 红色坦克
#define YELLOW_TANK	2		// 
#define GREEN_TANK	3


#define SHOOTABLE_X		-100	// 规定子弹坐标 x = -100 子弹可以发射

// 敌机子弹击中的障碍物的类型, 用在 EnemyBase::CheckBomb 返回值
enum EnemyBulletShootKind {None, Player_1 = PLAYER_SIGN, Player_2 = PLAYER_SIGN + 1, Other};

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
	int prop_8[26][26];			// 道具的格子标记
};


#define TOTAL_ENEMY_NUMBER	20
#define SHOWING_STAR	true		// 正在显示四角星
#define STOP_SHOW_STAR	false		// 四角星显示结束

// 指示敌机类型道具坦克和普通坦克
enum TANK_KIND { PROP, COMMON };

struct BulletStruct
{
	int x, y;					// 子弹坐标, 根据不同方向指定不同的位置代表 x,y, 指定 x=SHOOTABLE_X 的时候可以发射子弹
	int dir;					// 子弹方向
	int speed[4];				// 子弹速度, 根据坦克级别给予不同速度系数. 每次移动不能超过4个像素点!! 不然会跨越 4*4 的格子!!检测bug

	/* 如果玩家击中敌机,设置该值为敌机的id, GameControl 循环内检测该值, 然后删除该敌机
	* 如果是敌机击中玩家, 设置该值为玩家id, 游戏循环内再检测该值处理玩家数据*/
	int mKillId;				// 记录子弹击中的敌机/玩家 id

	static IMAGE mBulletImage[4];		// 图片
	static int mBulletSize[4][2];		// {{4,3},{3,4},{4,3},{3,4}} 尺寸: 上下-3*4 / 左右-4*3
	static int devto_tank[4][2];		// 规定子弹的坐标相对于tank中心点的偏移量
	static int devto_head[4][2];		// 规定子弹图片左上角相对于弹头坐标的偏移量;上下方向弹头坐标在弹头的右边;左右则在弹头的上面那个点
	static int bomb_center_dev[4][2];	// 爆炸中心相对于子弹头的偏移量
};

struct BombStruct
{
	static IMAGE mBombImage[3];				// 子弹爆炸图
	int mBombX, mBombY;						// 爆炸点中心坐标
	bool canBomb;							// 是否开始显示爆炸图片 flag
	int counter;						// 取模计数器, 多少次循环更换一张图片
};

class BlastStruct
{
public:
	BlastStruct();
	~BlastStruct() {}

	static IMAGE image[5];
	int blastx, blasty;			// 中心坐标, 32*32
	bool canBlast;
	int counter;				// 计数多少次换一张图片
};

class StarClass
{
public:
	StarClass();
	static IMAGE mStarImage[4];	// 四角星图片, 有玩家赋值
	int mStarIndexDev;			// 索引的变化量, -1, 1  -1是star由小变大, 1 是star由大变小
	byte mStarIndex : 2;		// 四角星下标索引变化规律 0-1-2-3-2-1-0-1-2-3-...
	int mStarCounter;			// 计数,多少次变更一次图像
	bool mIsOuted;				// 四角星消失的时候坦克出现, 停止播放四角星闪烁图

	// Enemy 专用! Player 用不到
	int mTankOutAfterCounter;	// 一个随机计数之后, 四角星开始闪烁,坦克出现
};

// 玩家坦克出现的时候显示的闪烁环状保护圈
class RingClass
{
public:
	RingClass();
	bool ShowRing(const HDC&, int centerx, int centery);		//
	void SetShowable();				// 设置可以显示环状, PlayerBase 内获取道具后调用
	static IMAGE image[2];
	bool canshow;			// 是否可以显示环状
	int index_counter;		// 变化下标索引
};

// 道具类型
#define ADD_PROP	0	// 加机
#define STAR_PROP	1	// 五角星
#define TIME_PROP	2	// 时钟
#define BOMB_PROP	3	// 地雷
#define SHOVEL_PRO	4	// 铲子
#define CAP_PROP	5	// 帽子

/*** 道具类,
* 在 PlayerBase 实例化成员指针
* PlayerBase 构造函数内调用该类一个函数传递 BoxMarkStuct* 过来
* 该类所有函数都在 PlayerBase 内调用
***/
class PropClass
{
	void SignPropBox(int val);			// 标记 prop_8 格子

public:
	PropClass();
	void ShowProp(const HDC&);			// GameControl 内循环检测该函数
	void StartShowProp(int x, int y);	// 道具坦克被击中后调用该函数
	void StopShowProp();			// 停止显示道具, 超时或者被玩家获得
	void SetBoxMarkStruct(BoxMarkStruct*);

private:
	BoxMarkStruct* bms;
	int leftx, topy;			// 道具中心点坐标
	int index_counter = 0;			// 下标变换索引
	/*static */IMAGE image[6];
	/*static */int prop_kind;			// 道具类型
	bool can_show;				// 是否可以显示道具
	//TimeClock mTimer;
};