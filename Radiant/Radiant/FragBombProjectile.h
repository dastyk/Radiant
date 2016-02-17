#ifndef _FRAG_BOMB_PROJECTILE_H_
#define _FRAG_BOMB_PROJECTILE_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "General.h"
#include "EntityBuilder.h"
#include "Projectile.h"

class FragBombProjectile : public Projectile
{
private:
	bool child;

public:
	FragBombProjectile(Entity playerEntity, EntityBuilder* builder);
	FragBombProjectile(DirectX::XMFLOAT3 origin, EntityBuilder* builder);
	virtual ~FragBombProjectile();

	void Update(float deltaTime);
	bool GetState();
	bool IsChild();

};

#endif