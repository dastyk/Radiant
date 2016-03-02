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
		for (unsigned int i = 0; i < _weapons.size(); i++)
		{
			if (input->IsKeyDown(i + 49))
			{

				if (_currentWep != i)
				{
					if (_weapons[_currentWep]->HasAmmo())
					{
						_weapons[_currentWep]->setActive(false);
						_currentWep = i;
						_weapons[_currentWep]->setActive(true);
					}
				}


			}
		}
	});

	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);
	_builder->Transform()->BindChild(_camera, _weaponEntity);
	_builder->Transform()->SetPosition(_weaponEntity, XMFLOAT3(0.07f, -0.05f, 0.2f));

	_weapons.push_back(new BasicWeapon(_builder, _weaponEntity));
	_currentWep = 0;
}

Player::~Player()
{
	for(auto& w : _weapons)
	 SAFE_DELETE(w);

	while (_powers.Size())
	{
		_powers.RemoveCurrentElement();
	}
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



	_weapons[_currentWep]->Shoot();



	for (auto& w : _weapons)
	{
		w->Update(_camera, deltatime);

	
	}
	if (!_weapons[_currentWep]->HasAmmo())
	{
		_weapons[_currentWep]->setActive(false);
		_currentWep = 0;
		_weapons[_currentWep]->setActive(true);
	}


	for (int i = 0; i < _powers.Size(); i++)
	{
		_powers.GetCurrentElement()->Update(_camera, deltatime);
		_powers.MoveCurrent();
	}

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

	if (i->IsKeyPushed(VK_Q))
	{
		_ChangePower();
	}
	if (i->IsKeyPushed(VK_R))
	{
		unsigned int bef = _currentWep;
		_currentWep -= (_currentWep == _weapons.size() - 1)? _weapons.size() - 1 : -1;
		while (!_weapons[_currentWep]->HasAmmo())
			_currentWep -= (_currentWep == _weapons.size() - 1) ? _weapons.size() - 1 : -1;
		if (!(bef == _currentWep))
		{
			_weapons[bef]->setActive(false);
			_weapons[_currentWep]->setActive(true);
		}

	}
	if (i->IsKeyPushed(VK_E))
	{
		unsigned int bef = _currentWep;
		_currentWep += (_currentWep == 0) ? _weapons.size() - 1 : -1;
		while (!_weapons[_currentWep]->HasAmmo())
			_currentWep += (_currentWep == 0) ? _weapons.size() - 1 : -1;
		if (!(bef == _currentWep))
		{
			_weapons[bef]->setActive(false);
			_weapons[_currentWep]->setActive(true);
		}
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
		const vector<Projectile*>& pr2 = w->GetProjectiles();
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
	for (unsigned int i = 0; i < _weapons.size(); i++)
	{
		if (_weapons[i]->Type() == type)
		{
			_weapons[_currentWep]->setActive(false);
			_currentWep = i;
			_weapons[_currentWep]->setActive(true);
			_weapons[_currentWep]->AddAmmo();
			return;
		}
	}


	switch (type)
	{
	case 1:
		_weapons.push_back(new FragBombWeapon(_builder, _weaponEntity));
		break;
	case 2:
		_weapons.push_back(new RapidFireWeapon(_builder, _weaponEntity));
		break;
	case 3:
		_weapons.push_back(new ShotgunWeapon(_builder, _weaponEntity));
		break;
	case 4:
		_weapons.push_back(new BounceWeapon(_builder, _weaponEntity));
		break;
	default:
		break;
	}

	_weapons[_currentWep]->setActive(false);
	_currentWep = (uint)(_weapons.size() - 1);
	_weapons[_currentWep]->setActive(true);

	return void();
}

const void Player::AddPower(Power* power)
{
	if(_powers.Size())
		_powers.GetCurrentElement()->setActive(false);
	_powers.AddElementToList(power, 1);
	_powers.GetCurrentElement()->setActive(true);
}

const void Player::_ChangePower()
{
	if (_powers.Size())
	{
		_powers.GetCurrentElement()->setActive(false);
		_powers.MoveCurrent();
		_powers.GetCurrentElement()->setActive(true);
	}
}
