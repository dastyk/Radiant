#include "Player.h"

Player::Player()
{
	
}

Player::~Player()
{

}

void Player::Update(float milliseconds)
{

}

void Player::TurnLeft(float radians)
{
	RotateYaw(-radians);
}

void Player::TurnRight(float radians)
{
	RotateYaw(radians);
}



void Player::Jump(const DirectX::XMFLOAT2& directionXZ)
{
	if (_activeJump)
		return;
	_activeJump = true;
}

void Player::Dash(const DirectX::XMFLOAT2& directionXZ)
{
	if (_activeDash)
		return;
	_activeDash = true;
}
