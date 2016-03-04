#ifndef _STATIC_OBJECT_
#define _STATIC_OBJECT_
#pragma once

#include <DirectXMath.h>
#include "Entity.h"
#include "EntityManager.h"
#include "StaticMeshManager.h"

class StaticObject
{
public:
	StaticObject();
	StaticObject(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotAxis, float radians);
	virtual ~StaticObject();

	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetRotation(const DirectX::XMFLOAT3& axis, float radians);

	DirectX::XMFLOAT4X4 GetTranslationMatrix();
	DirectX::XMFLOAT4X4 GetScalingMatrix();
	DirectX::XMFLOAT4X4 GetRotationMatrix();
	DirectX::XMFLOAT4X4 GetWorldMatrix();


protected:
	DirectX::XMVECTOR _position;
	DirectX::XMVECTOR _scale;
	DirectX::XMVECTOR _rotate;//x,y,z is axis, w is angle in radians
};

#endif

