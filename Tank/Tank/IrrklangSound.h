#pragma once


/*
* close 指定关闭某个声音的 别名 
* name 本地声音文件名称, 带扩展名, 不带路径,
* type_name 别名
*/
//void _PlaySound(const TCHAR* close, const TCHAR* name, const TCHAR* type_name);

#define S_START 0
#define S_SHOOT	1
#define S_BIN	2
#define S_BK	3
#define S_GETPROP	4		// 获得道具
#define S_PROPOUT	5
#define S_ENEMY_BOMB	6		// 敌机爆炸
#define S_CAMP_BOMB	7
#define S_SCOREPANEL_DI	8		// 分数面板 嘀嘀声
#define S_BOUNS1000		9
#define S_ADDLIFE	10
#define S_WIN	11
#define S_FAIL	12
#define S_PLAYER_BOMB	13
#define S_PLAYER_MOVE	14

class IrrklangSound
{
public:
	static void InitSounds();

	static void _PlaySound(int kind);

	static void PauseBk(bool);		// 开始或暂停背景音乐
	static void PauseMove(bool);		// 玩家坦克移动声音

private:
	static ISoundEngine* engine;
	static ISound* bk_control;
	static ISound* player_move_control;

	static ISoundSource* start;		// 选择玩家结束
	static ISoundSource* shoot;		// 射击子弹
	static ISoundSource* bin;		// 击中石头墙壁,最大的那个坦克
	static ISoundSource* bk;		// 游戏背景声音
	static ISoundSource* getprop;	// 
	static ISoundSource* propout;	// 
	static ISoundSource* enemy_bomb;	// 
	//static ISoundSource* camp_bomb;	// 
	static ISoundSource* scorepanel_di;	// 分数面板的嘀嘀声
	static ISoundSource* bunds1000;	// 
	static ISoundSource* addlife;	// addlife
	//static ISoundSource* win;	// 
	static ISoundSource* fail;	// 
	static ISoundSource* player_bomb;	// player_bombplayer_move
	static ISoundSource* player_move;	// player_move
};