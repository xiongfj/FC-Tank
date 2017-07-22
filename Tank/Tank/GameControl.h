#pragma once
#include "PlayerBase.h"
#include "EnemyBase.h"
#include "TimeClock.h"

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

** 成员数据:
- EnemyList: 存储被消灭的 and 正在移动 and 等待出现的敌机
*/

enum GameResult {Victory, Fail};		// 每一关胜利\失败

class GameControl
{
public:
	GameControl( HDC des_hdc, HDC image_hdc/*, BoxMarkStruct* */);
	~GameControl();
	void Init();
	void AddPlayer(int player_num);					// 加载玩家进该类控制
	void LoadMap();									// 读取 .map 地图文件|解析绘制地图
	bool CreateMap(bool* out);				// 玩家自定义地图
	void GameLoop();
	GameResult StartGame();								// 更新绘制游戏各种东西, 返回 false 结束游戏

private:
	void CutStage();		// 动画
	void ShowStage();				// 开始每一关之前显示 STAGE 2 之类的字样
	void ClearSignBox();
	void InitSignBox();				// 初始化格子标记
	void AddEnemy();						// 游戏开始前 添加20 架敌机
	void SignBox_4(int i, int j, int sign_val);	// 标记格子, LoadMap() 内调用
	bool RefreshData();
	void RefreshRightPanel();						// 刷新右边信息面板数据, 根据数据变化才调用更新
	void RefreshCenterPanel();						// 刷新中间游戏区域 208 * 208
	void CheckKillEnemy(PlayerBase*);		// 检测玩家是否击中敌机, 击中则删除该敌机
	void IsGameOver();				// 循环检测是否flag,  gameover
	void IsWinOver();				// 消灭所有敌机胜利过关

public:
	static int mCurrentStage;						// [1-35] 当前关卡, SelectPanel 内使用, 本类内使用

private:
	// 窗口绘图接口
	IMAGE mCenterImage;				// 用于保持 mCenter_hdc 存在有效性.
	HDC mDes_hdc,					// 主窗口hdc, main 传入
		mImage_hdc,					// 256*224 的 hdc, main 传入
		mCenter_hdc;				// 208*208 的 hdc, 本类产生, 是游戏区域,分开,经常更新,坐标也容易设定

	BoxMarkStruct* mBoxMarkStruct;			// 存储格子标记
	list<PlayerBase*> PlayerList;
	list<EnemyBase*> EnemyList;				// 敌机列表

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

	int mCutStageCounter;
	IMAGE mCurrentStageImage;		// STAGE 字样
	IMAGE mBlackNumberImage;		// 0123456789 当前关卡数
	IMAGE mGameOverImage;			//

	// 初始 mActiveEnemyTankNumber + mRemainEnemyTankNumber = 20
	int mRemainEnemyTankNumber;		// 余下未显示的敌机数量, 初始值=20, 出现一架敌机该值减一
	int mCurMovingTankNumber;		// 当前出现的敌机数量
	int mKillEnemyNum;				// 已经消灭的敌机数

	BlastStruct mBlast;				// 大本营爆炸
	bool mCampDie;					// 大本营被击中, 不再 Camp 贴图 

	bool mEnemyPause;			// 是否可以移动敌机
	//int mEnemyPauseCounter;		// 敌机暂停计数
	TimeClock mEnemyPauseTimer;		// 敌机暂停多久

	TimeClock mMainTimer;			// 控制绘图频率
	TimeClock mCampTimer;		// Camp 爆炸速度

	int mGameOverCounter;
	int mGameOverX, mGameOverY;	// 
	bool mGameOverFlag;			// 游戏结束标记, 玩家生命用完, 鸟巢被击中
	TimeClock mGameOverTimer;	// 上升控制

	IMAGE mCreateMapTankImage;		// 自定义地图中的 坦克游标图片
	int mCMTImageX, mCMTImageY;		// 游标中心点坐标

	bool mHasCustomMap;			// 当前是否有制作地图, 用于退出制作再次进入制作, 不会之前制作的数据
	bool mShowScorePanel;		// 是否显示分数面板

	bool mWin;			// 消灭完敌机,胜利
	int mWinCounter;	// 消灭完后隔几秒才跳转

	bool mShowGameOverAfterScorePanel;
	IMAGE msgoas_image;
	int msgoas_y;		// 上升坐标
	int msgoas_counter;
};