#include "stdafx.h"
#include "GameControl.h"
#include "MciSound.h"

int GameControl::mCurrentStage = 1;	// [1-35]
GameControl::GameControl( HDC des_hdc, HDC image_hdc/*, BoxMarkStruct* bms*/)
{
	mDes_hdc = des_hdc;
	mImage_hdc = image_hdc;
	mCenterImage.Resize( CENTER_WIDTH, CENTER_HEIGHT );
	mCenter_hdc = GetImageHDC(&mCenterImage);
	mBoxMarkStruct = new BoxMarkStruct();

	loadimage( &mBlackBackgroundImage, _T("./res/big/bg_black.gif"		));		// ��ɫ����
	loadimage( &mGrayBackgroundImage , _T("./res/big/bg_gray.gif"		));		// ��ɫ����
	loadimage( &mStoneImage			 , _T("./res/big/stone.gif"			));		// 12*12��ʯͷ
	loadimage( &mForestImage		 , _T("./res/big/forest.gif"		));		// ����
	loadimage( &mIceImage			 , _T("./res/big/ice.gif"			));		// ����
	loadimage( &mRiverImage[0]		 , _T("./res/big/river-0.gif"		));		// ����
	loadimage( &mRiverImage[1]		 , _T("./res/big/river-1.gif"		));		//
	loadimage( &mWallImage			 , _T("./res/big/wall.gif"			));		// ��ǽ
	loadimage( &mCamp[0]			 , _T("./res/big/camp0.gif"			));		// ��Ӫ
	loadimage( &mCamp[1]			 , _T("./res/big/camp1.gif"			));		// 
	loadimage( &mEnemyTankIcoImage	 , _T("./res/big/enemytank-ico.gif"	));		// �л�ͼ��
	loadimage( &mFlagImage			 , _T("./res/big/flag.gif"			));		// ����
	loadimage( &mCurrentStageImage	 , _T("./res/big/stage.gif"			));
	loadimage( &mBlackNumberImage	 , _T("./res/big/black-number.gif"	));		// 0123456789 ��ɫ����
	loadimage( &mGameOverImage		 , _T("./res/big/gameover.gif"		));

	// �Զ�����Ƶ�ͼ
	loadimage(&mCreateMapTankImage, _T("./res/big/0Player/m0-1-2.gif"));

	loadimage(&msgoas_image, _T("./res/big/big-gameover.gif"));

	Init();
}

GameControl::~GameControl()
{
	for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
	{
		delete *itor;
		//bug ?? PlayerList.erase(itor);
		//PlayerList.remove(*itor);
	}

	/* �ŵ����������ʾ����ͷ�
	for (list<EnemyBase*>::iterator itor = EnemyList.begin(); itor != EnemyList.end(); itor++)
	{
		delete *itor;
		//EnemyList.erase(itor);
	}
	EnemyList.clear();*/
	//printf("%d .....\n", EnemyList.size());

	delete mBoxMarkStruct;

	printf("~GameControl::GameControl()\n");
}

void GameControl::Init()
{
	//mOutedEnemyTankNumber = 0;													// �Ѿ������ڵ�ͼ�ϵĵл�����,�����ʾ6��
	mRemainEnemyTankNumber = 20;	 // ʣ��δ���ֵĵл�����
	mCurMovingTankNumber = 0;
	mKillEnemyNum = 0;
	mCampDie = false;															// ��־��Ӫ�Ƿ񱻻���

	mEnemyPause = false;			// �л���ͣ���
	//mEnemyPauseCounter = 0;		
	mEnemyPauseTimer.SetDrtTime(10000);	// �л���ͣ���

	mMainTimer.SetDrtTime(14);
	mCampTimer.SetDrtTime(33);

	mCutStageCounter = 0;		// STAGE ��������

	// GameOver ͼƬ
	mGameOverCounter = 0;
	mGameOverX = -100;
	mGameOverY = -100;
	mGameOverFlag = false;
	mGameOverTimer.SetDrtTime(30);

	// ���Ƶ�ͼ̹���α������
	mCMTImageX = BOX_SIZE;
	mCMTImageY = BOX_SIZE;

	// ÿ�ν����ͼ����֮ǰ�����֮ǰ�Ƿ���������ͼ
	mHasCustomMap = false;

	// �ؿ�������ʾ�������
	mShowScorePanel = false;

	// ʤ��
	mWin = false;
	mWinCounter = 0;

	msgoas_counter = 0;
	msgoas_y = CENTER_HEIGHT;
	//msgoas_Timer.SetDrtTime(20);
	mShowGameOverAfterScorePanel = false;
}

// �洢��ҽ�����
void GameControl::AddPlayer(int player_num)
{
	for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
	{
		delete *itor;
	}

	// ���ԭ������
	PlayerList.clear();

	for (int i = 0; i < player_num; i++)
		PlayerList.push_back(new PlayerBase(i, mBoxMarkStruct));

	PlayerBase::mPList = &PlayerList;
}

/*
* ��ȡdata���ݻ��Ƶ�ͼ,
* ��ʾ�л�����\�������\�ؿ�\����Ϣ
*/
void GameControl::LoadMap()
{
	// ��ȡ��ͼ�ļ�����
	FILE* fp = NULL;
	if (0 != fopen_s(&fp, "./res/data/map.dat", "rb"))
		throw _T("��ȡ��ͼ�����ļ��쳣.");
	fseek(fp, sizeof(Map) * (mCurrentStage - 1), SEEK_SET);
	fread(&mMap, sizeof(Map), 1, fp);
	fclose(fp);

	InitSignBox();
}

// ����Լ�������ͼ
bool GameControl::CreateMap(bool* isCreate)
{
	int i, j, x = 0, y = 0;
	int tempx, tempy;
	bool flag = true;
	int keys[4] = {VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN};		// �±������ DIR_LEFT �ȶ�Ӧ
	int dev[4][2] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1} };	// �α�̹���ƶ�����
	int twinkle_counter = 0;	// ̹���α���˸����
	int lastx = mCMTImageX, lasty = mCMTImageY;			// ��¼̹���ϴε�����, ���̹���ƶ�����任 sign_order ͼ��
	bool M_down = false;		// ��� M �Ƿ���, һֱ���� M �����л���ͼ, ֻ���µ���һ���л�

	// 14 �����
	int sign_order[14][4] = { {_ICE, _ICE, _ICE, _ICE},		// �ĸ����Ӷ��Ǳ�, ���������������� ..
		{ _FOREST,	_FOREST,	_FOREST,	_FOREST},
		{ _RIVER,	_RIVER,		_RIVER,		_RIVER },
		{ _STONE,	_STONE ,	_STONE ,	_STONE },
		{ _STONE ,	_STONE ,	_EMPTY,		_EMPTY},
		{ _STONE ,	_EMPTY,		_STONE ,	_EMPTY},
		{ _EMPTY,	_EMPTY,		_STONE ,	_STONE },
		{ _EMPTY,	_STONE ,	_EMPTY,		_STONE },
		{ _WALL,	_WALL ,		_WALL ,		_WALL },
		{ _WALL ,	_WALL ,		_EMPTY,		_EMPTY},
		{ _WALL ,	_EMPTY,		_WALL,		_EMPTY},
		{ _EMPTY,	_EMPTY,		_WALL ,		_WALL },
		{ _EMPTY,	_WALL ,		_EMPTY,		_WALL },
		{ _EMPTY,	_EMPTY ,	_EMPTY ,	_EMPTY } };
	
	int cur_index = 13;		// ��Ӧ��������

	// ��������ϴλ��Ƶĵ�ͼ
	if ( !mHasCustomMap )
		ClearSignBox();

	// �����ٶ�
	TimeClock click;
	click.SetDrtTime(80);

	// ��ɫ����
	StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);
	int counter = 0;
	while ( flag )
	{
		counter++;
		Sleep(34);

		if (GetAsyncKeyState(VK_LEFT) & 0x8000 && counter > 3)
		{
			counter = 0;

			lastx = mCMTImageX;
			lasty = mCMTImageY;
			tempx = mCMTImageX + dev[0][0] * BOX_SIZE * 2;
			tempy = mCMTImageY + dev[0][1] * BOX_SIZE * 2;
			if (tempx >= BOX_SIZE && tempx <= BOX_SIZE * 25 && tempy >= BOX_SIZE && tempy <= BOX_SIZE * 25)
			{
				mCMTImageX += dev[0][0] * BOX_SIZE * 2;
				mCMTImageY += dev[0][1] * BOX_SIZE * 2;
			}
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000 && counter > 3)
		{
			counter = 0;

			lastx = mCMTImageX;
			lasty = mCMTImageY;
			tempx = mCMTImageX + dev[1][0] * BOX_SIZE * 2;
			tempy = mCMTImageY + dev[1][1] * BOX_SIZE * 2;
			if (tempx >= BOX_SIZE && tempx <= BOX_SIZE * 25 && tempy >= BOX_SIZE && tempy <= BOX_SIZE * 25)
			{
				mCMTImageX += dev[1][0] * BOX_SIZE * 2;
				mCMTImageY += dev[1][1] * BOX_SIZE * 2;
			}
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && counter > 3)
		{
			counter = 0;

			lastx = mCMTImageX;
			lasty = mCMTImageY;
			tempx = mCMTImageX + dev[2][0] * BOX_SIZE * 2;
			tempy = mCMTImageY + dev[2][1] * BOX_SIZE * 2;
			if (tempx >= BOX_SIZE && tempx <= BOX_SIZE * 25 && tempy >= BOX_SIZE && tempy <= BOX_SIZE * 25)
			{
				mCMTImageX += dev[2][0] * BOX_SIZE * 2;
				mCMTImageY += dev[2][1] * BOX_SIZE * 2;
			}
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000 && counter > 3)
		{
			counter = 0;

			lastx = mCMTImageX;
			lasty = mCMTImageY;
			tempx = mCMTImageX + dev[3][0] * BOX_SIZE * 2;
			tempy = mCMTImageY + dev[3][1] * BOX_SIZE * 2;
			if (tempx >= BOX_SIZE && tempx <= BOX_SIZE * 25 && tempy >= BOX_SIZE && tempy <= BOX_SIZE * 25)
			{
				mCMTImageX += dev[3][0] * BOX_SIZE * 2;
				mCMTImageY += dev[3][1] * BOX_SIZE * 2;
			}
		}
		else if (GetAsyncKeyState('J') & 0x8000 && counter > 3)
		{
			counter = 0;

			if (mCMTImageX == lastx && mCMTImageY == lasty)
				cur_index = cur_index + 1 > 13 ? 0 : cur_index + 1;
			else
			{
				lastx = mCMTImageX;
				lasty = mCMTImageY;
			}

			// ���� 16*16 �ĵ�ͼ
			i = mCMTImageY / BOX_SIZE - 1;
			j = mCMTImageX / BOX_SIZE - 1;
			mBoxMarkStruct->box_8[i][j] = sign_order[cur_index][0];
			mBoxMarkStruct->box_8[i][j + 1] = sign_order[cur_index][1];
			mBoxMarkStruct->box_8[i + 1][j] = sign_order[cur_index][2];
			mBoxMarkStruct->box_8[i + 1][j + 1] = sign_order[cur_index][3];
		}
		else if (GetAsyncKeyState('K') & 0x8000 && counter > 3)
		{
			counter = 0;

			if (mCMTImageX == lastx && mCMTImageY == lasty)
				cur_index = cur_index - 1 < 0 ? 13 : cur_index - 1;
			else
			{
				lastx = mCMTImageX;
				lasty = mCMTImageY;
			}

			// ���� 16*16 �ĵ�ͼ
			i = mCMTImageY / BOX_SIZE - 1;
			j = mCMTImageX / BOX_SIZE - 1;
			mBoxMarkStruct->box_8[i][j] = sign_order[cur_index][0];
			mBoxMarkStruct->box_8[i][j + 1] = sign_order[cur_index][1];
			mBoxMarkStruct->box_8[i + 1][j] = sign_order[cur_index][2];
			mBoxMarkStruct->box_8[i + 1][j + 1] = sign_order[cur_index][3];
		}

			// J,K ��һ��˳��,һ������
			/*SHORT J_KEY = ;
			SHORT K_KEY = ;

			// �����ϰ���
			if(J_KEY || K_KEY)
			{
				if (mCMTImageX == lastx && mCMTImageY == lasty)
				{
					if (J_KEY)
						cur_index = cur_index + 1 > 13 ? 0 : cur_index + 1;
					else if (K_KEY)
						cur_index = cur_index - 1 < 0 ? 13 : cur_index - 1;
				}
				else
				{
					lastx = mCMTImageX;
					lasty = mCMTImageY;
				}

				// ���� 16*16 �ĵ�ͼ
				i = mCMTImageY / BOX_SIZE - 1;
				j = mCMTImageX / BOX_SIZE - 1;
				mBoxMarkStruct->box_8[i][j] = sign_order[cur_index][0];
				mBoxMarkStruct->box_8[i][j + 1] = sign_order[cur_index][1];
				mBoxMarkStruct->box_8[i + 1][j] = sign_order[cur_index][2];
				mBoxMarkStruct->box_8[i + 1][j + 1] = sign_order[cur_index][3];
			}*/

		else if (GetAsyncKeyState(VK_RETURN) & 0x8000 && counter > 3)
		{
			counter = 0;

			// ��� 8*8 �����ڲ��� 4*4 ����
			for (i = 0; i < 26; i++)
			{
				for (j = 0; j < 26; j++)
				{
					// ���� 8*8 ��� 4*4 ����, ��Ӫֵ��� box_8
					if (mBoxMarkStruct->box_8[i][j] != _EMPTY && mBoxMarkStruct->box_8[i][j] != CAMP_SIGN)
						SignBox_4(i, j, mBoxMarkStruct->box_8[i][j]);

					// ��յл����ֵ�����λ��
					if (i <= 1 && j <= 1 || j >= 12 && j <= 13 && i <= 1 || j >= 24 && i <= 1)
					{
						mBoxMarkStruct->box_8[i][j] = _EMPTY;
						SignBox_4(i, j, _EMPTY);
					}

					// ��λ�ò��ܻ���
					if (i >= 24 && j >= 12 && j <= 13)
					{
						mBoxMarkStruct->box_8[i][j] = CAMP_SIGN;
						//SignBox_4(i, j, CAMP_SIGN);
					}
				}
			}

			break;
		}

		if (GetAsyncKeyState(27) & 0x8000)
			break;

			/*M������: ��������������ͼif (GetAsyncKeyState('M') & 0x8000 && M_down == false)
			{
				M_down = true;
				if (mCMTImageX == lastx && mCMTImageY == lasty)
				{
					cur_index = cur_index + 1 > 13 ? 0 : cur_index + 1;
				}
				else
				{
					lastx = mCMTImageX;
					lasty = mCMTImageY;
				}

				// ���� 16*16 �ĵ�ͼ
				i = mCMTImageY / BOX_SIZE - 1;
				j = mCMTImageX / BOX_SIZE - 1;
				mBoxMarkStruct->box_8[i][j] = sign_order[cur_index][0];
				mBoxMarkStruct->box_8[i][j + 1] = sign_order[cur_index][1];
				mBoxMarkStruct->box_8[i + 1][j] = sign_order[cur_index][2];
				mBoxMarkStruct->box_8[i + 1][j + 1] = sign_order[cur_index][3];
			}else if ( !GetAsyncKeyState('M') & 0x8000 )
				M_down = false;*/

		// ��ɫ����
		StretchBlt(mCenter_hdc, 0, 0, CENTER_WIDTH, CENTER_HEIGHT, GetImageHDC(&mBlackBackgroundImage), 0, 0, CENTER_WIDTH, CENTER_HEIGHT, SRCCOPY);
		
		for (int i = 0; i < 26; i++)
		{
			for (int j = 0; j < 26; j++)
			{
				x = j * BOX_SIZE;// +CENTER_X;
				y = i * BOX_SIZE;// +CENTER_Y;
				switch (mBoxMarkStruct->box_8[i][j])
				{
				case _WALL:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mWallImage), 0, 0, SRCCOPY);
					break;
				case _FOREST:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mForestImage), 0, 0, SRCCOPY);
					break;
				case _ICE:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mIceImage), 0, 0, SRCCOPY);
					break;
				case _RIVER:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mRiverImage[0]), 0, 0, SRCCOPY);
					break;
				case _STONE:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mStoneImage), 0, 0, SRCCOPY);
					break;
				default:
					break;
				}
			}
		}

		// ��Ӫ
		TransparentBlt(mCenter_hdc, BOX_SIZE * 12, BOX_SIZE * 24, BOX_SIZE * 2, BOX_SIZE * 2,
			GetImageHDC(&mCamp[0]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

		// ̹���α�
		if(twinkle_counter++ / 28 % 2 == 0)
			TransparentBlt(mCenter_hdc, mCMTImageX - BOX_SIZE, mCMTImageY - BOX_SIZE, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&mCreateMapTankImage), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);

		// �����Ļ���ӡ�������� mImage_hdc ��
		BitBlt(mImage_hdc, CENTER_X, CENTER_Y, CENTER_WIDTH, CENTER_HEIGHT, mCenter_hdc, 0, 0, SRCCOPY);
		// ���Ż���������ʾ image ��������
		StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();
	}
	*isCreate = true;
	mHasCustomMap = true;
	return true;
}

void GameControl::GameLoop()
{
	MciSound::_PlaySound(S_START);
	CutStage();
	ShowStage();
	GameResult result = GameResult::Victory;

	while (result != GameResult::Fail)
	{
		result = StartGame();
		Sleep(1);
	}
}

/**********************************************
* ����ѭ����
* ����ˢ�£���ҡ��ӵ����л� ���еĸ��¶��ڴ�
* �������
**********************************************/
GameResult GameControl::StartGame()
{
	// ����ͼ����ʱ��
	if (mMainTimer.IsTimeOut())
	{
		// ʤ������ʧ�� ��ʾ�������
		if (mShowScorePanel)
		{
			BitBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&ScorePanel::background), 0, 0, SRCCOPY);
			for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
			{
				// ������������ʾ��
				if (!(*itor)->ShowScorePanel(mImage_hdc))
				{
					mShowScorePanel = false;

					// ʤ����ʧ�ܶ��ͷŵл���Դ
					for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
						delete *EnemyItor;
					EnemyList.clear();

					if (mWin)
					{
						Init();
						for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
						{
							if ( !(*itor)->IsLifeEnd() )
								(*itor)->Init();
						}

						// ��̬���ݻᱣ��,��Ҫ�ֶ�����
						EnemyBase::SetPause(false);

						mCurrentStage++;
						LoadMap();
						MciSound::_PlaySound(S_START);
						CutStage();
						ShowStage();
					}
					else
					{
						mCurrentStage = 1;
						mShowGameOverAfterScorePanel = true;
						MciSound::_PlaySound(S_FAIL);
					}
					break;
				}
			}

			// ���Ż���������ʾ image ��������
			StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
			FlushBatchDraw();
			return GameResult::Victory;
		}

		// ������ GAMEOVER ����
		if (mShowGameOverAfterScorePanel)
		{
			StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mBlackBackgroundImage),
				0, 0, CENTER_WIDTH, CENTER_HEIGHT, SRCCOPY);
			BitBlt(mImage_hdc, 66, msgoas_y, 124, 80, GetImageHDC(&msgoas_image), 0, 0, SRCCOPY);

			// ���Ż���������ʾ image ��������
			StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
			FlushBatchDraw();

			msgoas_y = msgoas_y - 2 > 66 ? msgoas_y - 2: 66;
			if (msgoas_y == 66)
				msgoas_counter++;

			if (msgoas_counter > 122)
			{
				mShowGameOverAfterScorePanel = false;
				msgoas_counter = 0;
				return GameResult::Fail;
			}
			return GameResult::Victory;
		}

		AddEnemy();

		// �����ұ���������, ���ж�, ��Ϊ����Ҫ�������� mImage_hdc
		RefreshRightPanel();

		// ����������Ϸ����: mCenter_hdc
		RefreshCenterPanel();

		// �����Ļ���ӡ�������� mImage_hdc ��
		BitBlt( mImage_hdc, CENTER_X, CENTER_Y, CENTER_WIDTH, CENTER_HEIGHT, mCenter_hdc, 0, 0, SRCCOPY );
		// ���Ż���������ʾ image ��������
		StretchBlt( mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY );
		FlushBatchDraw();
	}

	// ���ݱ仯, �����漰��ͼ����
	RefreshData();

	return GameResult::Victory;
}

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////// ˽�к���,����ʹ�� //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void GameControl::CutStage()
{
	// ��ɫ����
	StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT,
		GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);

	while (mCutStageCounter < 110)
	{
		Sleep(6);
		mCutStageCounter += 3;
		StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, mCutStageCounter, GetImageHDC(&mBlackBackgroundImage), 0, 0, CENTER_WIDTH, CENTER_HEIGHT, SRCCOPY);
		StretchBlt(mImage_hdc, 0, CANVAS_HEIGHT - mCutStageCounter, CANVAS_WIDTH, CANVAS_HEIGHT,
					GetImageHDC(&mBlackBackgroundImage), 0, 0, CENTER_WIDTH, CENTER_HEIGHT, SRCCOPY);

		StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
		FlushBatchDraw();
	}
}

void GameControl::ShowStage()
{
	// ��ɫ����
	StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT,
		GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);

	 
	TransparentBlt(mImage_hdc, 97, 103, 39, 7, GetImageHDC(&mCurrentStageImage), 0, 0, 39, 7, 0xffffff);

	// [1-9] �ؿ�����������
	if (mCurrentStage < 10)
		TransparentBlt(mImage_hdc, 157, 103, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE,
			GetImageHDC(&mBlackNumberImage), BLACK_NUMBER_SIZE * mCurrentStage, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
	else	// 10,11,12 .. ˫λ���ؿ�
	{
		TransparentBlt(mImage_hdc, 157, 103, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE,
			GetImageHDC(&mBlackNumberImage), BLACK_NUMBER_SIZE * (mCurrentStage / 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);

		TransparentBlt(mImage_hdc, 157, 103, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE,
			GetImageHDC(&mBlackNumberImage), BLACK_NUMBER_SIZE * (mCurrentStage % 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
	}
	StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
	FlushBatchDraw();

	Sleep(1300);

	MciSound::_PlaySound(S_BK);
}

//
void GameControl::ClearSignBox()
{
	// ��ʼ����Ǹ��ָ���
	int x = 0, y = 0;
	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			mBoxMarkStruct->prop_8[i][j] = _EMPTY;
			mBoxMarkStruct->box_8[i][j] = _EMPTY;	// 26*26
			SignBox_4(i, j, _EMPTY);		// ��� 26*26 �� 52*52 ����
		}
	}

	// ��Ǵ�Ӫ
	for (int i = 23; i < 26; i++)
	{
		for (int j = 11; j < 15; j++)
		{
			if (i >= 24 && j >= 12 && j <= 13)
				mBoxMarkStruct->box_8[i][j] = CAMP_SIGN;
			else
				mBoxMarkStruct->box_8[i][j] = _WALL;			// ����Χ�� _WALL
		}
	}
}

//
void GameControl::InitSignBox()
{
	// ��ʼ����Ǹ��ָ���
	int x = 0, y = 0;
	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			mBoxMarkStruct->prop_8[i][j] = _EMPTY;
			mBoxMarkStruct->box_8[i][j] = mMap.buf[i][j] - '0';	// 26*26
			SignBox_4(i, j, mMap.buf[i][j] - '0');		// ��� 26*26 �� 52*52 ����
		}
	}

	// ��Ǵ�Ӫ
	for (int i = 23; i < 26; i++)
	{
		for (int j = 11; j < 15; j++)
		{
			if (i >= 24 && j >= 12 && j <= 13)
				mBoxMarkStruct->box_8[i][j] = CAMP_SIGN;
			else
				mBoxMarkStruct->box_8[i][j] = _WALL;			// ����Χ�� _WALL
		}
	}
}

// ���޸�, ��ӵĵл�������Ҫ�޸�
void GameControl::AddEnemy()
{
	int size = EnemyList.size();

	if (mCurMovingTankNumber >= 6 || TOTAL_ENEMY_NUMBER - size <= 0)
		return;
	mCurMovingTankNumber++;

	int level;
	TANK_KIND kind;
	if (size < 8)
		level = 0;
	else if (size < 13)
		level = 1;
	else if (size < 17)
		level = 2;
	else
		level = 3;

	// ÿ��5�ܳ�һ�ܵ���̹��
	if (size % 5 == 4)
		kind = TANK_KIND::PROP;
	else
		kind = TANK_KIND::COMMON;

	switch (level)
	{
	case 0:
	case 1:
	case 2:
		//mOutedEnemyTankNumber++;
		switch (kind)
		{
			case 0: EnemyList.push_back((new PropTank(level, mBoxMarkStruct))); break;
			case 1: EnemyList.push_back((new CommonTank(level, mBoxMarkStruct))); break;
			default: break;
		}
		break;

	case 3:
		EnemyList.push_back(new BigestTank((TANK_KIND)kind, mBoxMarkStruct));
		//mOutedEnemyTankNumber++;
		break;
	default:
		break;
	}
}

// �ṩ8*8 �����Ͻ�����, �������4�� 4*4 �ĸ���
void GameControl::SignBox_4(int i, int j, int sign_val)
{
	int temp_i[4] = { 2 * i, 2 * i + 1, 2 * i, 2 * i + 1 };
	int temp_j[4] = { 2 * j, 2 * j, 2 * j + 1, 2 * j + 1 };

	for ( int i = 0; i < 4; i++ )
		mBoxMarkStruct->box_4[ temp_i[i] ][ temp_j[i] ] = sign_val;
}

// 
/***************************************************
* ���ݸ���, ���漰��ͼ����!!
* �������ж��������꣬��һ��ѭ�����н�������߻�ͼ��ʵ���˶�
****************************************************/
bool GameControl::RefreshData()
{
	if (GetAsyncKeyState(27) & 0x8000)
		return false;

	// �������Ƿ��� 'ʱ��' ��ֹ����
	if (PlayerBase::IsGetTimeProp())
	{
		mEnemyPause = true;
		mEnemyPauseTimer.Init();		// ���� t1 = t2
		//.mEnemyPauseCounter = 0;
		EnemyBase::SetPause(true);
	}

	// ѭ�����ӵ����߼�
	PlayerBase::IsGetShvelProp();

	// ��һ�õ��׵���
	for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
	{
		if ((*itor)->IsGetBombProp())
		{
			for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
			{
				if ((*EnemyItor)->BeKill(true))
				{
					mKillEnemyNum++;
					mCurMovingTankNumber--;

					// ��Ҽ�¼����ĵл�����
					(*itor)->AddKillEnemyNum((*EnemyItor)->GetLevel());

					if (mKillEnemyNum == 20)
					{
						mWinCounter = 0;
						mWin = true;
					}
				}
			}
		}
	}

	// ���, ���ܰ�����ͼ����! �ں���ʱ��
	for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
	{
		// �񳲱��������ֹͣ�ƶ�
		if (!mGameOverFlag)
			(*itor)->PlayerControl();

		// ����ӵ����н��
		BulletShootKind kind = (*itor)->BulletMoving(mCenter_hdc);
		switch (kind)
		{
		case BulletShootKind::Camp:
			mGameOverX = CENTER_WIDTH / 2 - GAMEOVER_WIDTH / 2;
			mGameOverY = CENTER_HEIGHT;
			mGameOverFlag = true;

			// ��Ӫ��ը
			mCampDie = true;
			mBlast.SetBlasting(11, 23);

			MciSound::_PlaySound(S_CAMP_BOMB);
			MciSound::PauseBk(true);
			MciSound::PlayMovingSound(false);
			break;

		// ���������е���� Ȼ����ͣ��
		case BulletShootKind::Player_1:
			for (list<PlayerBase*>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
			{
				if ((*i)->GetID() == 0)
					(*i)->SetPause();
			}
			break;

		// ���������е���� Ȼ����ͣ��
		case BulletShootKind::Player_2:
			for (list<PlayerBase*>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
			{
				if ((*i)->GetID() == 1)
					(*i)->SetPause();
			}
			break;
		default:
			break;
		}
	}

	// �л�, �˴����ܰ����ƻ�ͼ����, �ں���ʱ��, ��Ȼ�ǻᵼ�¼�ʱ��������ʱ����һ��,����ʧ֡
	for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
	{
		(*EnemyItor)->ShootBullet();

		int result = (*EnemyItor)->BulletMoving();
		switch (result)
		{
		case BulletShootKind::Player_1:
		case BulletShootKind::Player_2:
			for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
			{
				if ((*itor)->GetID() + PLAYER_SIGN == result)
				{
					(*itor)->BeKill();
					break;
				}
			}
			break;

		case BulletShootKind::Camp:

			// ���֮ǰû�����ù��� flag (��ұ�������hi���ø� flag, ��ʱ�����ٴ�����)
			if (mGameOverFlag == false)
			{
				mGameOverX = CENTER_WIDTH / 2 - GAMEOVER_WIDTH / 2;
				mGameOverY = CENTER_HEIGHT;
				mGameOverFlag = true;
			}

			// ��Ӫ��ը
			mCampDie = true;
			mBlast.SetBlasting(11, 23);

			MciSound::_PlaySound(S_CAMP_BOMB);
			MciSound::PauseBk(true);
			MciSound::PlayMovingSound(false);
			break;

		default:
			break;
		}

		// ����л���ͣ
		if (mEnemyPause == false)
		{
			(*EnemyItor)->TankMoving(mCenter_hdc);
		}
		else if (mEnemyPauseTimer.IsTimeOut())
		{
			mEnemyPause = false;
			(*EnemyItor)->SetPause(false);
		}
	}

	return true;
}

void GameControl::RefreshRightPanel()
{
	// ��ɫ����
	StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mGrayBackgroundImage), 0, 0, 66, 66, SRCCOPY);

	// ��ʾ�л�����ͼ��
	int x[2] = {233, 241};
	int n, index;
	for ( int i = 0; i < mRemainEnemyTankNumber; i++ )
	{
		n = i / 2;
		index = i % 2;

		TransparentBlt( mImage_hdc, x[index], 19 + n * 8, ENEMY_TANK_ICO_SIZE, ENEMY_TANK_ICO_SIZE, 
			GetImageHDC(&mEnemyTankIcoImage), 0, 0, ENEMY_TANK_ICO_SIZE, ENEMY_TANK_ICO_SIZE, 0xffffff );	// ע�����ͼ���к�ɫ����
	}
	
	for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
	{
		(*itor)->DrawPlayerTankIco(mImage_hdc);
	}

	// ����
	TransparentBlt(mImage_hdc, 232, 177, FLAG_ICO_SIZE_X, FLAG_ICO_SIZE_Y,
		GetImageHDC(&mFlagImage), 0, 0, FLAG_ICO_SIZE_X, FLAG_ICO_SIZE_Y, 0xffffff);	// ע��ͼ�����к�ɫ����

	// �ؿ�
	if (mCurrentStage < 10)
		TransparentBlt(mImage_hdc, 238, 193, 7, 7, GetImageHDC(&mBlackNumberImage),
			BLACK_NUMBER_SIZE * mCurrentStage, 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
	else	// 10,11,12 .. ˫λ���ؿ�
	{
		TransparentBlt(mImage_hdc, 233, 193, 7, 7, GetImageHDC(&mBlackNumberImage),
			BLACK_NUMBER_SIZE * (mCurrentStage / 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
		TransparentBlt(mImage_hdc, 241, 193, 7, 7, GetImageHDC(&mBlackNumberImage),
			BLACK_NUMBER_SIZE * (mCurrentStage % 10), 0, BLACK_NUMBER_SIZE, BLACK_NUMBER_SIZE, 0xffffff);
	}
}

// �����м���Ϸ����
void GameControl::RefreshCenterPanel()
{
		BitBlt(mCenter_hdc, 0, 0, CENTER_WIDTH, CENTER_HEIGHT, GetImageHDC(&mBlackBackgroundImage), 0, 0, SRCCOPY);// ���ĺ�ɫ������Ϸ��

	   // �Ľ�����˸����
		for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
		{
			// �����ǰ�л���û�г���, ���������һ�ܵл�
			Star_State result = (*EnemyItor)->ShowStar(mCenter_hdc, mRemainEnemyTankNumber);
			if (result != Star_State::Tank_Out)
				break;
		}

		/* ��ʼ���������ļ����Ƶ�ͼ
		* ����Ϊ BOX_SIZE x BOX_SIZE �ĸ���
		* x���꣺ j*BOX_SIZE
		* y���꣺ i*BOX_SIZE
		*/
		int x = 0, y = 0;
		for (int i = 0; i < 26; i++)
		{
			for (int j = 0; j < 26; j++)
			{
				x = j * BOX_SIZE;// +CENTER_X;
				y = i * BOX_SIZE;// +CENTER_Y;
				switch (mBoxMarkStruct->box_8[i][j])
				{
				case _WALL:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mWallImage), 0, 0, SRCCOPY);
					break;
				case _ICE:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mIceImage), 0, 0, SRCCOPY);
					break;
				case _RIVER:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mRiverImage[0]), 0, 0, SRCCOPY);
					break;
				case _STONE:
					BitBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mStoneImage), 0, 0, SRCCOPY);
					break;
				default:
					break;
				}
			}
		}

		// ��ⱻ���ٵ��ϰ���, ���ƺ�ɫͼƬ����
		for (int i = 0; i < 52; i++)
		{
			for (int j = 0; j < 52; j++)
			{
				if (mBoxMarkStruct->box_4[i][j] == _CLEAR)
				{
					BitBlt(mCenter_hdc, j * SMALL_BOX_SIZE, i * SMALL_BOX_SIZE, SMALL_BOX_SIZE, SMALL_BOX_SIZE,
						GetImageHDC(&mBlackBackgroundImage), 0, 0, SRCCOPY);
				}
			}
		}

		for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
		{
			(*itor) ->ShowStar(mCenter_hdc);
			(*itor)->DrawPlayerTank(mCenter_hdc);		// ̹��
			(*itor)->DrawBullet(mCenter_hdc);
			CheckKillEnemy(*itor);
			(*itor)->CheckShowGameOver(mCenter_hdc);
		}

		// �л�
		for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
		{
			(*EnemyItor)->DrawTank(mCenter_hdc);
			(*EnemyItor)->DrawBullet(mCenter_hdc);
		}

		// ɭ��
		for (int i = 0; i < 26; i++)
		{
			for (int j = 0; j < 26; j++)
			{
				x = j * BOX_SIZE;// +CENTER_X;
				y = i * BOX_SIZE;// +CENTER_Y;
				if (mBoxMarkStruct->box_8[i][j] == _FOREST)
					TransparentBlt(mCenter_hdc, x, y, BOX_SIZE, BOX_SIZE, GetImageHDC(&mForestImage), 0, 0, BOX_SIZE, BOX_SIZE, 0x000000);
			}
		}

		// �л��ӵ�\̹�˱�ըͼ, �����غ�
		for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
		{
			(*EnemyItor)->Bombing(mCenter_hdc);

			// ��ը���, �Ƴ��л�
			if ((*EnemyItor)->Blasting(mCenter_hdc))
			{
			}
		}

		// ����õл����д�Ӫ
		/*if (mGameOverFlag)
		{
			if (mBlast.canBlast == false)
			{
				int index[17] = { 0,0,0,1,1,2,2,3,3,4,4,4,4,3,2,1,0 };
				TransparentBlt(mCenter_hdc, 11 * BOX_SIZE, 23 * BOX_SIZE, BOX_SIZE * 4, BOX_SIZE * 4,
					GetImageHDC(&BlastStruct::image[index[mBlast.counter % 17]]), 0, 0, BOX_SIZE * 4, BOX_SIZE * 4, 0x000000);
				if ( mCampTimer.IsTimeOut() && mBlast.counter++ == 17)
					mBlast.canBlast = true;
				mCampDie = true;
			}
		}*/

		bool player_all_die = true;
		for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
		{
			(*itor)->Bombing(mCenter_hdc);
			if ((*itor)->Blasting(mCenter_hdc) == true)		// ��������ù�
			{
				// �����˫�����, ������һ����һ�û�б�����, ��ô��ǰ������������Ҿ���ʾ gameover ����
				if (PlayerList.size() == 2 && (PlayerList.front()->IsLifeEnd() == false || PlayerList.back()->IsLifeEnd() == false))
					(*itor)->SetShowGameover();
			}
			if (!(*itor)->IsLifeEnd())
				player_all_die = false;
		}

		// ��ұ�������
		if (player_all_die && mGameOverFlag == false)
		{
			mGameOverX = CENTER_WIDTH / 2 - GAMEOVER_WIDTH / 2;
			mGameOverY = CENTER_HEIGHT;
			mGameOverFlag = true;

			MciSound::PauseBk(true);
			MciSound::PlayMovingSound(false);
		}

		// ������˸, �ڲ��Զ���ʱ��
		PlayerBase::ShowProp(mCenter_hdc);

		// ��Ӫ
		if (!mCampDie)		// ���û��ը
		{
			TransparentBlt(mCenter_hdc, BOX_SIZE * 12, BOX_SIZE * 24, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&mCamp[0]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
		}
		else	// ��ʾ���ݻٵ�camp
		{
			TransparentBlt(mCenter_hdc, BOX_SIZE * 12, BOX_SIZE * 24, BOX_SIZE * 2, BOX_SIZE * 2,
				GetImageHDC(&mCamp[1]), 0, 0, BOX_SIZE * 2, BOX_SIZE * 2, 0x000000);
		}

		mBlast.CampBlasting(mCenter_hdc);

		IsWinOver();
		IsGameOver();
}

// ��ȡPlayerBase �ڵ�����, ����л�
void GameControl::CheckKillEnemy(PlayerBase* pb)
{
	int bullet[2] = {0, 0};
	pb->GetKillEnemy(bullet[0], bullet[1]);		// ��ȡ��һ��еĵл�id, �洢�� bullet[2] ��

	for (int i = 0; i < 2; i++)
	{
		if (bullet[i] >= ENEMY_SIGN )
		{
			for (list<EnemyBase*>::iterator EnemyItor = EnemyList.begin(); EnemyItor != EnemyList.end(); EnemyItor++)
			{
				if ((*EnemyItor)->GetId() == bullet[i] % 100)		// 100xx ����λ�� id
				{
					// �������л�
					if ((*EnemyItor)->BeKill(false))
					{
						mKillEnemyNum++;
						mCurMovingTankNumber--;
						if ((int)TANK_KIND::PROP == bullet[i] % 1000 / 100)		// ��ȡ�ٷ�λ�ĵл�����
							PlayerBase::SetShowProp();

						// ��Ҽ�¼����ĵл�����
						pb->AddKillEnemyNum((*EnemyItor)->GetLevel());
					}
					if (mKillEnemyNum == 20)
					{
						mWinCounter = 0;
						mWin = true;
					}
					break;
				}
			}
		}
	}
}
void GameControl::IsGameOver()
{
	if (!mGameOverFlag)
		return;

	TransparentBlt(mCenter_hdc, mGameOverX, mGameOverY, GAMEOVER_WIDTH, GAMEOVER_HEIGHT,
		GetImageHDC(&mGameOverImage), 0, 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, 0x000000);

	if (mGameOverTimer.IsTimeOut() && mGameOverY >= CENTER_HEIGHT * 0.45)
		mGameOverY -= 2;
	else if (mGameOverY < CENTER_HEIGHT * 0.45)
		mGameOverCounter++;

	if (mGameOverCounter > 250 && mShowScorePanel == false)
	{
		mShowScorePanel = true;
		mWin = false;
		for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
		{
			(*itor)->ResetScorePanelData(PlayerList.size(), mCurrentStage);
		}
	}
}

// ����л���������, �� mWinCounter ����ת���������
void GameControl::IsWinOver()
{
	if (mWin && mWinCounter++ > 210 && !mGameOverFlag && mShowScorePanel == false)
	{
		MciSound::PauseBk(true);
		mShowScorePanel = true;
		for (list<PlayerBase*>::iterator itor = PlayerList.begin(); itor != PlayerList.end(); itor++)
		{
			(*itor)->ResetScorePanelData(PlayerList.size(), mCurrentStage);
		}
	}
}

/*bool GameControl::ShowGameOverAfterScorePanel()
{
	if (!mShowGameOverAfterScorePanel || !msgoas_Timer.IsTimeOut())
		return true;

	StretchBlt(mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, GetImageHDC(&mBlackBackgroundImage),
		0, 0, CENTER_WIDTH, CENTER_HEIGHT, SRCCOPY);
	BitBlt(mImage_hdc, 60, msgoas_y, 124, 80, GetImageHDC(&msgoas_image), 0, 0, SRCCOPY);

	// ���Ż���������ʾ image ��������
	StretchBlt(mDes_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mImage_hdc, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, SRCCOPY);
	FlushBatchDraw();

	msgoas_y = msgoas_y - 2 > 50 ? msgoas_y : 50;
	if (msgoas_y == 50)
		msgoas_counter++;

	if (msgoas_counter > 30)
	{
		mShowGameOverAfterScorePanel = false;
		msgoas_counter = 0;
	}

	return true;
}*/
