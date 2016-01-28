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
	void Update(float deltatime);

	virtual void MoveForward(float amount);
	virtual void MoveBackward(float amount);
	virtual void MoveRight(float amount);
	virtual void MoveLeft(float amount);

	void TurnLeft(float radians);
	void TurnRight(float radians);
	
	float GetHealth();
	void SetHealth(float value);
	void RemoveHealth(float amount);
	void AddHealth(float amount);

	void SetMaxLight(float max);

	
	void Jump();
	/* Direction based on which keys (WASD) are pushed down */
	void Dash(const DirectX::XMFLOAT2& directionXZ);

private:
	float _health;
	float _maxHealth;
	float _maxLight; //What the light can regenerate up to
	float _currentLight;
	float _lightRegenerationRate; //How many units of light per millisecond is regenerated
	float _speedFactor; //Factor of how fast player will move

	bool _activeJump;
	bool _activeDash;

	float _dashCost;//How much light it costs to dash
	float _dashTime; //How long a dash takes
	float _dashDistance; //How many units a dash moves you
	DirectX::XMVECTOR _posAtStartOfDash; //Helper for DoDash
	DirectX::XMVECTOR _dashDir; //Is set by Dash() method

	float _jumpTime; //How long a jump takes
	float _yAtStartOfJump; //Helper variable for the jumping method
	
	

	float _heightOffset; //Used for gently swaying up and down.
	float _heightFunctionArgument;
	void _SetHeight(float deltatime);
	float _WaveFunction(float x);//Any sinusoid with a period of 2PI

	bool _DoJump(float deltatime);
	bool _DoDash(float deltatime);

};

#endif
