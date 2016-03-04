#ifndef _PLAYER_H_
#define _PLAYER_H_
#pragma once

#include <DirectXMath.h>

#include "EntityBuilder.h"

#include "BasicWeapon.h"
#include "RapidFireWeapon.h"
#include "ShotgunWeapon.h"
#include "FragBombWeapon.h"
#include "BounceWeapon.h"

#include "Power.h"
#include "List.h"

#define MAXLIGHTINCREASE 16.0f
#define STARTLIGHT 4.0f

class Player
{
public:
	Player(EntityBuilder* builder);
	~Player();

	/* Will update the players position based on where the camera is. */
	void Update(float deltatime);
	void HandleInput(float deltatime);

	float GetHealth();
	void SetHealth(float value);
	void RemoveHealth(float amount);
	void AddHealth(float amount);

	void SetMaxLight(float max);
	void SetPosition(DirectX::XMVECTOR newPosition);

	
	void Jump();
	/* Direction based on which keys (WASD) are pushed down */
	void Dash(const DirectX::XMFLOAT2& directionXZ);

	void SetCamera();
	Entity GetEntity();
	vector<Projectile*> GetProjectiles();

	void SetEnemyLightPercent(float enemyPercent);

	const void AddWeapon(unsigned int type);
	const void AddPower(Power* power);

private:
	float _health;
	float _maxHealth;
	float _maxLight; //What the light can regenerate up to
	float _currentLight;
	float _lightRegenerationRate; //How many units of light per millisecond is regenerated
	float _speedFactor; //Factor of how fast player will move

	bool _activeJump;
	bool _activeDash;

	unsigned int _currentWep;
	std::vector< Weapon*> _weapons;
	List<Power> _powers;

	float _dashCost;//How much light it costs to dash
	float _dashTime; //How long a dash takes
	float _dashDistance; //How many units a dash moves you
	DirectX::XMFLOAT3 _posAtStartOfDash; //Helper for DoDash
	DirectX::XMFLOAT3 _dashDir; //Is set by Dash() method

	float _jumpTime; //How long a jump takes
	float _yAtStartOfJump; //Helper variable for the jumping method
	
	

	float _heightOffset; //Used for gently swaying up and down.
	float _heightFunctionArgument;
	void _SetHeight(float deltatime);
	float _WaveFunction(float x);//Any sinusoid with a period of 2PI
	const void _ChangePower();

	bool _DoJump(float deltatime);
	bool _DoDash(float deltatime);

	Entity _weaponEntity;
	Entity _camera;
	EntityBuilder* _builder = nullptr;
	float _pulseTimer;
	float _pulse;
};

#endif
