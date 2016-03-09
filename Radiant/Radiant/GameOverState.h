#ifndef _GAME_OVER_STATE_H
#define _GAME_OVER_STATE_H

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

};

#endif