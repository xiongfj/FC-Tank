#pragma once

/************ 单个坦克信息 *********
* 玩家坦克信息
* 敌机坦克信息
***********************************/
class TankInfo
{
public:
	TankInfo();
	TankInfo(byte enemy_kind, int enemy_level, bool notuse );	// enemy_kind [0-1]; enemy_level [0-4], notuse->区别玩家坦克
	TankInfo(byte player, byte level);							// player [0-1]; level [0-3]
	~TankInfo();

	IMAGE GetTankImage( byte dir );
private:
	IMAGE mTankImage[4][2];		// 四个方向,一个方向两张贴图实现坦克移动动作.
};

/************ 玩家坦克级别 **********/

class PlayerTank
{
public:
	PlayerTank(byte player);		// player [0-1]
	~PlayerTank();
	IMAGE GetTankImage( byte level, byte dir );
private:
	TankInfo* mTankInfo[4];			// 4 个级别坦克
};

/************** 敌机坦克 ************
* 一个敌机实例化一个对象
* 提供敌机类别(是否是道具坦克), 敌机级别[0-4]
* 默认生成 0-0 敌机, 即0级的道具坦克
************************************/
class EnemyTank
{
public:
	EnemyTank();
	EnemyTank(byte kind, int level);	// kind[0-1]; level [0-4]
	~EnemyTank();
private:
	byte mEnemyTankKind;		// 敌机类别, 道具坦克和普通坦克两种, [0-1]
	int mEnemyTankLevel;		// 敌机坦克5个级别 [0-4]
	TankInfo* mEnemyTank;		// 指向坦克详细信息
};
