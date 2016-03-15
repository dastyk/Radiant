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
	_reservedLight = 0.0f;
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

	_currentWep = Weapons::Basic;
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


	_lightReservedBar = _builder->CreateOverlay(
		XMFLOAT3((_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth + 10.0f*_screenPercentWidth - (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth, System::GetOptions()->GetScreenResolutionHeight() - 50.0f*_screenPercentHeight, 0.0f),
		(_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth,
		40.0f*_screenPercentHeight,
		"Assets/Textures/Light_Res_Bar.png");

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

	_builder->Animation()->CreateAnimation(_lightReservedBar, "update", 0.25f,
		[this](float delta, float amount, float offset)
	{
		_builder->Overlay()->SetExtents(_lightReservedBar, offset + amount, 40.0f*_screenPercentHeight);
		_builder->Transform()->MoveLeft(_lightReservedBar, delta);
	});
	_builder->Animation()->CreateAnimation(_lightReservedBar, "ext", 0.5f,
		[this](float delta, float amount, float offset)
	{
		_builder->Overlay()->SetExtents(_lightReservedBar, offset + amount, 40.0f*_screenPercentHeight);
	});	
	_builder->Animation()->CreateAnimation(_lightReservedBar, "move", 0.5f,
		[this](float delta, float amount, float offset)
	{
		_builder->Transform()->MoveRight(_lightReservedBar, delta);
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

	_powerDecal = _builder->EntityC().Create(); //Dummy just so the wrong thing doesn't get deleted







	_dmgOD = _builder->CreateOverlay(
		XMFLOAT3(0.0f, System::GetOptions()->GetScreenResolutionHeight()-60.0f, 0.0f),
		System::GetOptions()->GetScreenResolutionWidth(),
		60.0f,
		"Assets/Textures/dmgtexd.png");
	_dmgOU = _builder->CreateOverlay(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		System::GetOptions()->GetScreenResolutionWidth(),
		60.0f,
		"Assets/Textures/dmgtexu.png");
	_dmgOL = _builder->CreateOverlay(
		XMFLOAT3(-60.0f, 0.0f, 0.0f),
		60.0f,
		System::GetOptions()->GetScreenResolutionHeight(),
		"Assets/Textures/dmgtexl.png");
	_dmgOR = _builder->CreateOverlay(
		XMFLOAT3(System::GetOptions()->GetScreenResolutionWidth(), 0.0f, 0.0f),
		60.0f,
		System::GetOptions()->GetScreenResolutionHeight(),
		"Assets/Textures/dmgtexr.png");
	_builder->Overlay()->ToggleVisible(_dmgOD, false);
	_builder->Overlay()->ToggleVisible(_dmgOU, false);
	_builder->Overlay()->ToggleVisible(_dmgOL, false);
	_builder->Overlay()->ToggleVisible(_dmgOR, false);



	_builder->Animation()->CreateAnimation(_dmgOD, "flash", 0.1f,
		[this](float delta, float amount, float offset)
	{
		_builder->Overlay()->ToggleVisible(_dmgOD, true);
		_builder->Transform()->MoveUp(_dmgOD, delta);
	},
		[this]()
	{
		_builder->Animation()->PlayAnimation(_dmgOD, "fade", 60.0f);
	});
	_builder->Animation()->CreateAnimation(_dmgOD, "fade", 0.1f,
		[this](float delta, float amount, float offset)
	{
		_builder->Transform()->MoveDown(_dmgOD, delta);

	},
		[this]()
	{
		_builder->Overlay()->ToggleVisible(_dmgOD, false);
	});




	_builder->Animation()->CreateAnimation(_dmgOU, "flash", 0.1f,
		[this](float delta, float amount, float offset)
	{
		_builder->Overlay()->ToggleVisible(_dmgOU, true);
		_builder->Transform()->MoveDown(_dmgOU, delta);
	},
		[this]()
	{
		_builder->Animation()->PlayAnimation(_dmgOU, "fade", 60.0f);
	});
	_builder->Animation()->CreateAnimation(_dmgOU, "fade", 0.1f,
		[this](float delta, float amount, float offset)
	{
		_builder->Transform()->MoveUp(_dmgOU, delta);

	},
		[this]()
	{
		_builder->Overlay()->ToggleVisible(_dmgOU, false);
	});





	_builder->Animation()->CreateAnimation(_dmgOL, "flash", 0.1f,
		[this](float delta, float amount, float offset)
	{
		_builder->Overlay()->ToggleVisible(_dmgOL, true);
		_builder->Transform()->MoveRight(_dmgOL, delta);
	},
		[this]()
	{
		_builder->Animation()->PlayAnimation(_dmgOL, "fade", 60.0f);
	});
	_builder->Animation()->CreateAnimation(_dmgOL, "fade", 0.1f,
		[this](float delta, float amount, float offset)
	{
		_builder->Transform()->MoveLeft(_dmgOL, delta);

	},
		[this]()
	{
		_builder->Overlay()->ToggleVisible(_dmgOL, false);
	});

	_builder->Animation()->CreateAnimation(_dmgOR, "flash", 0.1f,
		[this](float delta, float amount, float offset)
	{
		_builder->Overlay()->ToggleVisible(_dmgOR, true);
		_builder->Transform()->MoveLeft(_dmgOR, delta);
	},
		[this]()
	{
		_builder->Animation()->PlayAnimation(_dmgOR, "fade", 60.0f);
	});
	_builder->Animation()->CreateAnimation(_dmgOR, "fade", 0.1f,
		[this](float delta, float amount, float offset)
	{
		_builder->Transform()->MoveRight(_dmgOR, delta);

	},
		[this]()
	{
		_builder->Overlay()->ToggleVisible(_dmgOR, false);
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
	_reservedLight = 0.0f;
	_lightDownBy = 0.0f;

	float textWidth = _builder->Text()->GetLength(_llvl);

	_builder->Transform()->SetPosition(_lightBar, XMFLOAT3(10.0f*_screenPercentWidth, System::GetOptions()->GetScreenResolutionHeight() - 50.0f*_screenPercentHeight, 0.0f));
	_builder->Transform()->SetPosition(_currentLightIndicator, XMFLOAT3((_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth + 10.0f*_screenPercentWidth, System::GetOptions()->GetScreenResolutionHeight() - 50.0f*_screenPercentHeight, 0.0f));
	_builder->Overlay()->SetExtents(_lightBar, (_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth, 40.0f*_screenPercentHeight);
	
	_builder->Transform()->SetPosition(_lightReservedBar, XMFLOAT3((_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth + 10.0f*_screenPercentWidth - (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth, System::GetOptions()->GetScreenResolutionHeight() - 50.0f*_screenPercentHeight, 0.0f));
	_builder->Overlay()->SetExtents(_lightReservedBar, (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth,
		40.0f*_screenPercentHeight);

	_builder->Camera()->SetViewDistance(_camera, (_currentLight / 20.0f)*15.0f + 3.0f);
	_builder->Light()->ChangeLightRange(_camera, (_currentLight / 20.0f)*15.0f + 1.0f);
	
	_setPowerDecal();
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
	bool change = false;
	if (_lightDownBy > 0.0f)
	{
		_currentLight -= (_lightDownBy + 10.0f) * deltatime ;
		_lightDownBy -= (_lightDownBy + 10.0f) * deltatime;
		if (_lightDownBy < 0.0f)
		{
			_currentLight -= _lightDownBy;
			_lightDownBy = 0.0f;
		}
		change = true;
	}
	if (_currentLight < _maxLight*(1.0f-_reservedLight))
	{
		
		_currentLight += _lightRegenerationRate * deltatime;

		if (_currentLight > _maxLight*(1.0f - _reservedLight))
			_currentLight = _maxLight*(1.0f - _reservedLight);
		change = true;
		

	}
	if(change)
	{
		_builder->Overlay()->SetExtents(_lightBar, (_currentLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth, 40.0f*_screenPercentHeight);
		_builder->Camera()->SetViewDistance(_camera, (_currentLight / 20.0f)*15.0f + 3.0f);
		_builder->Light()->ChangeLightRange(_camera, (_currentLight / 20.0f)*15.0f + 1.0f);
	}

	if (_powers.Size())
	{
		XMFLOAT3 playPos;
		XMFLOAT3 playYaw;
		XMStoreFloat3(&playPos, _builder->Transform()->GetPositionW(_camera));
		playPos.y = 0.0f;
		XMStoreFloat3(&playYaw, _builder->Transform()->GetRotation(_camera));
		_builder->Transform()->SetPosition(_powerDecal, XMLoadFloat3(&playPos));
		_builder->Transform()->RotateYaw(_powerDecal, deltatime * 36.0f);
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

	if (i->IsKeyPushed(VK_T))
	{
		float offset = (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth;
		_reservedLight = 0.5f;
		float delta = (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth - offset;
		_lightDownBy += _maxLight*0.5f;
		_builder->Animation()->PlayAnimation(_lightReservedBar, "update", delta, offset);
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
		_currentWep = Weapons::Basic;
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
		if (_powers.Size())
		{
			bool exec = false;
			float cost = _powers.GetCurrentElement()->Activate(exec, _currentLight - _lightDownBy);
			if (exec)
			{
				_lightDownBy += cost;
			}
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

void Player::RegenerateLight(float percent)
{
	float offset = (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth;
	_reservedLight = percent;
	float delta = (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth - offset;
	_lightDownBy += min(_maxLight*percent, _currentLight);
	_builder->Animation()->PlayAnimation(_lightReservedBar, "update", delta, offset);
	_lightRegenerationRate *= 4.0f;
}

void Player::ResetRegen()
{
	float offset = (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth;
	_reservedLight = 0.0f;
	float delta = (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth - offset;
	_builder->Animation()->PlayAnimation(_lightReservedBar, "update", delta, offset);
	_lightRegenerationRate /= 4.0f;
}

float Player::GetHealth()
{
	return _health;
}

float Player::GetHealthPercent()
{
	return _health / _maxHealth;
}

float Player::GetCurrentLight() const
{
	return _currentLight;
}

void Player::SetHealth(float value)
{
	_health = value;
}

void Player::RemoveHealth(float amount, const DirectX::XMVECTOR& dir)
{
	_health -= amount;





	DirectX::XMVECTOR playerDir = DirectX::XMVector3Normalize(_builder->Transform()->GetDirection(_camera));
	DirectX::XMVECTOR playerRight = DirectX::XMVector3Normalize(_builder->Transform()->GetRight(_camera));
	playerDir = XMVectorSetY(playerDir, 0.0f);
	playerRight = XMVectorSetY(playerRight, 0.0f);
	DirectX::XMVECTOR projDir = XMVectorSetY(dir, 0.0f);
	float angle1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(playerDir, DirectX::XMVector3Normalize (-projDir)));
	float angle2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(playerRight, DirectX::XMVector3Normalize (-projDir)));

	if (angle1 <= -0.707f)
		_builder->Animation()->PlayAnimation(_dmgOD, "flash", 60.0f);
	if (angle1 >= 0.707f)
		_builder->Animation()->PlayAnimation(_dmgOU, "flash", 60.0f);
	if (angle2 >= 0.707f)
		_builder->Animation()->PlayAnimation(_dmgOR, "flash", 60.0f);
	if (angle2 <= -0.707f)
		_builder->Animation()->PlayAnimation(_dmgOL, "flash", 60.0f);



}

void Player::AddHealth(float amount)
{
	_health += amount;
}

void Player::RemoveLight(float amount)
{
	_currentLight -= amount;
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
	float offset = (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth;
	_maxLight = STARTLIGHT + MAXLIGHTINCREASE * (1.0f - amount);
	float delta = (_maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth - pmax;
	_builder->Animation()->PlayAnimation(_currentLightIndicator, "update", delta);
	_builder->Animation()->PlayAnimation(_lightReservedBar, "move", delta);
	
	
	delta = (_reservedLight * _maxLight / 20.0f)*BAR_MAXSIZE*_screenPercentWidth - offset;
	_builder->Animation()->PlayAnimation(_lightReservedBar, "update", delta, offset);
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
				_setPowerDecal();
				//delete power;
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::LOCK_ON_STRIKE);
		_setPowerDecal();
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
				_setPowerDecal();
				//delete power;
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::RANDOMBLINK);
		_setPowerDecal();
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
				_setPowerDecal();
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::CHARMPOWER);
		_setPowerDecal();
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
				_setPowerDecal();
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::TIMESTOPPER);
		_setPowerDecal();
	}
	p = dynamic_cast<RegenPower*>(power);
	if (p)
	{
		for (int i = 0; i < _powers.Size(); ++i)
		{
			p = dynamic_cast<RegenPower*>(_powers.GetCurrentElement());
			if (p)
			{
				p->Upgrade();
				return;
			}
			_powers.MoveCurrent();
		}
		_powers.AddElementToList(power, power_id_t::REGENPOWER);
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
		_setPowerDecal();
	}
}
//blabla
void Player::_setPowerDecal()
{
	if (_powers.Size())
	{
		Power* curPow = _powers.GetCurrentElement();
		if (curPow)
		{
			_builder->Decal()->ReleaseDecal(_powerDecal);
			std::string texName = _powers.GetCurrentElement()->GetTextureName();
			XMFLOAT3 posf;
			XMFLOAT3 scalef = XMFLOAT3(1.25f, 1.25f, 0.25f);
			XMFLOAT3 rotf = XMFLOAT3(90.0f, 0.0f, 0.0f);
			XMStoreFloat3(&posf, _builder->Transform()->GetPositionW(_camera));
			posf.y = 0.05f;
			_powerDecal = _builder->CreateDecal(posf, rotf, scalef, texName, "Assets/Textures/default_normal.png", texName);
			_builder->Material()->SetMaterialProperty(_powerDecal, "EmissiveIntensity", 0.5f, "Shaders/DecalsPS.hlsl");
		}
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