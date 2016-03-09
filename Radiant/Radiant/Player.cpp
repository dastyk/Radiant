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

	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);
	_builder->Transform()->BindChild(_camera, _weaponEntity);
	_builder->Transform()->SetPosition(_weaponEntity, XMFLOAT3(0.07f, -0.05f, 0.2f));

	_weapons.push_back(new LightThrowerWeapon(_builder, _weaponEntity));
	_currentWep = 0;






	Entity llvl = _builder->CreateLabel(
		XMFLOAT3(0.0f, System::GetOptions()->GetScreenResolutionHeight() - 50.0f, 0.0f),
		"Light Level ",
		TextColor,
		300.0f,
		50.0f,
		"");

	_lightBarBorder = _builder->CreateOverlay(
		XMFLOAT3(300.0f, System::GetOptions()->GetScreenResolutionHeight() - 50.0f, 0.0f),
		(_maxLight / 20.0f)*BAR_MAXSIZE,
		50.0f,
		"Assets/Textures/default_color.png");

	_lightBar = _builder->CreateOverlay(
		XMFLOAT3(300.0f, System::GetOptions()->GetScreenResolutionHeight() - 45.0f, 0.0f),
		(_currentLight / 20.0f)*BAR_MAXSIZE,
		40.0f,
		"Assets/Textures/default_normal.png");

	
	_builder->Animation()->CreateAnimation(_lightBarBorder, "scale", 0.5f, 
		[this](float delta, float amount, float offset)
	{
		_builder->Overlay()->SetExtents(_lightBarBorder, offset + amount, 50.0f);
	});

	//_controller->BindEventHandler(llvl, EventManager::Type::Overlay);
	//_controller->BindEvent(llvl, EventManager::EventType::Update,
	//	[llvl, this]()
	//{
	//	static float prev = _AI->GetLightPoolPercent() * 1000;
	//	static float curr = prev;

	//	curr = _AI->GetLightPoolPercent() * 1000;
	//	if (curr < prev)
	//	{
	//		float diff = prev - curr;
	//		prev -= _gameTimer.DeltaTime() * 2 * diff + 1;
	//		_controller->Text()->ChangeText(llvl, "Light Collected: " + to_string((uint)(1000 - prev)));
	//		//_controller->Camera()->SetDrawDistance(_player->GetEntity(), (1.0f - prev + 0.25) * 25);
	//		_controller->Camera()->SetViewDistance(_player->GetEntity(), (1.0f - prev / 1000.0f)*15.0f + 6.0f);
	//		_controller->Light()->ChangeLightRange(_player->GetEntity(), (1.0f - prev / 1000.0f)*15.0f + 1.0f);
	//	}
	//	else
	//	{
	//		prev = curr;
	//	}
	//});
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

		_builder->Overlay()->SetExtents(_lightBar, (_currentLight / 20.0f)*BAR_MAXSIZE, 40.0f);
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
	int sde = 0;
	//if (i->IsKeyPushed(VK_R))
	if (i->IsScrollUp(sde))
	{
		unsigned int bef = _currentWep;
		_currentWep -= (unsigned int)(_currentWep == (unsigned int)_weapons.size() - 1)? (unsigned int)_weapons.size() - 1 : -1;
		while (!_weapons[_currentWep]->HasAmmo())
			_currentWep -= (unsigned int)(_currentWep == (unsigned int)_weapons.size() - 1) ? (unsigned int)_weapons.size() - 1 : -1;
		if (!(bef == _currentWep))
		{
			_weapons[bef]->setActive(false);
			_weapons[_currentWep]->setActive(true);
		}

	}
	//if (i->IsKeyPushed(VK_E))
	if(i->IsScrollDown(sde))
	{
		unsigned int bef = _currentWep;
		_currentWep += (unsigned int)(_currentWep == 0) ? (unsigned int)_weapons.size() - 1 : -1;
		while (!_weapons[_currentWep]->HasAmmo())
			_currentWep += (unsigned int)(_currentWep == 0) ? (unsigned int)_weapons.size() - 1 : -1;
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
		const vector<Projectile*>& pr2 = w->GetProjectiles();
		pr.insert(pr.end(), pr2.begin(), pr2.end());
	}
	return pr;
}

void Player::AddLight(float amount)
{
	float pmax = (_maxLight / 20.0f)*BAR_MAXSIZE;
	_maxLight = STARTLIGHT + MAXLIGHTINCREASE * (1.0f - amount);
	float delta = (_maxLight / 20.0f)*BAR_MAXSIZE - pmax;
	_builder->Animation()->PlayAnimation(_lightBarBorder, "scale", delta, pmax);
	//_maxLight += amount;


}

const void Player::AddWeapon(unsigned int type)
{
	for (unsigned int i = 0; i < _weapons.size(); i++)
	{
		if (_weapons[i]->Type() == type)
		{
	
			if (_currentWep != i)
			{
				_weapons[_currentWep]->setActive(false);
				_currentWep = i;
				_weapons[_currentWep]->setActive(true);
			}
			_weapons[_currentWep]->AddAmmo();
			return;
		}
	}


	switch (type)
	{
	case 1:
		_weapons.push_back(new BounceWeapon(_builder, _weaponEntity));
		break;
	case 2:
		_weapons.push_back(new FragBombWeapon(_builder, _weaponEntity));
	
		break;
	case 3:
		_weapons.push_back(new RapidFireWeapon(_builder, _weaponEntity));
	
		break;
	case 4:
		_weapons.push_back(new ShotgunWeapon(_builder, _weaponEntity));
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
				delete power;
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
				delete power;
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::RANDOMBLINK);
	}
}

const void Player::_ChangePower()
{
	if (_powers.Size())
	{
		_powers.MoveCurrent();
	}
}
