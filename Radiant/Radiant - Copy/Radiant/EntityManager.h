#ifndef _ENTITY_MANAGER_H_
#define _ENTITY_MANAGER_H_

#include <vector>
#include <deque>
#include "Entity.h"

class EntityManager
{
public:
	Entity Create( void );
	bool Alive( const Entity& e ) const;
	void Destroy( const Entity& e );

private:
	std::vector<unsigned char> _generation;
	std::deque<unsigned> _freeIndices;

	const unsigned MINIMUM_FREE_INDICES = 1024;
};

#endif // _ENTITY_MANAGER_H_