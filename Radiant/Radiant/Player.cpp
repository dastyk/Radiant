#include "Player.h"
#include "System.h"
#include "PowersHeader.h"

#define BAR_MAXSIZE 500

Player::Player(EntityBuilder* builder) : _builder(builder)
{
	XMFLOAT4 TextColor = XMFLOAT4(0.56f, 0.26f, 0.15f, 1.0f);
	_health = 100.0f;
	_maxHealth = 100.0f;
	_maxLight = STARTLIGHT;
	_lightDownBy = 0.0f;
	_currentLight = _maxLight;
	_lightRegenerationRate = 0.5f;
	_speedFactor = 2.5f;
	_heightOffset = 0.5f;
	_heightFunctionArgument = 0.0f;
	
	_activeJump = false;
	_activeDash = false;
	_jumpTime = 1.0f;
	_dashTime = 0.25f;
	_dashCost = 2.0f;
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

	_screenPercentHeight = System::GetOptions()->GetScreenResolutionHeight() / 1080.0f;
	_screenPercentWidth = System::GetOptions()->GetScreenResolutionWidth() / 1920.0f;
	float fd = fmin(System::GetOptions()->GetFoV(), 90.0f) / 90.0f;


	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);
	_builder->Transform()->BindChild(_camera, _weaponEntity);
	_builder->Transform()->SetPosition(_weaponEntity, XMFLOAT3(0.20f*_screenPercentWidth*fd*fd*fd, -0.11f*_screenPercentHeight*fd*fd*fd, 0.2f));

	_currentWep = Weapons(Weapons::Basic);
	_weapons[_currentWep] = new BasicWeapon(_builder, _weaponEntity);


	_totalLightCollected = 0;
	_shotsFired = 0;
	_shotsHit = 0;
	_enemiesDefeated = 0;


	_llvl = _builder->CreateLabel(
		XMFLOAT3(0.0f, System::GetOptions()->GetScreenResolutionHeight() - 95.0f*_screenPercentHeight, 0.0f),
		"Light Level ",
		40 * _screenPercentWidth,
		TextColor,
		300.0f*_screenPercentWidth,
		50.0f*_screenPercentHeight,
		"");

	_builder->Text()->ChangeFontSize(_llvl, (uint)(40 * _screenPercentWidth));

	float textWidth = _builder->Text()->GetLength(_llvl);

	_lightBarBorder = _builder->CreateOverlay(
		XMFLOAT3(0.0f, System::GetOptions()->GetScreenResolutionHeight() - 60.0f*_screenPercentHeight, 0.0f),
		BAR_MAXSIZE*_screenPercentWidth+20.0f*_screenPercentWidth,
		60.0f*_screenPercentHeight,
		"Assets/Textures/Light_Bar_Border3.png");

	_lightBar = _builder->CreateOverlay(
		XMFLOAT3(10.0f*_screenPercentWidth, System::GetOptions()->GetScreenResolutionHeight() - 50.0f*_screenPercentHeight, 0.0f),
		(_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth,
		40.0f*_screenPercentHeight,
		"Assets/Textures/Light_Bar.png");

	_currentLightIndicator = _builder->CreateOverlay(
		XMFLOAT3((_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth + 10.0f*_screenPercentWidth, System::GetOptions()->GetScreenResolutionHeight() - 50.0f*_screenPercentHeight, 0.0f),
		5.0f*_screenPercentWidth,
		40.0f*_screenPercentHeight,
		"Assets/Textures/Light_Indicator.png");

	
	_builder->Animation()->CreateAnimation(_currentLightIndicator, "update", 0.5f,
		[this](float delta, float amount, float offset)
	{
		_builder->Transform()->MoveRight(_currentLightIndicator, delta);
	});

	_builder->Camera()->SetViewDistance(_camera, (_currentLight / 20.0f)*15.0f + 3.0f);
	_builder->Light()->ChangeLightRange(_camera, (_currentLight / 20.0f)*15.0f + 1.0f);


	_builder->Animation()->CreateAnimation(_camera, "dash", _dashTime,
		[this](float delta, float amount, float offset)
	{
		_builder->Transform()->MoveAlongVector(_camera, XMLoadFloat3(&_dashDir), delta);
	},
	[this]()
	{
		_activeDash = false;
	});


}

Player::~Player()
{
	for(auto& w : _weapons)
	 SAFE_DELETE(w.second);

	while (_powers.Size())
	{
		_powers.RemoveCurrentElement();
	}
}

void Player::ResetPlayerForLevel(bool hardcoreMode)
{
	if (!hardcoreMode)
	{
		_health = _maxHealth;
	}
	_currentLight = STARTLIGHT;
	_maxLight = STARTLIGHT;
	_lightDownBy = 0.0f;

	float textWidth = _builder->Text()->GetLength(_llvl);

	_builder->Transform()->SetPosition(_lightBar, XMFLOAT3(10.0f*_screenPercentWidth, System::GetOptions()->GetScreenResolutionHeight() - 50.0f*_screenPercentHeight, 0.0f));
	_builder->Transform()->SetPosition(_currentLightIndicator, XMFLOAT3((_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth + 10.0f*_screenPercentWidth, System::GetOptions()->GetScreenResolutionHeight() - 50.0f*_screenPercentHeight, 0.0f));
	_builder->Overlay()->SetExtents(_lightBar, (_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth, 40.0f*_screenPercentHeight);
}

void Player::Update(float deltatime)
{
	if(System::GetOptions()->GetWeaponMode() == 0)
		_builder->Transform()->RotateYaw(_weaponEntity, -60 * deltatime);

	_activeJump && _DoJump(deltatime);

	for (auto& w : _weapons)
	{
		w.second->Update(_camera, deltatime);	
	}


	for (int i = 0; i < _powers.Size(); i++)
	{
		_powers.GetCurrentElement()->Update(_camera, deltatime);
		_powers.MoveCurrent();
	}
	if (_lightDownBy > 0.0f)
	{
		_currentLight -= (_lightDownBy + 10.0f) * deltatime ;
		_lightDownBy -= (_lightDownBy + 10.0f) * deltatime;
		if (_lightDownBy < 0.0f)
		{
			_currentLight -= _lightDownBy;
			_lightDownBy = 0.0f;
		}
	}
	if (_currentLight < _maxLight)
	{
		
		_currentLight += _lightRegenerationRate * deltatime;

		if (_currentLight > _maxLight)
			_currentLight = _maxLight;

		_builder->Overlay()->SetExtents(_lightBar, (_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth, 40.0f*_screenPercentHeight);
		_builder->Camera()->SetViewDistance(_camera, (_currentLight / 20.0f)*15.0f + 3.0f);
		_builder->Light()->ChangeLightRange(_camera, (_currentLight / 20.0f)*15.0f + 1.0f);

	}


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
	//if (i->IsKeyDown(VK_SHIFT))
	//{
	//	moveVec += up;
	//	change = true;
	//}
	//if (i->IsKeyDown(VK_CONTROL))
	//{
	//	moveVec -= up;
	//	change = true;
	//}
	if (change)
	{
		if (i->IsKeyDown(VK_SHIFT) && _currentLight-_dashCost >= 0.0f && !_activeDash)
		{
			_activeDash = true;
			_lightDownBy += _dashCost;
			XMStoreFloat3(&_dashDir, XMVector3Normalize(moveVec));
			_builder->Animation()->PlayAnimation(_camera, "dash", 2.0f);
		}
		else
			_builder->GetEntityController()->Transform()->MoveAlongVector(_camera, XMVector3Normalize(moveVec), _speedFactor*deltatime);



	}
	if (i->IsKeyPushed(VK_Q))
	{
		_ChangePower();
	}


	if (i->IsMouseKeyDown(VK_LBUTTON))
	{
		if (_weapons[_currentWep]->Shoot(_camera))
		{
			_shotsFired++;
		}
	}

	if (!_weapons[_currentWep]->HasAmmo())
	{
		_weapons[_currentWep]->setActive(false);
		_currentWep = Weapons(Weapons::Basic);
		_weapons[_currentWep]->setActive(true);
	}



	int sde = 0;
	if (i->IsScrollUp(sde))
	{
		Weapons c = _currentWep << 1;
		if (c._flags == 0)
			c._flags = 1;
		auto& find = _weapons.find(c);

		while (find == _weapons.end() || !find->second->HasAmmo())
		{
			c._flags = c._flags << 1;
			if (c._flags == 0)
				c._flags = 1;
			find = _weapons.find(c);
		}
		if (!(c == _currentWep))
		{
			_weapons[_currentWep]->setActive(false);
			_currentWep = c;
			_weapons[_currentWep]->setActive(true);
		}
	}
	if(i->IsScrollDown(sde))
	{
		Weapons c = _currentWep >> 1;
		if (c._flags == 0)
			c._flags = 1 << (sizeof(unsigned int)*8-1);
		auto& find = _weapons.find(c);

		while (find == _weapons.end() || !find->second->HasAmmo())
		{
			c._flags = c._flags >> 1;
			if (c._flags == 0)
				c._flags = 1 << (sizeof(unsigned int) * 8 - 1);
			find = _weapons.find(c);
		}
		if (!(c == _currentWep))
		{
			_weapons[_currentWep]->setActive(false);
			_currentWep = c;
			_weapons[_currentWep]->setActive(true);
		}
	}

	if (System::GetInput()->IsKeyDown(VK_SPACE))
	{
		//Dash(XMFLOAT2(1.0f, 0.0f));
		Jump();
	}
	
	if (i->IsMouseKeyPushed(VK_RBUTTON))
	{
		bool exec = false;
		float cost = _powers.GetCurrentElement()->Activate(exec, _currentLight - _lightDownBy);
		if (exec)
		{
			_lightDownBy += cost;
		}
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

void Player::AddLight(float amount)
{
	float pmax = (_maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth;
	_maxLight = STARTLIGHT + MAXLIGHTINCREASE * (1.0f - amount);
	float delta = (_maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth - pmax;
	_builder->Animation()->PlayAnimation(_currentLightIndicator, "update", delta);
}

const void Player::AddWeapon(Weapons type)
		{
	auto& find = _weapons.find(type);
	
	if (find == _weapons.end())
			{
	switch (type)
	{
		case Weapons::Bounce:
			_weapons[type] = new BounceWeapon(_builder, _weaponEntity);
		break;
		case Weapons::FragBomb:
			_weapons[type] = new FragBombWeapon(_builder, _weaponEntity);
	
		break;
		case Weapons::RapidFire:
			_weapons[type] = new RapidFireWeapon(_builder, _weaponEntity);
	
		break;
		case Weapons::Shotgun:
			_weapons[type] = new ShotgunWeapon(_builder, _weaponEntity);
		break;
		case Weapons::Charge:
			_weapons[type] = new ChargeWeapon(_builder, _weaponEntity, _camera);
			break;
		case Weapons::LightThrower:
			_weapons[type] = new LightThrowerWeapon(_builder, _weaponEntity);
			break;
		case Weapons::Rocket:
			_weapons[type] = new RocketWeapon(_builder, _weaponEntity);
			break;
		case Weapons::Basic:
			_weapons[type] = new BasicWeapon(_builder, _weaponEntity);
			break;
	default:
		break;
	}

		if (!(_currentWep == type))
		{
			_weapons[_currentWep]->setActive(false);
			_currentWep = type;
			_weapons[_currentWep]->setActive(true);
	}
	}
	else
	{
	if (!(_currentWep == type))
	{
	_weapons[_currentWep]->setActive(false);
		_currentWep = type;
	_weapons[_currentWep]->setActive(true);
	}
		_weapons[_currentWep]->AddAmmo();
	}

	return void();
}

const void Player::AddPower(Power* power)
{
	Power* p = nullptr;
	p = dynamic_cast<LockOnStrike*>(power);
	if (p)
	{
		bool exists = false;
		for (int i = 0; i < _powers.Size(); ++i)
		{
			p = dynamic_cast<LockOnStrike*>(_powers.GetCurrentElement());
			if (p)
			{
				p->Upgrade();
				//delete power;
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::LOCK_ON_STRIKE);
		return;
	}
	p = dynamic_cast<RandomBlink*>(power);
	if (p)
	{
		for (int i = 0; i < _powers.Size(); ++i)
		{
			p = dynamic_cast<LockOnStrike*>(_powers.GetCurrentElement());
			if (p)
			{
				p->Upgrade();
				//delete power;
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::RANDOMBLINK);
	}
	p = dynamic_cast<CharmPower*>(power);
	if (p)
	{
		for (int i = 0; i < _powers.Size(); ++i)
		{
			p = dynamic_cast<CharmPower*>(_powers.GetCurrentElement());
			if (p)
			{
				p->Upgrade();
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::CHARMPOWER);
	}
	p = dynamic_cast<TimeStopper*>(power);
	if (p)
	{
		for (int i = 0; i < _powers.Size(); ++i)
		{
			p = dynamic_cast<TimeStopper*>(_powers.GetCurrentElement());
			if (p)
			{
				p->Upgrade();
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::TIMESTOPPER);
	}
}

const void Player::GetPowerInfo(std::vector<power_id_t>& powerinfo)
{
	for (int i = 0; i < _powers.Size(); ++i)
	{
		for (int j = -1; j < _powers.GetCurrentElement()->GetPowerLevel(); ++j)
		{
			powerinfo.push_back(_powers.GetCurrentElement()->GetType());
		}
		_powers.MoveCurrent();
	}
	return void();
}

const void Player::ClearAllPowers()
{
	while (_powers.Size())
	{
		_powers.RemoveCurrentElement();
	}
	return void();
}

const void Player::_ChangePower()
{
	if (_powers.Size())
	{
		_powers.MoveCurrent();
	}
}

const void Player::ShotFired()
{
	_shotsFired++;
}

const void Player::EnemyDefeated()
{
	_enemiesDefeated++;
	_totalLightCollected += 20;
}

const void Player::ShotConnected()
{
	_shotsHit++;
}

int Player::GetShotsFired()
{
	return _shotsFired;
}

int Player::GetShotsConnected()
{
	return _shotsHit;
}

float Player::GetHitPercent()
{
	return 100.0f*(_shotsHit / (_shotsFired*1.0f));
}

int Player::GetEnemiesDefeated()
{
	return _enemiesDefeated;
}

int Player::GetTotalLightCollected()
{
	return _totalLightCollected;
}