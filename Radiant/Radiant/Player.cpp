#include "Player.h"

Player::Player(ManagerWrapper* managers) : _managers(managers)
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

	_activeJump && _DoJump(deltatime);
	_activeDash && _DoDash(deltatime);
}


void Player::_SetHeight(float deltatime)
{
	//Set y-pos back to non-offset position
	//float curY = DirectX::XMVectorGetY(_position);
	//DirectX::XMVectorSetY(_position, curY - _heightOffset);

	_managers->transform->MoveDown(_camera, _heightOffset);


	_heightFunctionArgument += deltatime;
	if (_heightFunctionArgument > DirectX::XM_2PI)
		_heightFunctionArgument -= DirectX::XM_2PI;

	_heightOffset = _WaveFunction(_heightFunctionArgument);
	//DirectX::XMVectorSetY(_position, DirectX::XMVectorGetY(_position) + _heightOffset);
	_managers->transform->MoveUp(_camera, _heightOffset);
}

float Player::_WaveFunction(float x)
{
	//One period every three seconds.
	x = (x / 3000) * DirectX::XM_2PI;
	return DirectX::XMScalarSin(x) / 2.0f;
}

bool Player::_DoJump(float deltatime)
{
	static float timeSoFar = 0.0f;
	timeSoFar += deltatime;

	float offset = DirectX::XMScalarSin((timeSoFar / _jumpTime) * DirectX::XM_PI);
	//DirectX::XMVectorSetY(_position, _yAtStartOfJump + offset);

	_managers->transform->MoveUp(_camera, offset);

	if(timeSoFar < _jumpTime)
		return true;

	timeSoFar = 0.0f;
	XMVECTOR pos = _managers->transform->GetPosition(_camera);
	DirectX::XMVectorSetY(pos, _yAtStartOfJump);
	_managers->transform->SetPosition(_camera, pos);
	return _activeJump = false;
}

bool Player::_DoDash(float deltatime)
{
	static float timeSoFar = 0.0f;
	timeSoFar += deltatime;

	float dt = timeSoFar / _dashTime;
	//Based on v(t) = 1 - t^2 to move fast initally and taper off
	float distanceMoved = _dashDistance * (dt - ((dt * dt * dt) / 3.0f));
	DirectX::XMVECTOR v = DirectX::XMVectorScale(_dashDir, distanceMoved);
	XMVECTOR pos = DirectX::XMVectorAdd(_posAtStartOfDash, v);
	
	_managers->transform->SetPosition(_camera, pos);

	if (timeSoFar < _dashTime)
		return true;

	return _activeDash = false;
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
	_yAtStartOfJump = XMVectorGetY( _managers->transform->GetPosition(_camera));
}

void Player::Dash(const DirectX::XMFLOAT2& directionXZ)
{
	if (_activeDash)
		return;
	if (_currentLight < _dashCost)
		return;
	_activeDash = true;
	_posAtStartOfDash = _managers->transform->GetPosition(_camera);
	_currentLight -= _dashCost;
	_dashDir = DirectX::XMVectorSet(directionXZ.x, 0.0f, directionXZ.y, 0.0f);
}
