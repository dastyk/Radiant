#ifndef _PLAYER_H_
#define _PLAYER_H_
#pragma once

#include <DirectXMath.h>
#include "Camera.h"

class Player : public Camera
{
public:
	Player();
	~Player();

	/* Will update the players position based on where the camera is. */
	void Update(float milliseconds);

	void TurnLeft(float radians);
	void TurnRight(float radians);
	

	/* Direction based on which keys (WASD) are pushed down */
	void Jump(const DirectX::XMFLOAT2& directionXZ);
	void Dash(const DirectX::XMFLOAT2& directionXZ);



private:
	float _health;
	float _maxHealth;
	float _maxLight;
	float _currentLight;

	bool _activeJump;
	bool _activeDash;

	DirectX::XMFLOAT3 _position;





};

#endif
