#include "TransformManager.h"

#include <memory>

using namespace DirectX;

TransformManager::TransformManager()
{
	_data.Length = 0;
	_data.Capacity = 0;
	_data.Buffer = nullptr;
	_data.Entity = nullptr;
	_data.Local = nullptr;
	_data.World = nullptr;
	_data.Parent = nullptr;
	_data.FirstChild = nullptr;
	_data.PrevSibling = nullptr;
	_data.NextSibling = nullptr;
	_Allocate( 5 );
}

TransformManager::~TransformManager()
{
	operator delete(_data.Buffer);
}

void TransformManager::CreateTransform( Entity entity )
{
	if ( _data.Length == _data.Capacity )
		_Allocate( _data.Capacity * 2 );

	unsigned index = _data.Length++;

	_data.Entity[index] = entity;
	XMStoreFloat4x4( &_data.Local[index], XMMatrixIdentity() );
	XMStoreFloat4x4( &_data.World[index], XMMatrixIdentity() );
	_data.Parent[index].i = -1;
	_data.FirstChild[index].i = -1;
	_data.PrevSibling[index].i = -1;
	_data.NextSibling[index].i = -1;

	_entityToIndex[entity] = index;
}

void TransformManager::BindChild( Entity parent, Entity child )
{
	// TODO: set child as child of parent. Do this by setting parent, firstchild, prevsibling, nextsibling
	// of any transforms involved to appropriate values. If the child already has a parent we can
	// either exit binding or rebind it. If rebinding, we must make sure any affected transforms are
	// updated as well.

	// Both parent and child exists
	if ( _entityToIndex.find( parent ) != _entityToIndex.end() && _entityToIndex.find( child ) != _entityToIndex.end() )
	{
		unsigned parentIndex = _entityToIndex[parent];
		unsigned childIndex = _entityToIndex[child];

		// Child doesn't already have a parent
		if ( _data.Parent[childIndex].i == -1 )
		{
			// Set parent of child
			_data.Parent[childIndex].i = parentIndex;

			Instance childOfParent = _data.FirstChild[parentIndex];

			// Parent doesn't have any children: add directly
			if ( childOfParent.i == -1 )
			{
				_data.FirstChild[parentIndex].i = childIndex;
			}
			// Parent has child(ren): find last valid child
			else
			{
				// As long as we have another sibling, traverse to it (walk to the last).
				while ( _data.NextSibling[childOfParent.i].i != -1 )
					childOfParent = _data.NextSibling[childOfParent.i];

				// We are now at the last sibling. Have its next sibling be
				// the new child, and set the previous sibling of the new one.
				_data.NextSibling[childOfParent.i].i = childIndex;
				_data.PrevSibling[childIndex].i = childOfParent.i;
			}
		}
	}
}

// TODO: Recursive now, make iterative for performance. Don't forget to call callback if valid
void TransformManager::SetTransform( const Entity& entity, const XMMATRIX& transform )
{
	auto indexIt = _entityToIndex.find( entity );

	if ( indexIt != _entityToIndex.end() )
	{
		XMStoreFloat4x4( &_data.Local[indexIt->second], transform );
		Instance parent = _data.Parent[indexIt->second];

		// Get parent transform if valid parent instance (ie, it has a parent).
		// If no parent: use identity matrix
		XMMATRIX parentTransform = parent.i != -1 ? XMLoadFloat4x4( &_data.World[parent.i] ) : XMMatrixIdentity();

		_Transform( indexIt->second, parentTransform );
	}
}

void TransformManager::_Transform( unsigned instance, const XMMATRIX& parent )
{
	XMMATRIX world = XMMatrixMultiply( XMLoadFloat4x4( &_data.Local[instance] ), parent );
	XMStoreFloat4x4( &_data.World[instance], world );

	if ( _transformChangeCallback )
		_transformChangeCallback( _data.Entity[instance], world );
	//if ( mTransformChangeCallback2 )
	//	mTransformChangeCallback2( _data.Entity[instance], world );
	//if ( mTransformChangeCallback3 )
	//	mTransformChangeCallback3( _data.Entity[instance], world );
	//if ( mTransformChangeCallback4 )
	//	mTransformChangeCallback4( _data.Entity[instance], world );
	//if ( mTransformChangeCallback5 )
	//	mTransformChangeCallback5( _data.Entity[instance], world );

	Instance child = _data.FirstChild[instance];
	// while valid child
	while ( child.i != -1 )
	{
		_Transform( child.i, world );
		child = _data.NextSibling[child.i];
	}
}

XMMATRIX TransformManager::GetTransform( const Entity& entity ) const
{
	auto indexIt = _entityToIndex.find( entity );

	if ( indexIt != _entityToIndex.end() )
	{
		return XMLoadFloat4x4( &_data.Local[indexIt->second] );
	}

	return XMMatrixIdentity();
}

void TransformManager::_Allocate( unsigned numItems )
{
	if ( numItems <= _data.Capacity )
		throw("Allocation should only grow to accomodate more items, not fewer!");

	Data newData;
	const unsigned bytes = numItems * (sizeof( Entity ) + 2 * sizeof( XMFLOAT4X4 ) + 4 * sizeof( Instance ));
	newData.Buffer = operator new(bytes);
	newData.Length = _data.Length;
	newData.Capacity = numItems;

	newData.Entity = reinterpret_cast<Entity*>(newData.Buffer);
	newData.Local = reinterpret_cast<XMFLOAT4X4*>(newData.Entity + numItems);
	newData.World = newData.Local + numItems;
	newData.Parent = reinterpret_cast<Instance*>(newData.World + numItems);
	newData.FirstChild = newData.Parent + numItems;
	newData.PrevSibling = newData.FirstChild + numItems;
	newData.NextSibling = newData.PrevSibling + numItems;

	memcpy( newData.Entity, _data.Entity, _data.Length * sizeof( Entity ) );
	memcpy( newData.Local, _data.Local, _data.Length * sizeof( XMFLOAT4X4 ) );
	memcpy( newData.World, _data.World, _data.Length * sizeof( XMFLOAT4X4 ) );
	memcpy( newData.Parent, _data.Parent, _data.Length * sizeof( Instance ) );
	memcpy( newData.FirstChild, _data.FirstChild, _data.Length * sizeof( Instance ) );
	memcpy( newData.PrevSibling, _data.PrevSibling, _data.Length * sizeof( Instance ) );
	memcpy( newData.NextSibling, _data.NextSibling, _data.Length * sizeof( Instance ) );

	operator delete(_data.Buffer);

	_data = newData;
}