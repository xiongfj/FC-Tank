#include "stdafx.h"
#include "MciSound.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void MciSound::InitSounds()
{ 
	mciSendString(_T("open ./res/music/start.wav alias			start"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/shoot.wav alias			shoot0"), NULL, 0, NULL);		// 一个玩家两个子弹
	mciSendString(_T("open ./res/music/shoot.wav alias			shoot1"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/shoot.wav alias			shoot2"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/shoot.wav alias			shoot3"), NULL, 0, NULL);

	mciSendString(_T("open ./res/music/bin.wav alias bin"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/bk_sound.wav alias bk_sound"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/get-prop.wav alias get_prop"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/enemy-bomb.wav alias		enemy_bomb"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/scorepanel_di.wav alias scorepanel_di"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/bouns1000.wav alias bouns1000"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/addlife.wav alias		addlife"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/fail.wav alias			fail"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/player_bomb.wav alias	player_bomb"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/player_move.wav alias	player_move"), NULL, 0, NULL);
	mciSendString(_T("open ./res/music/prop-out.wav alias		prop_out"), NULL, 0, NULL);
}

void MciSound::CloseSounds()
{
	mciSendString(_T("close	start"), NULL, 0, NULL);
	mciSendString(_T("close	shoot0"), NULL, 0, NULL);
	mciSendString(_T("close	shoot1"), NULL, 0, NULL);
	mciSendString(_T("close	shoot2"), NULL, 0, NULL);
	mciSendString(_T("close	shoot3"), NULL, 0, NULL);
	mciSendString(_T("close	bin"), NULL, 0, NULL);
	mciSendString(_T("close	bk_sound"), NULL, 0, NULL);
	mciSendString(_T("close	get_prop"), NULL, 0, NULL);
	mciSendString(_T("close	enemy_bomb"), NULL, 0, NULL);
	mciSendString(_T("close	scorepanel_di"), NULL, 0, NULL);
	mciSendString(_T("close	bouns1000"), NULL, 0, NULL);
	mciSendString(_T("close	addlife"), NULL, 0, NULL);
	mciSendString(_T("close	fail"), NULL, 0, NULL);
	mciSendString(_T("close	player_bomb"), NULL, 0, NULL);
	mciSendString(_T("close	player_move"), NULL, 0, NULL);
	mciSendString(_T("close	prop_out"), NULL, 0, NULL);
}

void MciSound::_PlaySound(int kind)
{
	switch (kind)
	{
	case S_START:
		mciSendString(_T("play start from 0"), NULL, 0, NULL);
		break;
	case S_SHOOT0:
		mciSendString(_T("play shoot0 from 0"), NULL, 0, NULL);
		break;
	case S_SHOOT1:
		mciSendString(_T("play shoot1 from 0"), NULL, 0, NULL);
		break;
	case S_SHOOT2:
		mciSendString(_T("play shoot2 from 0"), NULL, 0, NULL);
		break;
	case S_SHOOT3:
		mciSendString(_T("play shoot3 from 0"), NULL, 0, NULL);
		break;
	case S_BIN:
		mciSendString(_T("play bin from 0"), NULL, 0, NULL);
		break;
	case S_BK:
		mciSendString(_T("play bk_sound repeat"), NULL, 0, NULL);
		break;
	case S_GETPROP:
		mciSendString(_T("play get_prop from 0"), NULL, 0, NULL);
		break;
	case S_PROPOUT:
		mciSendString(_T("play prop_out from 0"), NULL, 0, NULL);
		break;
	case S_ENEMY_BOMB:
		mciSendString(_T("play enemy_bomb from 0"), NULL, 0, NULL);
		break;
	case S_CAMP_BOMB:
		mciSendString(_T("play player_bomb from 0"), NULL, 0, NULL);
		break;
	case S_SCOREPANEL_DI:
		mciSendString(_T("play scorepanel_di from 0"), NULL, 0, NULL);
		break;
	case S_BOUNS1000:
		mciSendString(_T("play bouns1000 from 0"), NULL, 0, NULL);
		break;
	case S_ADDLIFE:
		mciSendString(_T("play addlife from 0"), NULL, 0, NULL);
		break;
	case S_FAIL:
		mciSendString(_T("play fail from 0"), NULL, 0, NULL);
		break;
	case S_PLAYER_BOMB:
		mciSendString(_T("play player_bomb from 0"), NULL, 0, NULL);
		break;
	case S_PLAYER_MOVE:
		mciSendString(_T("play player_move from 0"), NULL, 0, NULL);
		break;

	default:
		break;
	}
}

void MciSound::PauseBk(bool paused)
{
	/*if (paused)
		mciSendString(_T("pause bk_sound"), NULL, 0, NULL);
	else
		mciSendString(_T("play bk_sound from 0"), NULL, 0, NULL);*/
}

void MciSound::PlayMovingSound(bool play)
{
	if (!play)
		mciSendString(_T("pause player_move"), NULL, 0, NULL);
	else
		mciSendString(_T("play player_move from 0"), NULL, 0, NULL);
}
