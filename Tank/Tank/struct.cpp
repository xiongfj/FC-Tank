#include "stdafx.h"
#include "struct.h"
#include "MciSound.h"

// ---------------- �ӵ��ṹ��̬����
IMAGE BulletStruct::mBulletImage[4];
int BulletStruct::mBulletSize[4][2] = { { 4,3 },{ 3,4 },{ 4,3 },{ 3,4 } };			// ��ͬ�����ӵ��ߴ�(width height)

// �ӵ����̹�����ĵ�ƫ����, ����ƫ��
int BulletStruct::devto_tank[4][2] = { { -BOX_SIZE, -1 },{ -2, -BOX_SIZE },{ BOX_SIZE - 4, -1 },{ -2, BOX_SIZE - 4 } };

// �ӵ�ͼƬ���Ͻ�����ת������ͷ��ƫ����(������ƫ�Ʋ����±�),
// ���ҷ�����͹���������ǵ�, 
// ���·�������͹�����ұ��ǵ�
int BulletStruct::devto_head[4][2] = { { 0, 1 },{ 2, 0 },{ 4, 1 },{ 2, 4 } };

// ��ը����������ӵ�ͷ���±�ƫ����
int BulletStruct::bomb_center_dev[4][2] = { { 1, 0 },{ 0, 1 },{ 0, 0 },{ 0, 0 } };	// 

//------------------------

IMAGE BombStruct::mBombImage[3];

//----------------------------------

IMAGE BlastStruct::image[5];
BlastStruct::BlastStruct()
{
	Init();
	timer.SetDrtTime(36);

	// ̹�˱�ըͼƬ�ṹ
	TCHAR buf[100];
	for (INT i = 0; i < 5; i++)
	{
		_stprintf_s(buf, _T("./res/big/blast/%d.gif"), i);
		loadimage(&BlastStruct::image[i], buf);
	}
}

void BlastStruct::Init()
{
	blastx = -100;
	blasty = -100;
	canBlast = false;
	counter = 0;
}

BlastState BlastStruct::CampBlasting(const HDC & center_hdc)
{
	if (canBlast)
	{
		int index[17] = { 0,0,0,1,1,2,2,3,3,4,4,4,4,3,2,1,0 };
		TransparentBlt(center_hdc, blastx * BOX_SIZE, blasty * BOX_SIZE, BOX_SIZE * 4, BOX_SIZE * 4,
			GetImageHDC(&BlastStruct::image[index[counter % 17]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
		if (timer.IsTimeOut() && counter++ == 17)
		{
			canBlast = false;
			return BlastState::BlastEnd;
		}
		return BlastState::Blasting;
	}
	return BlastState::NotBlast;
}

BlastState BlastStruct::Blasting(const HDC& center_hdc)
{
	int index[13] = {0,1,1,2,2,3,3,4,4,3,2,1,0};
	if (canBlast)
	{
		TransparentBlt(center_hdc, blastx - BOX_SIZE * 2, blasty - BOX_SIZE * 2, BOX_SIZE * 4, BOX_SIZE * 4,
			GetImageHDC(&BlastStruct::image[index[counter % 13]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
		if (timer.IsTimeOut())
		{
			if (counter++ >= 12)
			{
				Init();
				return BlastState::BlastEnd;
			}
		}
		return BlastState::Blasting;
	}
	return BlastState::NotBlast;
}

// ר���ڵл���ը
BlastState BlastStruct::EnemyBlasting(const HDC &center_hdc, IMAGE* score )
{
	int index[13] = { 0,1,1,2,2,3,3,4,4,3,2,1,0 };
	if (canBlast)
	{
		if (counter < 13)
			TransparentBlt(center_hdc, blastx - BOX_SIZE * 2, blasty - BOX_SIZE * 2, BOX_SIZE * 4, BOX_SIZE * 4,
				GetImageHDC(&BlastStruct::image[index[counter % 13]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
		else
		{
			TransparentBlt(center_hdc, blastx - 7, blasty - 3, 14, 7,
				GetImageHDC(score), 0, 0, 14, 7, 0x000000);
		}
		if (timer.IsTimeOut())
		{
			if (counter++ >= 18)
			{
				Init();
				return BlastState::BlastEnd;
			}
		}
		return BlastState::Blasting;
	}
	return BlastState::NotBlast;
}

void BlastStruct::SetBlasting(int tankx, int tanky)
{
	blastx = tankx;
	blasty = tanky;
	canBlast = true;
}

bool BlastStruct::IsBlasting()
{
	return canBlast;
}

//-----------------------------------------

IMAGE StarClass::mStarImage[4];
StarClass::StarClass()
{
	TCHAR buf[100];
	// �����Ľ�����˸
	for (int i = 0; i < 4; i++)
	{
		_stprintf_s(buf, _T("./res/big/star%d.gif"), i);
		loadimage(&StarClass::mStarImage[i], buf);
	}

	Init();
}

void StarClass::Init()
{
	mStarIndexDev = -1;						// ���������±�����ӻ��Ǽ���
	mStarIndex = 3;							// star ͼƬ�����±�
	mStarCounter = 0;						// ���ٴθ��� star ͼƬ
	mTankOutAfterCounter = rand() % 10 + 10;
	mIsOuted = false;						// ̹���Ƿ��Ѿ�����
	mStarOuted = false;
}

Star_State StarClass::ShowStar(const HDC& center_hdc, int tankx, int tanky)
{
	// ̹���Ѿ�����,������˸,ֱ�ӷ���
	if (mIsOuted == true)
		return Star_State::Tank_Out;

	// ��ʼ��˸�Ľ���
	if (mStarCounter++ % 2 == 0)
	{
		if (mStarIndex + mStarIndexDev < 0)
		{
			mStarIndex = 1;
			mStarIndexDev = 1;
		}
		else if (mStarIndex + mStarIndexDev > 3)
		{
			mStarIndex = 2;
			mStarIndexDev = -1;
		}
		else
		{
			mStarIndex += mStarIndexDev;
		}
		if (mStarCounter == 25)
		{
			mIsOuted = true;						// ������˸, TankMoving() ������ʼѭ��, ̹�˿�ʼ�ƶ�
			return Star_State::Star_Stop;
		}
	}

	TransparentBlt(center_hdc, tankx - BOX_SIZE, tanky - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&mStarImage[mStarIndex]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

	return Star_State::Star_Showing;
}

bool StarClass::IsStop()
{
	return mIsOuted;
}

Star_State StarClass::EnemyShowStar(const HDC &center_hdc, int tankx, int tanky, const BoxMarkStruct* bms)
{
	// ̹���Ѿ�����,������˸,ֱ�ӷ���
	if (mIsOuted == true)
		return Star_State::Tank_Out;

	// һ��ʱ������ʾ�Ľ���, ֮ǰ����
	if (mTankOutAfterCounter-- > 0)
		return Star_State::Star_Timing;

	// �Ľ��ǻ�û���ֲż�� box_4
	if (mStarOuted == false)
	{
		int iy = tanky / SMALL_BOX_SIZE - 2;
		int jx = tankx / SMALL_BOX_SIZE - 2;
		for (int i = iy; i < iy + 4; i++)
		{
			for (int j = jx; j < jx + 4; j++)
			{
				// ����Ľ���, ���,�л�,
				if (bms->box_4[i][j] != STAR_SIGN && bms->box_4[i][j] > _FOREST)
				{
					mTankOutAfterCounter = rand() % 100 + 10;
					return Star_State::Star_Failed;
				}
			}
		}

		// �Ľ��Ǹճ���, �����ϲ㺯�� ��� box_4
		mStarOuted = true;
		return Star_State::Star_Out;
	}

	// ��ʼ��˸�Ľ���
	if (mStarCounter++ % 2 == 0)
	{
		if (mStarIndex + mStarIndexDev < 0)
		{
			mStarIndex = 1;
			mStarIndexDev = 1;
		}
		else if (mStarIndex + mStarIndexDev > 3)
		{
			mStarIndex = 2;
			mStarIndexDev = -1;
		}
		else
		{
			mStarIndex += mStarIndexDev;
		}
		if (mStarCounter == 35)
		{
			mIsOuted = true;						// ������˸, TankMoving() ������ʼѭ��, ̹�˿�ʼ�ƶ�
			return Star_State::Star_Stop;
		}
	}

	TransparentBlt(center_hdc, tankx - BOX_SIZE, tanky - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
		GetImageHDC(&mStarImage[mStarIndex]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

	return Star_State::Star_Showing;
}

//------------------------------------------
IMAGE RingClass::image[2];
RingClass::RingClass()
{
	TCHAR buf[100];
	// ����������
	for (int i = 0; i < 2; i++)
	{
		_stprintf_s(buf, _T("./res/big/ring%d.gif"), i);
		loadimage(&RingClass::image[i], buf);
	}

	Init();
}

void RingClass::Init()
{
	canshow = true;
	index_counter = 0;
	//timer.Init();
	timer.SetDrtTime(3222);
}

//
bool RingClass::ShowRing(const HDC &canvas_hdc, int mTankX, int mTankY)
{
	if (timer.IsTimeOut())
	{
		index_counter = 0;
		canshow = false;
		return false;
	}
	TransparentBlt(canvas_hdc, (int)(mTankX - BOX_SIZE), (int)(mTankY - BOX_SIZE), BOX_SIZE * 2, 
			BOX_SIZE * 2, GetImageHDC(&image[index_counter++ / 4 % 2]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	return true;
}

void RingClass::SetShowable( long drt_time)
{
	canshow = true;
	index_counter = 0;
	timer.Init();
	timer.SetDrtTime(drt_time);
}

/////////////////////////////////////////////////////

PropClass::PropClass()
{
	//Init();

	//score_counter = 0;
	//show_score = false;

	//loadimage(&score, _T("./res/big/500.gif"));

	//TCHAR buf[100];
	//for (int i = 0; i < 6; i++)
	//{�����ڴ˼��أ�IMAGE �� �����滹û�г�ʼ������
	//	_stprintf_s(buf, _T("./res/big/prop/p%d.gif"), i);
	//	loadimage(&image[i], buf);
	//}
}

//
void PropClass::Init(BoxMarkStruct * b)
{
	bms = b;
	leftx = -100;
	topy = -100;
	index_counter = 0;
	can_show = false;
	show_score = false;
	score_counter = 0;

	loadimage(&score, _T("./res/big/500.gif"));

	TCHAR buf[100];
	for (int i = 0; i < 6; i++)
	{
		_stprintf_s(buf, _T("./res/big/prop/p%d.gif"), i);
		loadimage(&image[i], buf);
	}
}

// GameControl ��ѭ�����ú���
void PropClass::ShowProp(const HDC &canvas_hdc)
{
	if (!can_show)
		return;

	if (show_score)
	{
		score_counter++;
		TransparentBlt(canvas_hdc, leftx + 8, topy + 3, 14, 7, GetImageHDC(&score), 0, 0, 14, 7, 0x000000);
		if (score_counter > 37)
		{
			show_score = false;
			can_show = false;
		}
	}
	else if ( can_show )
	{
		if ((++index_counter / 17) % 2 == 0)
			TransparentBlt(canvas_hdc, leftx, topy, BOX_SIZE * 2,
				BOX_SIZE * 2, GetImageHDC(&image[prop_kind]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
	}

	// ����ʱ�� ��ʧ
	if (index_counter > 1300)
		StopShowProp(false);
}

// ���ߵл���������øú���
void PropClass::StartShowProp(int _i, int _j)
{
	// �����һ�εı��, ��ֹ��һ�����߻�û��ʧ,���ֵڶ������ߵ�ʱ��, ������һ�����ߵ� SIGN
	SignPropBox(_EMPTY);

	leftx = _j * BOX_SIZE;// (rand() % 25 + 1) * BOX_SIZE;
	topy = _i * BOX_SIZE; //(rand() % 25 + 1) * BOX_SIZE;
	can_show = true;
	prop_kind = rand() % 6;		// �������һ������
	index_counter = 0;
	SignPropBox(PROP_SIGN + prop_kind);
}

// getted = true ��ʾ��һ�õ���, ��ʾ����, false ��ʱ����ʾ����
void PropClass::StopShowProp(bool getted)
{
	//can_show = false;
	if (getted)
	{
		show_score = true;
		score_counter = 0;
	}
	else
		can_show = false;
	SignPropBox(_EMPTY);
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
int ScorePanel::end_counter = 0;
int ScorePanel::cur_line = 0;
bool ScorePanel::line_done_flag[2] = {false, false};
int ScorePanel::player_num = 0;
int ScorePanel::who_bunds[2] = {0, 0};
IMAGE ScorePanel::number;
IMAGE ScorePanel::bunds;
IMAGE ScorePanel::background;
IMAGE ScorePanel::yellow_number;
ScorePanel::ScorePanel(int id)
{
	//����, gameover �ٴν�����Ϸ���ܻ���� 2 player_num++;
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

		loadimage(&yellow_number, _T("./res/big/yellow-number.gif"));
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

		total_score_x = 78;
		total_score_y = 58;
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

		total_score_x = 224;
		total_score_y = 58;
		break;
	default:
		break;
	}

	for (int i = 0; i < 4; i++)
	{
		kill_num[i] = 0;		// ���� PlayerBase ���ݹ���������
		kill_num2[i] = -1;		// Ĭ��ɱ���� = -1 flag, ��ʱ����ʾ
	}

	total_kill_numm = 0;
	end_counter = 0;
	total_score = 0;
	stage = 1;

	who_bunds[0] = who_bunds[1] = 0;
}

ScorePanel::~ScorePanel()
{
}

/*
*/
bool ScorePanel::show(const HDC& image_hdc)
{
	BitBlt(image_hdc, player_x, player_y, player.getwidth(), player.getheight(), GetImageHDC(&player), 0, 0, SRCCOPY);
	BitBlt(image_hdc, pts_x, pts_y, pts.getwidth(), pts.getheight(), GetImageHDC(&pts), 0, 0, SRCCOPY);

	// ����ÿ��ɱ�����Լ���ʾ ++ ��ʾ��һ�в���ʾ��һ��
	for (int i = 0; i < 4; i++)
	{
		// ��ǰ��ʾ�ŵ���
		if (cur_line == i)
		{
			int temp = kill_num2[i] + 1;

			if (temp <= kill_num[i])
			{
				kill_num2[i]++;
				MciSound::_PlaySound(S_SCOREPANEL_DI);
				break;
			}

			// ���������Ҹ��з�����ʾ������
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

	//STAGE
	TransparentBlt(image_hdc, 154, 32, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
		BLACK_NUMBER_SIZE * (stage % 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
	if ( stage > 10)
		TransparentBlt(image_hdc, 154 - 8, 32, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
			BLACK_NUMBER_SIZE * (stage / 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

	// 0��
	TransparentBlt(image_hdc, total_score_x, total_score_y, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&yellow_number),
		BLACK_NUMBER_SIZE * 0, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

	//  ��λ��
	if (total_score > 90)
	{
		TransparentBlt(image_hdc, total_score_x - 8, total_score_y, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&yellow_number),
			BLACK_NUMBER_SIZE * 0, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

		TransparentBlt(image_hdc, total_score_x - 16, total_score_y, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&yellow_number),
			BLACK_NUMBER_SIZE * (total_score % 1000 / 100), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
	}
	// ��λ��
	if (total_score > 900)	
		TransparentBlt(image_hdc, total_score_x - 24, total_score_y, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&yellow_number),
			BLACK_NUMBER_SIZE * (total_score / 1000), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);

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

		int score = (i + 1) * 100 * kill_num2[i];	// һ��̹��һ��100��, ��������

		// ��������Ƕ���λ��
		int temp = score;
		int score_bit = 1;		
		while (temp / 10 != 0)
		{
			score_bit++;
			temp /= 10;
		}

		// ������� 4 λ��
		switch (score_bit)
		{
		case 1:
			TransparentBlt(image_hdc, x[i][0] + 16, y[i][0], BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, GetImageHDC(&number),
				BLACK_NUMBER_SIZE * 0, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0x000000);
			break;
		// �������� ��λ��!?!? case 2: break;

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

		if (player_num == 2 && who_bunds[0] > who_bunds[1] && who_bunds[0] > 1000)
			TransparentBlt(image_hdc, 26, 190, 63, 15, GetImageHDC(&bunds), 0, 0, 63, 15, 0x000000);
		else if (player_num == 2 && who_bunds[1] > who_bunds[0] && who_bunds[1] > 1000)
			TransparentBlt(image_hdc, 170, 190, 63, 15, GetImageHDC(&bunds), 0, 0, 63, 15, 0x000000);
			
		// ֻ����һ��
		if (end_counter == 0 && player_num == 2 && who_bunds[0] + who_bunds[1] > 2000)
			MciSound::_PlaySound(S_BOUNS1000);

		if (end_counter++ > 30)
			return false;			// ���ؽ�����־
	}

	return true;
}

/*��Ϸ����ʱ��, ��ȡÿ����ҵ�ɱ����!! ֻ�ܵ���һ��!!! */
void ScorePanel::ResetData(const int * nums, const int& players, const int& sta)
{
	// ����Ҫ����, �����´���ʾ��ʱ����ԭ�ȵ�����
	player_num = players;
	cur_line = 0;
	end_counter = 0;
	line_done_flag[0] = false;
	line_done_flag[1] = false;
	total_kill_numm = 0;
	total_score = 0;
	stage = sta;
	who_bunds[player_id] = 0;// �������õ���һ����ҵ�����~~!! = who_bunds[1] = 0;

	for (int i = 0; i < 4; i++)
	{
		kill_num[i] = 0;		// ���� PlayerBase ���ݹ���������
		kill_num2[i] = -1;		// Ĭ��ɱ���� = -1 flag, ��ʱ����ʾ
	}

	// ��ȡ������
	for (int i = 0; i < 4; i++)
	{
		kill_num[i] = nums[i];
		total_kill_numm += nums[i];
	}

	total_score = nums[0] * 100 + nums[1] * 200 + nums[2] * 300 + nums[3] * 400;

	who_bunds[player_id] = total_score;
}

/////////////////////////////////////////////

IMAGE PlayerGameover::mGameoverImage;
PlayerGameover::PlayerGameover()
{
	mGameoverTimer.SetDrtTime(20);
	loadimage(&mGameoverImage, _T("./res/big/gameover.gif"));
}

void PlayerGameover::Init(int player_id)
{
	switch (player_id)
	{
	case 0:
		// ���die ����ʾ���Ƶ� GAMEOVER ����
		mGameoverX = 0;
		mGameover_Dev = 3;
		mGameover_end_x = 53;
		break;

	case 1:
		// ���die ����ʾ���Ƶ� GAMEOVER ����
		mGameoverX = 220;
		mGameover_Dev = -3;
		mGameover_end_x = 122;
		break;
	default:
		break;
	}

	// ��ұ��������ʾͼƬ GAMEOVER
	mGameoverY = 191;
	mGameoverCounter = 0;
	mShowGameover = false;
}

void PlayerGameover::SetShow()
{
	mShowGameover = true;
}

void PlayerGameover::Show(const HDC& center_hdc)
{
	if (mGameoverCounter > 70)
		mShowGameover = false;

	if (!mShowGameover)
		return;

	TransparentBlt(center_hdc, mGameoverX, mGameoverY, 31, 15, GetImageHDC(&mGameoverImage), 0, 0, 31, 15, 0x000000);

	if (mGameoverTimer.IsTimeOut() == false)
		return;

	if (abs(mGameoverX - mGameover_end_x) < 5)
	{
		mGameoverCounter++;
		mGameoverX = mGameover_end_x;
	}
	else
		mGameoverX += mGameover_Dev;
}
