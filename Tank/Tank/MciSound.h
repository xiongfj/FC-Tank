#pragma once

#define S_START 0
#define S_SHOOT0	100
#define S_SHOOT1	101
#define S_SHOOT2	102
#define S_SHOOT3	103
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

////////////////////////////////////////////////////////////////////
/////////////////////// mciSendString //////////////////////////////
////////////////////////////////////////////////////////////////////

class MciSound
{
public:
	static void InitSounds();
	static void CloseSounds();

	static void _PlaySound(int kind);

	static void PauseBk(bool);		// 开始或暂停背景音乐
	static void PlayMovingSound(bool);		// 玩家坦克移动声音
};