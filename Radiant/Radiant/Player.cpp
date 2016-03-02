#include "Player.h"
#include "System.h"

Player::Player(EntityBuilder* builder) : _builder(builder)
{
	_health = 100.0f;
	_maxHealth = 100.0f;
	_maxLight = STARTLIGHT;
	_currentLight = STARTLIGHT;
	_lightRegenerationRate = 5.0f;
	_speedFactor = 2.5f;
	
	_heightOffset = 0.5f;
	_heightFunctionArgument = 0.0f;
	
	_activeJump = false;
	_activeDash = false;
	_jumpTime = 1.0f;
	_dashTime = 0.5f;
	_dashCost = 10.0f;
	_dashDistance = 10.0f;
	_yAtStartOfJump = 0.0f;
	_pulse = 0.0f;
	_pulseTimer = 0.0f;

	_camera = _builder->CreateCamera(XMVectorSet(0.0f, 0.5f, 0.0f, 0.0f));
	_builder->Light()->BindPointLight(_camera, XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.3f, 0.5f, 0.8f), 10.0f);
	_builder->GetEntityController()->Light()->SetAsVolumetric(_camera, false);
	_builder->CreateImage(XMFLOAT3(System::GetOptions()->GetScreenResolutionWidth() / 2.0f - 5, System::GetOptions()->GetScreenResolutionHeight() / 2.0f - 5, 0), 10, 10, "Assets/Textures/default_color.png");
	//_builder->Light()->BindSpotLight(_camera, XMFLOAT3(1.0f, 0.0f, 0.0f), 10.0f, XMConvertToRadians(40.0f), XMConvertToRadians(20.0f), 20.0f);

	_builder->Bounding()->CreateBoundingSphere(_camera, 0.3f);
	_builder->GetEntityController()->Transform()->SetFlyMode(_camera, false);


	auto input = System::GetInput();

	_builder->GetEntityController()->BindEventHandler(_camera, EventManager::Type::Object);
	_builder->GetEntityController()->BindEvent(_camera, EventManager::EventType::Update, 
		[this, input]()
	{
		for (auto& w : _weapons)
		{
			if (w.second != _weapon)
			{
				if (w.second->HasAmmo())
				{
					if (input->IsKeyDown(w.first + 49))
					{
						_weapon->setActive(false);
						_weapon = w.second;
						_weapon->setActive(true);
					}
				}
			}
		}
	});

	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);
	_builder->Transform()->BindChild(_camera, _weaponEntity);
	_builder->Transform()->SetPosition(_weaponEntity, XMFLOAT3(0.07f, -0.05f, 0.2f));

	_weapon = new BasicWeapon(_builder, _weaponEntity);
	_weapons[0] = _weapon;
}

Player::~Player()
{
	for(auto& w : _weapons)
	 SAFE_DELETE(w.second);

	SAFE_DELETE(_power);

}

void Player::Update(float deltatime)
{

	_builder->Transform()->RotateYaw(_weaponEntity, -60 * deltatime);
	//_builder->Transform()->RotateRoll(_weaponEntity, 60 * deltatime);

	//Swaying up and down when not jumping or dashing <---Need to be rewritten. Sorry, Jimbo!
	if (!_activeDash && !_activeJump)
	{
	//	_SetHeight(deltatime); 
	}

	_activeJump && _DoJump(deltatime);
	_activeDash && _DoDash(deltatime);



	_weapon->Shoot();


	if (!_weapon->HasAmmo())
	{
		_weapon->setActive(false);
		_weapon = _weapons[0];
		_weapon->setActive(true);
	}

	for (auto& w : _weapons)
	{
		w.second->Update(_camera, deltatime);

	
	}


	if(_power != nullptr)
		_power->Update(_camera, deltatime);

	_currentLight += _lightRegenerationRate * deltatime;

	if (_currentLight > _maxLight)
		_currentLight = _maxLight;

	//_builder->Light()->ChangeLightRange(_camera, _currentLight);
}

void Player::HandleInput(float deltatime)
{

	auto i = System::GetInput();
	int x, y;

	XMVECTOR moveVec = XMVectorZero();
	XMVECTOR forward = _builder->GetEntityController()->Transform()->GetDirection(_camera);
	XMVECTOR right = _builder->GetEntityController()->Transform()->GetRight(_camera);
	XMVECTOR up = _builder->GetEntityController()->Transform()->GetUp(_camera);
	bool change = false;
	i->GetMouseDiff(x, y);
	if (x != 0)
		_builder->GetEntityController()->Transform()->RotateYaw(_camera, x  * 0.1f);
	if (y != 0)
		_builder->GetEntityController()->Transform()->RotatePitch(_camera, y  * 0.1f);


	if (i->IsKeyDown(VK_W))
	{
		moveVec += forward;
		change = true;
	}
	if (i->IsKeyDown(VK_S))
	{
		moveVec -= forward;
		change = true;
	}
	if (i->IsKeyDown(VK_A))
	{
		moveVec -= right;
		change = true;
	}
	if (i->IsKeyDown(VK_D))
	{
		moveVec += right;
		change = true;
	}
	if (i->IsKeyDown(VK_SHIFT))
	{
		moveVec += up;
		change = true;
	}
	if (i->IsKeyDown(VK_CONTROL))
	{
		moveVec -= up;
		change = true;
	}

	if (change)
		_builder->GetEntityController()->Transform()->MoveAlongVector(_camera, XMVector3Normalize(moveVec),_speedFactor*deltatime);

	if (System::GetInput()->IsKeyDown(VK_SPACE))
	{
		//Dash(XMFLOAT2(1.0f, 0.0f));
		Jump();
	}

}

void Player::_SetHeight(float deltatime)
{
	//Set y-pos back to non-offset position
	//float curY = DirectX::XMVectorGetY(_position);
	//DirectX::XMVectorSetY(_position, curY - _heightOffset);

	_builder->GetEntityController()->Transform()->MoveDown(_camera, _heightOffset);


	_heightFunctionArgument += deltatime;
	if (_heightFunctionArgument > DirectX::XM_2PI)
		_heightFunctionArgument -= DirectX::XM_2PI;

	_heightOffset = _WaveFunction(_heightFunctionArgument);
	//DirectX::XMVectorSetY(_position, DirectX::XMVectorGetY(_position) + _heightOffset);
	_builder->GetEntityController()->Transform()->MoveUp(_camera, _heightOffset);
}

float Player::_WaveFunction(float x)
{
	//One period every three seconds.
	x = x * DirectX::XM_2PI;
	return DirectX::XMScalarSin(x) / 2.0f;
}

bool Player::_DoJump(float deltatime)
{
	static float timeSoFar = 0.0f;
	timeSoFar += deltatime;

	XMVECTOR pos = _builder->GetEntityController()->Transform()->GetPosition(_camera);
	float offset = DirectX::XMScalarSin((timeSoFar / _jumpTime) * DirectX::XM_PI) * 1.25f;
	//DirectX::XMVectorSetY(_position, _yAtStartOfJump + offset);

	//_builder->GetEntityController()->Transform()->MoveUp(_camera, offset);
	_builder->GetEntityController()->Transform()->SetPosition(_camera, XMVectorSetY(pos, _yAtStartOfJump + offset));

	if(timeSoFar < _jumpTime)
		return true;

	timeSoFar = 0.0f;
	
	pos = DirectX::XMVectorSetY(pos, _yAtStartOfJump);
	_builder->GetEntityController()->Transform()->SetPosition(_camera, pos);
	_activeJump = false;
	return false;
}

bool Player::_DoDash(float deltatime)
{
	static float timeSoFar = 0.0f;
	timeSoFar += deltatime;

	float dt = timeSoFar / _dashTime;
	//Based on v(t) = 1 - t^2 to move fast initally and taper off
	float distanceMoved = _dashDistance * (dt - ((dt * dt * dt) / 3.0f));
	DirectX::XMVECTOR v = DirectX::XMVectorScale(XMLoadFloat3(& _dashDir), distanceMoved);
	XMVECTOR pos = DirectX::XMVectorAdd(XMLoadFloat3(&_posAtStartOfDash), v);
	
	_builder->GetEntityController()->Transform()->SetPosition(_camera, pos);

	if (timeSoFar < _dashTime)
		return true;
	timeSoFar = 0.0f;
	_activeDash = false;
	return false;
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
	_yAtStartOfJump = XMVectorGetY(_builder->GetEntityController()->Transform()->GetPosition(_camera));
}

void Player::Dash(const DirectX::XMFLOAT2& directionXZ)
{
	if (_activeDash)
		return;
	if (_currentLight < _dashCost)
		return;
	_activeDash = true;
	XMStoreFloat3(&_posAtStartOfDash, _builder->GetEntityController()->Transform()->GetPosition(_camera));
	_currentLight -= _dashCost;

	XMStoreFloat3(&_dashDir, DirectX::XMVectorSet(directionXZ.x, 0.0f, directionXZ.y, 0.0f));
}

void Player::SetCamera()
{
	_builder->GetEntityController()->Camera()->SetActivePerspective(_camera);
}

void Player::SetPosition(XMVECTOR newPosition)
{
	_builder->GetEntityController()->Transform()->SetPosition(_camera, newPosition);
}

Entity Player::GetEntity()
{
	return _camera;
}

vector<Projectile*> Player::GetProjectiles()
{
	vector<Projectile*> pr;
	for (auto& w : _weapons)
	{
		const vector<Projectile*>& pr2 = w.second->GetProjectiles();
		pr.insert(pr.end(), pr2.begin(), pr2.end());
	}
	return pr;
}

void Player::SetEnemyLightPercent(float enemyPercent)
{
	_maxLight = STARTLIGHT + MAXLIGHTINCREASE * (1.0f - enemyPercent);
}

const void Player::AddWeapon(unsigned int type)
{
	auto got = _weapons.find(type);
	if (got == _weapons.end())
	{
		switch (type)
		{
		case 1:
			_weapons[type] = new FragBombWeapon(_builder, _weaponEntity);
			break;
		case 2:
			_weapons[type] = new RapidFireWeapon(_builder, _weaponEntity);
			break;
		case 3:
			_weapons[type] = new ShotgunWeapon(_builder, _weaponEntity);
			break;
		default:
			break;
		}
	}
	else
	{
		_weapons[type]->AddAmmo();
	}
	if (_weapons[type] != _weapon)
	{
		_weapon->setActive(false);
		_weapon = _weapons[type];
		_weapon->setActive(true);
	}
	return void();
}

const void Player::SetPower(Power* power)
{
	SAFE_DELETE(_power);
	_power = power;
}