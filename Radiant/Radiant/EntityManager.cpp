#include "EntityManager.h"

Entity EntityManager::Create( void )
{
	unsigned index;

	if ( _freeIndices.size() > MINIMUM_FREE_INDICES )
	{
		index = _freeIndices.front();
		_freeIndices.pop_front();
	}
	else
	{
		_generation.push_back( 0 );
		index = static_cast<unsigned int>(_generation.size() - 1);

		if ( index >= (1 << ENTITY_INDEX_BITS) )
			throw("Too large entity index!");
	}

	return{ index | (_generation[index] << ENTITY_INDEX_BITS) };
}

bool EntityManager::Alive( const Entity& e ) const
{
	return _generation[e.Index()] == e.Generation();
}

void EntityManager::Destroy( const Entity& e )
{
	const unsigned index = e.Index();
	++_generation[index];
	_freeIndices.push_back( index );
}