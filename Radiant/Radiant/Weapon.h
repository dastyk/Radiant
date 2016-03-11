#ifndef _WEAPON_H_
#define _WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Projectile.h"
#include "EntityBuilder.h"
#include "Dungeon.h"
#include "Flags.h"

//
//class Weapons
//{
//public:
//	static const unsigned int Basic = 1 << 0;
//	static const unsigned int Bounce = 1 << 1;
//	static const unsigned int Charge = 1 << 2;
//	static const unsigned int FragBomb = 1 << 3;
//	static const unsigned int LightThrower = 1 << 4;
//	static const unsigned int RapidFire = 1 << 5;
//	static const unsigned int Rocket = 1 << 6;
//	static const unsigned int Shotgun = 1 << 7;
//
//
//
//
//	
//
//	static const unsigned int Num_Weapons = 8;
//	
//	Weapons() : _flags(0) {}
//	Weapons(unsigned int singleFlag) : _flags(singleFlag) {}
//	Weapons(const Weapons& original) : _flags(original._flags) {}
//
//
//
//	bool operator==(const Weapons& rhs) const { return _flags == rhs._flags; }
//	Weapons    operator =(unsigned int addValue) { this->_flags = addValue; return *this;}
//	Weapons&   operator |=(unsigned int addValue) { _flags |= addValue; return *this; }
//	Weapons    operator |(unsigned int addValue) { Weapons  result(*this); result |= addValue; return result; }
//	Weapons&   operator &=(unsigned int maskValue) { _flags &= maskValue; return *this; }
//	Weapons    operator &(unsigned int maskValue) { Weapons  result(*this); result &= maskValue; return result; }
//	Weapons    operator ~() { Weapons  result(*this); result._flags = ~result._flags; return result; }
//	unsigned int Count() { unsigned int count = 0; for (unsigned int i = 0; i < sizeof(unsigned int); i++) { count += _flags << i; } return count; }
//	operator unsigned int() { return _flags; }
//
//	unsigned int _flags;
//
//	std::size_t operator()(const Weapons& key) const
//	{
//		return std::hash<unsigned int>()(key._flags);
//	}
//};
//
//
//struct WeaponsHasher
//{
//	std::size_t operator()(const Weapons& key) const
//	{
//		return std::hash<unsigned>()(key._flags);
//	}
//};

CCR(Weapons, unsigned int, Num_Weapons = 8,
	Basic = 1 << 0,
	Bounce = 1 << 1,
	Charge = 1 << 2,
	FragBomb = 1 << 3,
	LightThrower = 1 << 4,
	RapidFire = 1 << 5,
	Rocket = 1 << 6,
	Shotgun = 1 << 7);
//stLightThrower = 1 << 4;atic const unsigned int Num_Weapons = 7;
//enRapidFire = 1 << 5;um class Weapons : unsigned
//{	Rocket = 1 << 6;
//	Shotgun = 1 << 7;Basic = 1 << 0,
//	Shotgun = 1 << 1,
//	RapidFire = 1 << 2,
//	FragBomb = 1 << 3,
//	Bounce = 1 << 4,
//	Charge = 1 << 5,
//	LightThrower = 1 << 6
//};
//
//unsigned int operator |(const Weapons & f, const Weapons& s)
//{
//	unsigned int out = static_cast<unsigned int>(f) | static_cast<unsigned int>(s);
//	return out;
//}
//unsigned int operator |(const unsigned int & f, const Weapons& s)
//{
//	unsigned int out = f | static_cast<unsigned int>(s);
//	return out;
//}
class Weapon
{	
protected:
	Weapon(EntityBuilder* builder, unsigned int type);
public:




	virtual void Update(const Entity& playerEntity, float deltaTime) =0;
	virtual ~Weapon();

	virtual const vector<Projectile*>& GetProjectiles();

	virtual void setActive(bool value);

	virtual bool Shoot(const Entity& playerEntity) = 0;

	virtual void AddAmmo();
	virtual bool HasAmmo() { return _currentAmmo ? true : false; }
	virtual unsigned int Type() { return _type; };
protected:
	float _cooldown;
	float _timeSinceLastActivation;
	unsigned int _type;
	bool _active;
	XMFLOAT3 _moveVector;
	unsigned int _maxAmmo;
	unsigned int _currentAmmo;
	vector<Projectile*> _projectiles;
	float _currentSize;
	Entity _weaponEntity;
	EntityBuilder* _builder;


	float _uneS;
	float _eS;
};

#endif