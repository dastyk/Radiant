#include "Player.h"
#include "System.h"

Player::Player(EntityBuilder* builder) : _builder(builder)
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

	_pulseTimer = 0.0f;

	_camera = _builder->CreateCamera(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	_builder->Light()->BindPointLight(_camera, XMFLOAT3(0.0f, 0.0f, 0.0f), 7.5f, XMFLOAT3(0.3f, 0.5f, 0.8f), 1.0f);
	

	// Create dummy model for collision -- Needs to be changed if starting camera position changes -- Also should probably look at near plane position and adjust to avoid clipping

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT2> uv;
	std::vector<unsigned int> indices;

	positions.resize(16);
	uv.resize(16);
	indices.resize(24);

	float extent = 0.1;

	for (int i = 0; i < 4; i++) // front face
	{
		if (i == 0 || i == 2)
		{
			positions[i].x = 0 - extent;
			positions[i].y = i * extent - extent;
			positions[i].z = 0 - extent;
		}
		else if (i == 1 || i == 3)
		{
			positions[i].x = 0 - extent;
			positions[i].y = (i - 1) * extent - extent;
			positions[i].z = 0 + extent;
		}
}

	uv[0].x = 1;
	uv[0].y = 1;

	uv[1].x = 0;
	uv[1].y = 1;

	uv[2].x = 1;
	uv[2].y = 0;

	uv[3].x = 0;
	uv[3].y = 0;


	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;

	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;


	for (int i = 0; i < 4; i++) // back face
	{
		if (i == 0 || i == 2)
		{
			positions[i + 4].x = 0 + extent;
			positions[i + 4].y = i * extent - extent;
			positions[i + 4].z = 0 - extent;
		}
		else if (i == 1 || i == 3)
		{
			positions[i + 4].x = 0 + extent;
			positions[i + 4].y = (i - 1) * extent - extent;
			positions[i + 4].z = 0 + extent;
		}
	}


	uv[4].x = 0;
	uv[4].y = 1;

	uv[5].x = 1;
	uv[5].y = 1;

	uv[6].x = 0;
	uv[6].y = 0;

	uv[7].x = 1;
	uv[7].y = 0;


	indices[6] = 1 + 4;
	indices[7] = 2 + 4;
	indices[8] = 0 + 4;

	indices[9] = 3 + 4;
	indices[10] = 2 + 4;
	indices[11] = 1 + 4;


	for (int i = 0; i < 4; i++) // left face
	{
		if (i == 0 || i == 2)
		{
			positions[i + 8].x = 0 + extent;
			positions[i + 8].y = i * extent - extent;
			positions[i + 8].z = 0 - extent;
		}
		else if (i == 1 || i == 3)
		{
			positions[i + 8].x = 0 - extent;
			positions[i + 8].y = (i - 1) * extent - extent;
			positions[i + 8].z = 0 - extent;
		}
	}


	uv[8].x = 1;
	uv[8].y = 1;

	uv[9].x = 0;
	uv[9].y = 1;

	uv[10].x = 1;
	uv[10].y = 0;

	uv[11].x = 0;
	uv[11].y = 0;


	indices[12] = 0 + 8;
	indices[13] = 2 + 8;
	indices[14] = 1 + 8;

	indices[15] = 1 + 8;
	indices[16] = 2 + 8;
	indices[17] = 3 + 8;


	for (int i = 0; i < 4; i++) // right face
	{
		if (i == 0 || i == 2)
		{
			positions[i + 12].x = 0 - extent;
			positions[i + 12].y = i * extent - extent;
			positions[ i + 12].z = 0 + extent;
		}
		else if (i == 1 || i == 3)
		{
			positions[i + 12].x = 0 + extent;
			positions[i + 12].y = (i - 1) * extent - extent;
			positions[i + 12].z = 0 + extent;
		}
	}


	uv[12].x = 1;
	uv[12].y = 1;

	uv[13].x = 0;
	uv[13].y = 1;

	uv[14].x = 1;
	uv[14].y = 0;

	uv[15].x = 0;
	uv[15].y = 0;


	indices[18] = 0 + 12;
	indices[19] = 2 + 12;
	indices[20] = 1 + 12;

	indices[21] = 1 + 12;
	indices[22] = 2 + 12;
	indices[23] = 3 + 12;


	_builder->Mesh()->CreateStaticMesh(_camera, "Player", positions, uv, indices);
	_builder->Material()->BindMaterial(_camera, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_camera, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_camera, 0, "Metallic", 0.1f, "Shaders/GBuffer.hlsl");
	_builder->Bounding()->CreateBoundingBox(_camera, _builder->Mesh()->GetMesh(_camera));
	_builder->GetEntityController()->Transform()->MoveForward(_camera, -1);

}

Player::~Player()
{

}

void Player::Update(float deltatime)
{

	//Swaying up and down when not jumping or dashing <---Need to be rewritten. Sorry, Jimbo!
	if (!_activeDash && !_activeJump)
	{
	//	_SetHeight(deltatime); 
	}

	_activeJump && _DoJump(deltatime);
	_activeDash && _DoDash(deltatime);

	_pulseTimer += deltatime * XM_PI * 0.25f;
	if (_pulseTimer > XM_2PI)
		_pulseTimer -= XM_2PI;
	_pulse = abs(sin(_pulseTimer));

	_builder->Light()->ChangeLightIntensity(_camera,0.4f + _pulse/2.0f);
	_builder->Light()->ChangeLightRange(_camera, 3.0f + _pulse * 10.0f);

}

void Player::HandleInput(float deltatime)
{
	int x, y;
	System::GetInput()->GetMouseDiff(x, y);
	if (x != 0)
		_builder->GetEntityController()->Transform()->RotateYaw(_camera, x * 0.1f);
	if (y != 0)
		_builder->GetEntityController()->Transform()->RotatePitch(_camera, y * 0.1f);
	if (System::GetInput()->IsKeyDown(VK_W))
		_builder->GetEntityController()->Transform()->MoveForward(_camera, 10.0f * deltatime);
	if (System::GetInput()->IsKeyDown(VK_S))
		_builder->GetEntityController()->Transform()->MoveBackward(_camera, 10.0f * deltatime);
	if (System::GetInput()->IsKeyDown(VK_A))
		_builder->GetEntityController()->Transform()->MoveLeft(_camera, 10.0f * deltatime);
	if (System::GetInput()->IsKeyDown(VK_D))
		_builder->GetEntityController()->Transform()->MoveRight(_camera, 10.0f * deltatime);
	if (System::GetInput()->IsKeyDown(VK_SHIFT))
		_builder->GetEntityController()->Transform()->MoveUp(_camera, 10.0f * deltatime);
	if (System::GetInput()->IsKeyDown(VK_CONTROL))
		_builder->GetEntityController()->Transform()->MoveDown(_camera, 10.0f * deltatime);
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

	float offset = DirectX::XMScalarSin((timeSoFar / _jumpTime) * DirectX::XM_PI);
	//DirectX::XMVectorSetY(_position, _yAtStartOfJump + offset);

	_builder->GetEntityController()->Transform()->MoveUp(_camera, offset);

	if(timeSoFar < _jumpTime)
		return true;

	timeSoFar = 0.0f;
	XMVECTOR pos = _builder->GetEntityController()->Transform()->GetPosition(_camera);
	DirectX::XMVectorSetY(pos, _yAtStartOfJump);
	_builder->GetEntityController()->Transform()->SetPosition(_camera, pos);
	return _activeJump = false;
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

Entity Player::GetEntity()
{
	return _camera;
}
