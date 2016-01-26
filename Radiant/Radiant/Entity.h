#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <functional>

const unsigned ENTITY_INDEX_BITS = 24;
const unsigned ENTITY_INDEX_MASK = (1 << ENTITY_INDEX_BITS) - 1;

const unsigned ENTITY_GENERATION_BITS = 8;
const unsigned ENTITY_GENERATION_MASK = (1 << ENTITY_GENERATION_BITS) - 1;

struct Entity
{
	unsigned ID;

	unsigned Index() const { return ID & ENTITY_INDEX_MASK; }
	unsigned Generation() const { return (ID >> ENTITY_INDEX_BITS) & ENTITY_GENERATION_MASK; }

	bool operator==( const Entity& rhs ) const
	{
		return ID == rhs.ID;
	}
};

struct EntityHasher
{
	std::size_t operator()( const Entity& key ) const
	{
		return std::hash<unsigned>()(key.ID);
	}
};

#endif // _ENTITY_H_