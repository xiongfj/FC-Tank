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
	case S_SHOOT:
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
