#pragma once
#include "PlayerBase.h"
#include "EnemyBase.h"
#include "List.h"

struct Map
{
	char buf[26][27];
};

/* 类功能:
* 从 main 接收主窗口绘图接口
* 定义一个 IMAGE 画布, 绘制地图/敌机列表信息/旗子/关卡
* 存储格子标记
* 存储玩家链表
* 进行游戏循环
*/

class GameControl
{
public:
	GameControl( HDC des_hdc, HDC image_hdc/*, BoxMarkStruct* */);
	~GameControl();
	void Init();
	void AddPlayer(int player_num);					// 加载玩家进该类控制
	void LoadMap();									// 读取 .map 地图文件|解析绘制地图
	bool StartGame();								// 更新绘制游戏各种东西, 返回 false 结束游戏

private:
	void AddEnemy();						// 游戏开始前 添加20 架敌机
	void SignBoxMark(int i, int j, int sign_val);	// 标记格子, LoadMap() 内调用
	void RefreshRightPanel();						// 刷新右边信息面板数据, 根据数据变化才调用更新
	void RefreshCenterPanel();						// 刷新中间游戏区域 208 * 208
	void CheckKillEnemy(PlayerBase*);		// 检测玩家是否击中敌机, 击中则删除该敌机
	void CheckKillPlayer(list<EnemyBase*>::iterator);					// 
	//void SignBox_8(int i, int j, int val);					// 根据参数左上角 8*8 格子的索引, 标记 四个 8*8 格子

public:
	static int mCurrentStage;						// [1-35] 当前关卡, SelectPanel 内使用, 本类内使用

private:
	// 窗口绘图接口
	//Graphics* mGraphics;
	IMAGE mCenterImage;				// 用于保持 mCenter_hdc 存在有效性.
	HDC mDes_hdc,					// 主窗口hdc, main 传入
		mImage_hdc,					// 256*224 的 hdc, main 传入
		mCenter_hdc;				// 208*208 的 hdc, 本类产生, 是游戏区域,分开,经常更新,坐标也容易设定

	BoxMarkStruct* mBoxMarkStruct;			// 存储格子标记
	//list<PlayerBase*> PlayerList;			// 存储玩家列表
	ListTable<PlayerBase*> PlayerList;
	//list<PlayerBase*>::iterator PlayerItor;	// 用于遍历 PlayerList

	list<EnemyBase*> EnemyList;				// 敌机列表
	//list<EnemyBase*>::iterator EnemyItor;	// 用于遍历 PlayerList

	IMAGE mGrayBackgroundImage;		// 游戏灰色背景图
	IMAGE mBlackBackgroundImage;	// 黑色背景图

	Map mMap;						// 存储地图数据结构
	IMAGE mStoneImage;				// 石头
	IMAGE mForestImage;				// 树林
	IMAGE mIceImage;				// 冰块
	IMAGE mRiverImage[2];			// 河流
	IMAGE mWallImage;				// 泥墙
	IMAGE mCamp[2];					// 大本营

	IMAGE mEnemyTankIcoImage;		// 敌机坦克图标
	IMAGE mFlagImage;				// 旗子
	IMAGE mBlackNumberImage;		// 0123456789 当前关卡数

	// 初始 mActiveEnemyTankNumber + mRemainEnemyTankNumber = 20
	int mActiveEnemyTankNumber;		// 当前活动的敌机数量, 最多显示 6 架
	int mRemainEnemyTankNumber;		// 余下未显示的敌机数量, 初始值=20, 出现一架敌机该值减一

	BlastStruct mBlast;				// 大本营爆炸
	bool mCampDie;					// 大本营被击中, 不再 Camp 贴图 

	//PropClass mProp;			// 道具类		

	bool mEnemyPause;			// 是否可以移动敌机
	int mEnemyPauseCounter;		// 敌机暂停计数
};