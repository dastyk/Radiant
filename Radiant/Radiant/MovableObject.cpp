#include "MovableObject.h"

MovableObject::MovableObject()
{
	//empty
}

MovableObject::~MovableObject()
{

}

void MovableObject::Update()
{
	
}

void MovableObject::TranslateAlongVector(const DirectX::XMFLOAT3 & direction, float amount)
{
	DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&direction);
	_position = DirectX::XMVectorAdd(_position, DirectX::XMVectorScale(v, amount));
}
