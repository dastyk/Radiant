#include "Player.h"

Player::Player()
{
	_health = 100.0f;
	_maxHealth = 100.0f;
	_maxLight = 100.0f;
	_currentLight = 100.0f;
	_lightRegenerationRate = 0.005f;
	_speedFactor = 1.0f;
	
	_heightOffset = 0.0f;
	_heightFunctionArgument = 0.0f;
	
	_activeJump = false;
	_activeDash = false;
	_jumpTime = 1000.0f;
	_dashTime = 500.0f;
	_dashCost = 10.0f;
	_dashDistance = 10.0f;
}

Player::~Player()
{

}

void Player::Update(float deltatime)
{

	//Swaying up and down when not jumping or dashing
	if (!_activeDash && !_activeJump)
	{
		_SetHeight(deltatime);
	}

	_activeJump && DoJump(deltatime);
	_activeDash && DoDash(deltatime);
}

void Player::MoveForward(float amount)
{
	Camera::MoveForward(amount * _speedFactor);
}

void Player::MoveBackward(float amount)
{
	Camera::MoveBackward(amount * _speedFactor);
}

void Player::MoveRight(float amount)
{
	Camera::MoveRight(amount * _speedFactor);
}

void Player::MoveLeft(float amount)
{
	Camera::MoveLeft(amount * _speedFactor);
}

void Player::_SetHeight(float deltatime)
{
	//Set y-pos back to non-offset position
	float curY = DirectX::XMVectorGetY(_position);
	DirectX::XMVectorSetY(_position, curY - _heightOffset);

	_heightFunctionArgument += deltatime;
	if (_heightFunctionArgument > DirectX::XM_2PI)
		_heightFunctionArgument -= DirectX::XM_2PI;

	_heightOffset = _WaveFunction(_heightFunctionArgument);
	DirectX::XMVectorSetY(_position, DirectX::XMVectorGetY(_position) + _heightOffset);
}

float Player::_WaveFunction(float x)
{
	//One period every three seconds.
	x = (x / 3000) * DirectX::XM_2PI;
	return DirectX::XMScalarSin(x) / 2.0f;
}

bool Player::DoJump(float deltatime)
{
	static float timeSoFar = 0.0f;
	timeSoFar += deltatime;

	float offset = DirectX::XMScalarSin((timeSoFar / _jumpTime) * DirectX::XM_PI);
	DirectX::XMVectorSetY(_position, _yAtStartOfJump + offset);

	if(timeSoFar < _jumpTime)
		return true;

	timeSoFar = 0.0f;
	DirectX::XMVectorSetY(_position, _yAtStartOfJump);
	return _activeJump = false;
}

bool Player::DoDash(float deltatime)
{
	static float timeSoFar = 0.0f;
	timeSoFar += deltatime;

	float smoothingFactor = 2.0f - (2.0f * timeSoFar);
	float unitsToMove = (_dashDistance / _dashTime) * deltatime * smoothingFactor;
	DirectX::XMVECTOR toAdd = DirectX::XMVectorSet(unitsToMove * _dashDir.x, 0.0f, unitsToMove * _dashDir.y, 0.0f);
	_position = DirectX::XMVectorAdd(_position, toAdd);
	if (timeSoFar < _dashTime)
		return true;

	return _activeDash = false;
}

void Player::TurnLeft(float radians)
{
	RotateYaw(-radians);
}

void Player::TurnRight(float radians)
{
	RotateYaw(radians);
}

float Player::GetHealth()
{
	return _health;
}

void Player::SetHealth(float value)
{
	_health = value;
}

void Player::RemoveHealth(float amount)
{
	_health -= amount;
}

void Player::AddHealth(float amount)
{
	_health += amount;
}

void Player::SetMaxLight(float max)
{
	_maxLight = max;
}

void Player::Jump()
{
	if (_activeJump)
		return;
	_activeJump = true;
	_yAtStartOfJump = DirectX::XMVectorGetY(_position);
}

void Player::Dash(const DirectX::XMFLOAT2& directionXZ)
{
	if (_activeDash)
		return;
	if (_currentLight < _dashCost)
		return;
	_activeDash = true;
	_currentLight -= _dashCost;
	DirectX::XMVECTOR v = DirectX::XMVectorSet(directionXZ.x, 0.0f, directionXZ.y, 0.0f);
	v = DirectX::XMVector3Normalize(v);
	_dashDir.x = DirectX::XMVectorGetX(v);
	_dashDir.y = DirectX::XMVectorGetZ(v);
}
