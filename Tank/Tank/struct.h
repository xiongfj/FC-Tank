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

// GameOver 字样大小
#define GAMEOVER_WIDTH	31
#define GAMEOVER_HEIGHT	15

// <<box_8>>, <<box_4>> 同时标记
#define _CLEAR		-1		// 标记障碍物被消除的格子, 用于扫描时与 0 区分开, 然后绘制黑色图片, 避免对 0 也进行多余操作
#define _EMPTY		0		// 空地
#define _FOREST		1		// 森林
#define _ICE		2		// 冰
// ------ 以上 < 3 坦克可以穿行
#define _WALL		3		// 墙
#define _RIVER		4		// 河流
#define _STONE		5		// 石头

//* <<box_8 标记>> */
#define CAMP_SIGN	200			// 大本营标记
#define STAR_SIGN	2000		// 敌机出现四角星标记, 此时敌机不能进来

/* <<box_4 标记>> */
#define PLAYER_SIGN	100

/*  <<box_4 标记>> 
* 重新定义规则:
	规定标记是 10000 + 1000*mEnemyTankLevel + 100*menemyKind + enemy_id 
*/
#define ENEMY_SIGN	10000		// 4*4 或 8*8 格子标记, 加上敌机自身 id再标记

/* 
* <<bullet_4 标记>>
* 敌机子弹标记
* box_4[i][j] = E_B_SIGN + enemy_id
* 只标记弹头坐标所在的那一个 4*4 格子
*/
#define E_B_SIGN	300

/* 
* <<bullet_4>> 标记
* 玩家子弹标记
* bullet_4[i][j] = P_B_SIGN + player_id*10 + bullet_id 
* 只标记弹头坐标所在的那一个 4*4 格子
*/
#define P_B_SIGN	400

/*
* <<bullet_4>> 标记
子弹击中子弹,将自身所在box_4 标记为 wait_unsign, 等待对方擦除
*/
#define WAIT_UNSIGN	444

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

// 子弹击中的障碍物的类型, 用在 EnemyBase::CheckBomb 返回值
enum BulletShootKind {None, Player_1 = PLAYER_SIGN, Player_2 = PLAYER_SIGN + 1, Camp, Other};

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
	int bullet_4[52][52];		// 子弹层标记
};

#define TOTAL_ENEMY_NUMBER	20
#define SHOWING_STAR	true		// 正在显示四角星
#define STOP_SHOW_STAR	false		// 四角星显示结束

// 指示敌机类型道具坦克和普通坦克
enum TANK_KIND { PROP, COMMON };

struct BulletStruct
{
	int x, y;					// 子弹图片左上角坐标, 根据不同方向指定不同的位置代表 x,y, 指定 x=SHOOTABLE_X 的时候可以发射子弹
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

// 坦克爆炸状态
enum BlastState {Blasting, BlastEnd, NotBlast};
class BlastStruct
{
public:
	BlastStruct();
	~BlastStruct() {}
	void Init();

	BlastState CampBlasting(const HDC&);

	/*PlayerBase 内调用, 循环检测坦克爆炸
	爆炸完毕返回 BLAST_END
	其余情况返回 */
	BlastState Blasting(const HDC&);

	/*敌机爆炸不一样, 还要显示分数*/
	BlastState EnemyBlasting(const HDC&, IMAGE*);

	/*设置开始爆炸标志 和 传递坐标*/
	void SetBlasting(int tankx, int tanky);

	/*返回当前是否否在爆炸*/
	bool IsBlasting();


	static IMAGE image[5];
	int blastx, blasty;			// 中心坐标, 32*32
	bool canBlast;
	int counter;				// 计数多少次换一张图片

	TimeClock timer;
};


// 敌机或者玩家的四角星闪烁返回类型
enum Star_State { 
	Star_Timing,		// 只用于敌机, 表示当前还没有出现四角星, 还在计时之中
	Star_Failed,		// 用于敌机, 表示当前位置敌机出现失败
	Star_Out,			// 四角星刚刚出现
	Star_Showing,		// 正在闪烁四角星
	Star_Stop,			// 刚刚停止闪烁
	Tank_Out };			// 坦克已经出现

class StarClass
{
public:
	StarClass();
	void Init();

	/*PlayerBase  内调用, 参数是坦克坐标*/
	Star_State ShowStar(const HDC&, int, int );
	bool IsStop();

	/*EnemyBase 内调用*/
	Star_State EnemyShowStar(const HDC&, int, int, const BoxMarkStruct* );

	static IMAGE mStarImage[4];	// 四角星图片, 有玩家赋值
	int mStarIndexDev;			// 索引的变化量, -1, 1  -1是star由小变大, 1 是star由大变小
	byte mStarIndex : 2;		// 四角星下标索引变化规律 0-1-2-3-2-1-0-1-2-3-...
	int mStarCounter;			// 计数,多少次变更一次图像
	bool mIsOuted;				// 四角星消失的时候坦克出现, 停止播放四角星闪烁图

	// Enemy 专用! Player 用不到
	int mTankOutAfterCounter;	// 一个随机计数之后, 四角星开始闪烁,坦克出现
	bool mStarOuted;		// 四角星刚出现, 只用于敌机
};

// 玩家坦克出现的时候显示的闪烁环状保护圈
class RingClass
{
public:
	RingClass();
	void Init();
	bool ShowRing(const HDC&, int centerx, int centery);		//

	/*设置可以显示环状, PlayerBase 内获取道具后调用
	参数设置显示多久, 出生显示较短, 获得道具后显示较长*/
	void SetShowable(long);				// 

	static IMAGE image[2];
	bool canshow;			// 是否可以显示环状
	int index_counter;		// 变化下标索引

	TimeClock timer;
};

// 道具类型
#define ADD_PROP	0	// 加机
#define STAR_PROP	1	// 五角星
#define TIME_PROP	2	// 时钟
#define BOMB_PROP	3	// 地雷
#define SHOVEL_PROP	4	// 铲子
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
	void Init(BoxMarkStruct * b);
	void ShowProp(const HDC&);			// GameControl 内循环检测该函数

	/*参数是左上角的坐标*/
	void StartShowProp(int x, int y);	// 道具坦克被击中后调用该函数
	void StopShowProp(bool);			// 停止显示道具, 超时或者被玩家获得, 参数指示是玩家获得还是超时

private:
	int score_counter;		// 计数显示多久
	bool show_score;
	IMAGE score;				// 500 分数

	BoxMarkStruct* bms;
	int leftx, topy;			// 道具中心点坐标
	int index_counter = 0;			// 下标变换索引
	/*static */IMAGE image[6];
	/*static */int prop_kind;			// 道具类型
	bool can_show;				// 是否可以显示道具
	//TimeClock mTimer;
};

/*
* 每一关胜利\失败后显示的分数面板
* PlayerBase 内实例化
* 一个玩家一个对象
*/
class ScorePanel
{
public:
	ScorePanel(int player_id);
	~ScorePanel();
	bool show(const HDC&);

	/*在PlayerBase 内调用, PlayerBase又在GameControl 内调用
	* 传递杀敌数, 玩家数
	*/
	void ResetData(const int * nums, const int&, const int& stage);				// 每次显示前需要重置

	static IMAGE background;

private:

	static IMAGE yellow_number;
	static IMAGE number;
	IMAGE player;
	IMAGE pts;
	static IMAGE bunds;		// bunds 1000pts 字样

	// 检测哪个玩家分数多,就显示在哪边
	static int who_bunds[2];			// 显示在玩家一还是玩家二处

	static int player_num;		// 玩家数; 初始化一个本类对象 +1
	static bool line_done_flag[2];	// 每一行数据两个玩家都显示完与否

	byte player_id;
	int player_x, player_y;
	int pts_x, pts_y;
	int kill_num[4], kill_num2[4];		// 4种敌机杀敌数, kill_num = -1 的项先不显示

	// x[..][0] 是分数, x[..][1] 是杀敌数
	int x[4][2];	// 一个是分数, 一个是杀敌数, 分数等于 杀敌数*100,200,400...
	int y[4][2];

	static int cur_line;		// 当前正在更新显示的数据行索引

	int total_kill_numm;		// 总杀敌数
	int total_kill_x, total_kill_y;	// 坐标

	static int end_counter;		// 分数面板显示完全后多久跳转

	int total_score_x, total_score_y;
	int total_score;			// 总分
	int stage;
};

/*某个玩家被消灭单独显示的 gameover 字样*/
class PlayerGameover
{
public:
	PlayerGameover();
	~PlayerGameover() {}
	void Init(int p_id);
	void SetShow();
	void Show(const HDC& center_hdc);

	TimeClock mGameoverTimer;		//
	int mGameover_Dev;			// X 轴移动, 玩家一是正数分量, 玩家二是负数分量
	int mGameoverX, mGameoverY;
	int mGameover_end_x;			// 图片停止的 x 坐标
	static IMAGE mGameoverImage;		// 玩家生命值用完, 显示一个水平移动的 GAMEOVER 字样
	int mGameoverCounter;			// 图片停止后多久消失
	bool mShowGameover;
};