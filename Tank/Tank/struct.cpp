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
int ScorePanel::cur_line = 0;
bool ScorePanel::line_done_flag[2] = {false, false};
int ScorePanel::player_num = 0;
IMAGE ScorePanel::number;
IMAGE ScorePanel::bunds;
IMAGE ScorePanel::background;
ScorePanel::ScorePanel(int id)
{
	player_num++;
	player_id = id;
	switch (player_id)
	{
	case 0:
		player_x = 24;
		player_y = 47;
		pts_x = 25;
		pts_y = 85;
		loadimage(&player, _T("./res/big/scorepanel/player-0.gif") );
		loadimage(&pts, _T("./res/big/scorepanel/pts-0.gif"));

		loadimage(&number, _T("./res/big/white-number.gif"));
		loadimage(&background, _T("./res/big/scorepanel/background.gif"));
		loadimage(&bunds, _T("./res/big/scorepanel/bunds.gif"));

		for (int i = 0; i < 4; i++)
		{
			x[i][0] = 34;
			x[i][1] = 103;

			y[i][0] = 88 + i * 24;
			y[i][1] = 88 + i * 24;

			total_kill_x = 103;
			total_kill_y = 178;
		}
		break;

	case 1:
		player_x = 170;
		player_y = 47;
		pts_x = 137;
		pts_y = 85;
		loadimage(&player, _T("./res/big/scorepanel/player-1.gif"));
		loadimage(&pts, _T("./res/big/scorepanel/pts-1.gif"));

		for (int i = 0; i < 4; i++)
		{
			x[i][0] = 177;
			x[i][1] = 154;
			
			y[i][0] = 88 + i * 24;
			y[i][1] = 88 + i * 24;

			total_kill_x = 154;
			total_kill_y = 178;
		}
		break;
	default:
		break;
	}

	for (int i = 0; i < 4; i++)
	{
		kill_num[i] = 0;		// 接收 PlayerBase 传递过来的数据
		kill_num2[i] = -1;		// 默认杀敌数 = -1 flag, 此时不显示
	}

	total_kill_numm = 0;
}

ScorePanel::~ScorePanel()
{
}

/*
step 是一个 int step[2] 数据;
* step[0]: 0,1,2,3,4,5 分 6 步, 0,1,2,3 显示杀敌数和的分数; 4 显示总杀敌数; 5 显示 bunds (两个玩家谁杀敌数多显示在哪一边)
* step[1]: 是 step[0] 内的细分, 
*/
void ScorePanel::show(const HDC& image_hdc, int* step)
{
	BitBlt(image_hdc, player_x, player_y, player.getwidth(), player.getheight(), GetImageHDC(&player), 0, 0, SRCCOPY);
	BitBlt(image_hdc, pts_x, pts_y, pts.getwidth(), pts.getheight(), GetImageHDC(&pts), 0, 0, SRCCOPY);

	// 控制每行杀敌数自加显示 ++ 显示完一行才显示下一行
	for (int i = 0; i < 4; i++)
	{
		if (cur_line == i)
		{
			int temp = kill_num2[i] + 1;
			if (temp <= kill_num[i])
			{
				kill_num2[i]++;
				break;
			}
			else
			{
				line_done_flag[player_id] = true;
				bool temp = true;
				for (int m = 0; m < player_num; m++)
				{
					if (line_done_flag[m] == false)
					{
						temp = false;
						break;
					}
				}
				if (temp)
				{
					cur_line++; 
					for (int m = 0; m < player_num; m++)
						line_done_flag[m] = false;
				}
			}
		}
	}

	Sleep(100);

	for (int i = 0; i < 4; i++)
	{
		if (kill_num2[i] == -1)
			continue;

		if (kill_num2[i] < 10)
		{
			TransparentBlt(image_hdc, x[i][1], y[i][1], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * kill_num2[i], 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
		}
		else
		{
			TransparentBlt(image_hdc, x[i][1]- 8, y[i][1], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * (kill_num2[i] / 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

			TransparentBlt(image_hdc, x[i][1], y[i][1], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * (kill_num2[i] % 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
		}

		int score = (i + 1) * 100 * kill_num2[i];	// 一级坦克一架100分, 依次类推

		// 计算分数是多少位数
		int temp = score;
		int score_bit = 1;		
		while (temp / 10 != 0)
		{
			score_bit++;
			temp /= 10;
		}

		// 分数最多 4 位数
		switch (score_bit)
		{
		case 1:
			TransparentBlt(image_hdc, x[i][0] + 16, y[i][0], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * 0, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
			break;
		// 不可能是 两位数!?!? case 2: break;

		case 3:
			TransparentBlt(image_hdc, x[i][0], y[i][0], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * (score / 100), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

			TransparentBlt(image_hdc, x[i][0] + 8, y[i][0], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * 0, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

			TransparentBlt(image_hdc, x[i][0] + 16, y[i][0], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * 0, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
			break;

		case 4:
			TransparentBlt(image_hdc, x[i][0] - 8, y[i][0], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * (score / 1000), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

			TransparentBlt(image_hdc, x[i][0], y[i][0], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * (score % 1000 / 100), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

			TransparentBlt(image_hdc, x[i][0] + 8, y[i][0], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * 0, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

			TransparentBlt(image_hdc, x[i][0] + 16, y[i][0], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * 0, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
			break;

		default:
			break;
		}
	}

	if (cur_line == 4)
	{
		if (total_kill_numm < 10)
		{
			TransparentBlt(image_hdc, total_kill_x, total_kill_y, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * total_kill_numm, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
		}
		else
		{
			TransparentBlt(image_hdc, total_kill_x - 8, total_kill_y, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * (total_kill_numm / 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

			TransparentBlt(image_hdc, total_kill_x, total_kill_y, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * (total_kill_numm % 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
		}
	}
}

/*游戏结束时候, 获取每个玩家的杀敌数!! 只能调用一次!!! */
void ScorePanel::SetKillNum(const int * nums)
{
	for (int i = 0; i < 4; i++)
	{
		kill_num[i] = nums[i];
		total_kill_numm += nums[i];
	}
}
