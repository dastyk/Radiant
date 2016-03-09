#ifndef _GAME_OVER_STATE_H_
#define _GAME_OVER_STATE_H_

#pragma once
#include "State.h"
#include "Player.h"
class GameOverState :
	public State
{
public:
	GameOverState();
	GameOverState(Player* theLoser);
	~GameOverState();


	void Init();
	void Shutdown();


private:
	Player* _thePlayer;
};

#endif