#ifndef _MOVABLE_OBJECT_
#define _MOVABLE_OBJECT_
#pragma once

#include <DirectXMath.h>
#include "StaticObject.h"

class MovableObject : public StaticObject
{
public:
	MovableObject();
	virtual ~MovableObject();
	virtual void Update();

	void TranslateAlongVector(const DirectX::XMFLOAT3& direction, float amount);

protected:
	

	
	
};

#endif

