#pragma once


#define LENGTH		10										// 每个小方框占据大小, 4 * 4 个小方框组成一架坦克
#define NUM			100										// 敌机数量(待改为全局变量)

#define BLOCK		500										// 障碍物标记值,可被摧毁
#define STONE		501										// 磐石标记，子弹坦克都不能穿越
#define CAMP		502										// 鸟窝大本营标记
#define FOREST		503										// 森林标记，坦克隐身
#define RIVER		504										// 河流标记，坦克不能穿越，子弹可以。

#define UP		0
#define DOWN	1
#define LEFT	2
#define RIGHT	3

enum KIND { MASTER, ENEMY /*, _TURRET, _TRANSFER */};						// 主机、敌机、炮塔、传送阵

struct NEST												// 巢穴
{
	int Nx, Ny, Np;										// 主机巢穴坐标
	IMAGE Nimg[20];										// 储存旋转图片
	DWORD Nt1, Nt2, Ndt;								// 输出图片间隔，制作旋转效果

}Nest;

///////////////////////////////


struct TANK
{
	IMAGE Timg[4];										// 储存四个方向坦克
	IMAGE Bimg[4];										// 储存四个方向子弹

	int X, Y;											// 坦克左上角坐标
	byte DIR  : 2;										// 方向只用两位二进制表示，只能取0,1,2,3。
	KIND kind;											// 0 为主机，1 为敌机
	bool shoot, live;									// 
	//bool besend;										// 是否在被传送ing，传送过程无视伤害
	bool appear;										// 是否出现在屏幕

	DWORD T1, T2, DRCT;									// 敌机移动间隔，主机用不到!!!
	int step;											// 一定步后变向，主机用不到!!!
	DWORD Freq, Ft1, Ft2;								// 发射子弹间隔，主机用不到!!!

	int b_x, b_y;										// 子弹中心坐标
	byte b_r : 2;										// 子弹方向
	DWORD b_t1, b_t2, b_dt;								// 子弹移动速度

	static int DRCXY[4][2];								// 上下左右移动的分量
	static int T_Bult_of_Cent[4][2];					// 坦克到子弹中心的相对坐标
	static double Cent_Bult_of_LeUp[4][2];				// 子弹中心到子弹左上角的相对坐标
	static int HP;										// 主机生命 4

}Tank[NUM + 1];											// +1 主机
int TANK::DRCXY[4][2]					= { {0	 , -1}, {0	 ,  1}, {-1,	0}, { 1,	0} };	// 4 = 坦克 或 子弹的方向
int TANK::T_Bult_of_Cent[4][2]			= { {2	 ,  1}, {2	 ,  3}, { 1,	2}, { 3,	2} };	// 4 = 坦克的dir
double TANK::Cent_Bult_of_LeUp[4][2]	= { {-0.5, -1}, {-0.5, -1}, {-1, -0.5}, {-1, -0.5} };	// 4 = 子弹的方向
int TANK::HP = 4;

////////////////////////////////////////

struct GAME												// 当前游戏状态
{
	bool start, replay;
	int level;

}game;

////////////////////////////////////////

struct STATICPICTURE									// 静态图片区：森林、河流、障碍物、磐石
{
	IMAGE block, stone, forest, river;

}StaPic;

////////////////////////////////////////

struct LEVEL											// 当前级别信息：坦克总量、当前显示数量
{
	static int base_tot_num;							// 基础值，tot_num 在该基础上增加
	static int base_cur_num;							// 同上
	int next;											// 下一架坦克的 id
	int tot_num, cur_num;								// tot_num 不能大于 #define NUM !!!

}Level;
int LEVEL::base_tot_num = 30;
int LEVEL::base_cur_num = 4;
