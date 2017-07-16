#include "stdafx.h"
#include "struct.h"


// ---------------- 子弹结构静态数据
IMAGE BulletStruct::mBulletImage[4];
int BulletStruct::mBulletSize[4][2] = { { 4,3 },{ 3,4 },{ 4,3 },{ 3,4 } };			// 不同方向子弹尺寸(width height)

// 子弹相对坦克中心的偏移量, 坐标偏移
int BulletStruct::devto_tank[4][2] = { { -BOX_SIZE, -1 },{ -2, -BOX_SIZE },{ BOX_SIZE - 4, -1 },{ -2, BOX_SIZE - 4 } };

// 子弹图片左上角坐标转换到弹头的偏移量(是坐标偏移不是下标),
// 左右方向在凸出的上面那点, 
// 上下方向则在凸出的右边那点
int BulletStruct::devto_head[4][2] = { { 0, 1 },{ 2, 0 },{ 4, 1 },{ 2, 4 } };

// 爆炸中心相对于子弹头的下标偏移量
int BulletStruct::bomb_center_dev[4][2] = { { 1, 0 },{ 0, 1 },{ 0, 0 },{ 0, 0 } };	// 

//------------------------

IMAGE BombStruct::mBombImage[3];

//----------------------------------

IMAGE BlastStruct::image[5];
BlastStruct::BlastStruct()
{
	blastx = -100;
	blasty = -100;
	canBlast = false;
	counter = 0;
}

//-----------------------------------------

IMAGE StarClass::mStarImage[4];
StarClass::StarClass()
{
	mStarIndexDev = -1;						// 控制索引下标的增加还是减少
	mStarIndex = 3;							// star 图片索引下标
	mStarCounter = 0;						// 多少次更换 star 图片
	mTankOutAfterCounter = rand() % 10 + 10;
	mIsOuted = false;						// 坦克是否已经出现
}

//------------------------------------------
IMAGE RingClass::image[2];
RingClass::RingClass()
{
	canshow = true;
	index_counter = 0;
}

bool RingClass::ShowRing(const HDC &canvas_hdc, int mTankX, int mTankY)
{
	if (index_counter > 790)
	{
		canshow = false;
		index_counter = 0;
		return false;
	}
	TransparentBlt(canvas_hdc, (int)(mTankX - BOX_SIZE), (int)(mTankY - BOX_SIZE), BOX_SIZE * 2, 
			BOX_SIZE * 2, GetImageHDC(&image[index_counter++ / 4 % 2]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	return true;
}

void RingClass::SetShowable()
{
	canshow = true;
	index_counter = 0;
}

/////////////////////////////////////////////////////

PropClass::PropClass()
{
	//bms = _bms;
	leftx = -100;
	topy = -100;
	index_counter = 0;
	can_show = false;

	TCHAR buf[100];
	for (int i = 0; i < 6; i++)
	{
		_stprintf_s(buf, _T("./res/big/prop/p%d.gif"), i);
		loadimage(&image[i], buf);
	}

	//mTimer.SetDrtTime(10);
}

// GameControl 内循环检测该函数
void PropClass::ShowProp(const HDC &canvas_hdc)
{
	if (!can_show)
		return;

	if ( (++index_counter / 17) % 2 == 0 )
		TransparentBlt(canvas_hdc, leftx, topy, BOX_SIZE * 2,
			BOX_SIZE * 2, GetImageHDC(&image[prop_kind]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

	// 超过时间 消失
	if (index_counter > 1300)
		StopShowProp();
}

// 道具敌机被消灭调用该函数
void PropClass::StartShowProp(int _x, int _y)
{
	leftx = 12 * BOX_SIZE;// (rand() % 25 + 1) * BOX_SIZE;
	topy = 12 * BOX_SIZE; //(rand() % 25 + 1) * BOX_SIZE;
	can_show = true;
	prop_kind = 2;// rand() % 6;		// 随机出现一个道具
	index_counter = 0;
	SignPropBox(PROP_SIGN + prop_kind);
}

//
void PropClass::StopShowProp()
{
	can_show = false;
	SignPropBox(_EMPTY);
}

void PropClass::SetBoxMarkStruct(BoxMarkStruct * b)
{
	bms = b;
}

//
void PropClass::SignPropBox(int val)
{
	int i = topy / BOX_SIZE;
	int j = leftx / BOX_SIZE;

	for (int ix = i; ix < i + 2; ix++)
	{
		for (int jy = j; jy < j + 2; jy++)
		{
			bms->prop_8[ix][jy] = val;
		}
	}
}
/*
IMAGE PropClass::image[6];
int PropClass::prop_kind = ADD_PROP;
*/
IMAGE ScorePanel::bunds;
IMAGE ScorePanel::background;
ScorePanel::ScorePanel(int player_id)
{
	switch (player_id)
	{
	case 0:
		player_x = 24;
		player_y = 47;
		pts_x = 25;
		pts_y = 85;
		loadimage(&player, _T("./res/big/scorepanel/player-0.gif") );
		loadimage(&pts, _T("./res/big/scorepanel/pts-0.gif"));

		loadimage(&background, _T("./res/big/scorepanel/background.gif"));
		loadimage(&bunds, _T("./res/big/scorepanel/bunds.gif"));
		break;
	case 1:
		player_x = 170;
		player_y = 47;
		pts_x = 137;
		pts_y = 85;
		loadimage(&player, _T("./res/big/scorepanel/player-1.gif"));
		loadimage(&pts, _T("./res/big/scorepanel/pts-1.gif"));
		break;
	default :
		break;
	}
}

ScorePanel::~ScorePanel()
{
}

void ScorePanel::show(const HDC& image_hdc)
{
	Sleep(30);
	//GameControl 内绘制BitBlt(image_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&background), 0, 0, SRCCOPY);
	BitBlt(image_hdc, player_x, player_y, player.getwidth(), player.getheight(), GetImageHDC(&player), 0, 0, SRCCOPY);
	BitBlt(image_hdc, pts_x, pts_y, pts.getwidth(), pts.getheight(), GetImageHDC(&pts), 0, 0, SRCCOPY);
}
