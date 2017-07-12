#pragma once

#define TOTAL_ENEMY_NUMBER	20

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