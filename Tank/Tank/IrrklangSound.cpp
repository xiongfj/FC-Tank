#include "stdafx.h"
#include "IrrklangSound.h"


/*void _PlaySound(const TCHAR* close1, const TCHAR* name, const TCHAR* type_name)
{
TCHAR c1[20], close[20], open[90], play[20];

if (close1 != NULL)
{
_stprintf_s(c1, _T("close %s"), close1);
mciSendString(c1, 0, 0, 0);
}

_stprintf_s(close, _T("close %s"), type_name);
_stprintf_s(open, _T("open ./res/music/%s alias %s"), name, type_name);
_stprintf_s(play, _T("play %s"), type_name);

mciSendString(close, 0, 0, 0);
mciSendString(open, 0, 0, 0);
mciSendString(play, 0, 0, 0);
}*/


void IrrklangSound::InitSounds()
{
	engine = createIrrKlangDevice();

	start = engine->addSoundSourceFromFile("./res/music/start.wav");
	shoot = engine->addSoundSourceFromFile("./res/music/shoot.wav");
	bin = engine->addSoundSourceFromFile("./res/music/bin.wav");
	bk = engine->addSoundSourceFromFile("./res/music/bk-sound.wav");
	getprop = engine->addSoundSourceFromFile("./res/music/get-prop.wav");
	propout = engine->addSoundSourceFromFile("./res/music/prop-out.wav");
	enemy_bomb = engine->addSoundSourceFromFile("./res/music/enemy-bomb.wav");
	//camp_bomb = engine->addSoundSourceFromFile("./res/music/camp_bomb.wav");
	scorepanel_di = engine->addSoundSourceFromFile("./res/music/scorepanel_di.wav");
	bunds1000 = engine->addSoundSourceFromFile("./res/music/bouns1000.wav");
	addlife = engine->addSoundSourceFromFile("./res/music/addlife.wav");
	//win = engine->addSoundSourceFromFile("./res/music/win.wav");
	fail = engine->addSoundSourceFromFile("./res/music/fail.wav");
	player_bomb = engine->addSoundSourceFromFile("./res/music/player_bomb.wav");
	player_move = engine->addSoundSourceFromFile("./res/music/player_move.wav");
}

void IrrklangSound::_PlaySound(int kind)
{
	switch (kind)
	{
	case S_START:
		if (start != 0)
			engine->play2D(start);
		break;
	case S_SHOOT0:
	if (shoot != 0)
		engine->play2D(shoot);
		break;
	case S_BIN:
	if (bin != 0)
		engine->play2D(bin);
		break;
	case S_BK:
		if ( bk != 0 )
		bk_control = engine->play2D(bk, true, true, true);		// 背景重复播放
		bk_control->setIsPaused(false);
		break;
	case S_GETPROP:
		if (getprop != 0)
			engine->play2D(getprop);
		break;
	case S_PROPOUT:
		if (propout != 0)
			engine->play2D(propout);
		break;
	case S_ENEMY_BOMB:
		if (enemy_bomb != 0)
			engine->play2D(enemy_bomb);
		break;
	case S_CAMP_BOMB:					// 公用一个声音
		if (player_bomb != 0)
			engine->play2D(player_bomb);
		break;
	case S_SCOREPANEL_DI:
		if (scorepanel_di != 0)
			engine->play2D(scorepanel_di);
		break;
	case S_BOUNS1000:
		if (bunds1000 != 0)
			engine->play2D(bunds1000);
		break;
	case S_ADDLIFE:
		if (addlife != 0)
			engine->play2D(addlife);
		break;
	//case S_WIN:
	//	if (win != 0)
			//engine->play2D(win);
		//break;
	case S_FAIL:
		if (fail != 0)
			engine->play2D(fail);
		break;
	case S_PLAYER_BOMB:
		if (player_bomb != 0)
			engine->play2D(player_bomb);
		break;
	case S_PLAYER_MOVE:
		if (player_move != 0)
			player_move_control = engine->play2D(player_move, true, true, true);		// 必须是三个 true 才能返回 ISound* 不然异常!!
		break;

	default:
		break;
	}
}

void IrrklangSound::PauseBk(bool paused )
{
	if (bk_control)
		bk_control->setIsPaused(paused);
}

void IrrklangSound::PauseMove(bool paused)
{
	if (player_move_control)
		player_move_control->setIsPaused(paused);
}

ISoundEngine* IrrklangSound::engine = 0;
ISound* IrrklangSound::bk_control = 0;
ISound* IrrklangSound::player_move_control = 0;

ISoundSource* IrrklangSound::start = 0;
ISoundSource* IrrklangSound::shoot = 0;
ISoundSource* IrrklangSound::bin = 0;
ISoundSource* IrrklangSound::bk = 0;
ISoundSource* IrrklangSound::getprop = 0;
ISoundSource* IrrklangSound::propout = 0;
ISoundSource* IrrklangSound::enemy_bomb = 0;
//ISoundSource* IrrklangSound::camp_bomb = 0;
ISoundSource* IrrklangSound::scorepanel_di = 0;
ISoundSource* IrrklangSound::bunds1000 = 0;
ISoundSource* IrrklangSound::addlife = 0;
//ISoundSource* IrrklangSound::win = 0;
ISoundSource* IrrklangSound::fail = 0;
ISoundSource* IrrklangSound::player_bomb = 0;
ISoundSource* IrrklangSound::player_move = 0;


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
	mciSendString(_T("open ./res/music/bk_sound.mp3 alias bk_sound"), NULL, 0, NULL);
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
	if (paused)
		mciSendString(_T("pause bk_sound"), NULL, 0, NULL);
	else
		mciSendString(_T("play bk_sound from 0"), NULL, 0, NULL);
}

void MciSound::PauseMove(bool paused)
{
	if (paused)
		mciSendString(_T("pause player_move"), NULL, 0, NULL);
	else
		mciSendString(_T("play player_move from 0"), NULL, 0, NULL);
}
