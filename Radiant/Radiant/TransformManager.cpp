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
	_data.position = nullptr;
	_data.rotation = nullptr;
	_data.lookDir = nullptr;
	_data.up = nullptr;
	_data.right = nullptr;
	_Allocate( 5 );
}

TransformManager::~TransformManager()
{
	operator delete(_data.Buffer);
}

void TransformManager::CreateTransform(const Entity& entity )
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		return;
	}
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

	XMStoreFloat3(&_data.position[index], XMVectorSet(0,0,0,1));
	XMStoreFloat3(&_data.rotation[index], XMVectorSet(0, 0, 0, 0));	
	XMStoreFloat3(&_data.up[index], XMVectorSet(0, 1, 0, 0));
	XMStoreFloat3(&_data.right[index], XMVectorSet(1, 0, 0, 0));
	XMStoreFloat3(&_data.lookDir[index], XMVectorSet(0, 0, 1, 0));

	_entityToIndex[entity] = index;
}

void TransformManager::BindChild(const Entity& parent, const Entity& child )
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



void TransformManager::_Transform(const unsigned instance, const XMMATRIX& parent )
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




const void TransformManager::MoveForward(const Entity& entity, const float amount)
{
	
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		XMVECTOR pos = XMLoadFloat3(&_data.position[indexIt->second]);
		XMVECTOR dir = XMLoadFloat3(&_data.lookDir[indexIt->second]);
		pos = XMVectorAdd(pos, XMVectorScale(dir, amount));
		XMStoreFloat3(&_data.position[indexIt->second], pos );
		XMVECTOR up = XMLoadFloat3(&_data.up[indexIt->second]);
		if (_transformChangeCallback2)
			_transformChangeCallback2(entity, pos, dir, up);
	}
}

const void TransformManager::MoveBackward(const Entity& entity, const float amount)
{
	MoveForward(entity, -amount);
}

const void TransformManager::MoveRight(const Entity& entity, const float amount)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		XMVECTOR pos = XMLoadFloat3(&_data.position[indexIt->second]);
		XMVECTOR right = XMLoadFloat3(&_data.right[indexIt->second]);
		pos = XMVectorAdd(pos, XMVectorScale(right, amount));
		XMStoreFloat3(&_data.position[indexIt->second], pos);

		XMVECTOR up = XMLoadFloat3(&_data.up[indexIt->second]);
		XMVECTOR dir = XMLoadFloat3(&_data.lookDir[indexIt->second]);

		if (_transformChangeCallback2)
			_transformChangeCallback2(entity, pos, dir, up);
	}
}

const void TransformManager::MoveLeft(const Entity& entity, const float amount)
{
	MoveRight(entity, -amount);
}

const void TransformManager::MoveUp(const Entity& entity, const float amount)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		XMVECTOR pos = XMLoadFloat3(&_data.position[indexIt->second]);
		XMVECTOR dir = XMLoadFloat3(&_data.lookDir[indexIt->second]);
		XMVECTOR up = XMLoadFloat3(&_data.up[indexIt->second]);
		pos = XMVectorAdd(pos, XMVectorScale(up, amount));

		XMStoreFloat3(&_data.position[indexIt->second], pos);

		if (_transformChangeCallback2)
			_transformChangeCallback2(entity, pos, dir, up);
	}
}


const void TransformManager::MoveDown(const Entity& entity, const float amount)
{
	MoveUp(entity, -amount);
	return void();
}

const void TransformManager::RotateYaw(const Entity& entity, const float radians)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		_data.rotation[indexIt->second].x += radians;
		if (_data.rotation[indexIt->second].x > 360)
			_data.rotation[indexIt->second].x = 0;
		if (_data.rotation[indexIt->second].x < -360)
			_data.rotation[indexIt->second].x = 0;
		_CalcForwardUpRightVector(indexIt->second);
	}

}

const void TransformManager::RotatePitch(const Entity& entity, const float radians)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{

		_data.rotation[indexIt->second].y += radians;


		if (_data.rotation[indexIt->second].y > 90)
			_data.rotation[indexIt->second].y = 90;
		if (_data.rotation[indexIt->second].y < -90)
			_data.rotation[indexIt->second].y = -90;

		_CalcForwardUpRightVector(indexIt->second);
		/*if (mRotation.z > 360)
			mRotation.z = 0;
		

		if (mRotation.z < -360)
			mRotation.z = 0;*/
		
	}
}

const void TransformManager::RotateRoll(const Entity & entity, const float radians)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		_data.rotation[indexIt->second].z += radians;


		if (_data.rotation[indexIt->second].z > 360)
			_data.rotation[indexIt->second].z = 0;
		if (_data.rotation[indexIt->second].z < -360)
			_data.rotation[indexIt->second].z = 0;

		_CalcForwardUpRightVector(indexIt->second);
	}
}

const void TransformManager::SetPosition(const Entity & entity, const DirectX::XMFLOAT3 & position)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_data.position[indexIt->second] = position;
		XMVECTOR pos = XMLoadFloat3(&_data.position[indexIt->second]);
		XMVECTOR dir = XMLoadFloat3(&_data.lookDir[indexIt->second]);
		XMVECTOR up = XMLoadFloat3(&_data.up[indexIt->second]);
		if (_transformChangeCallback2)
			_transformChangeCallback2(entity, pos, dir, up);
	}
}

const void TransformManager::SetPosition(const Entity & entity, const DirectX::XMVECTOR & position)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		XMStoreFloat3(&_data.position[indexIt->second], position);
		XMVECTOR dir = XMLoadFloat3(&_data.lookDir[indexIt->second]);
		XMVECTOR up = XMLoadFloat3(&_data.up[indexIt->second]);
		if (_transformChangeCallback2)
			_transformChangeCallback2(entity, position, dir, up);
	}
}

const void TransformManager::SetRotation(const Entity & entity, const DirectX::XMFLOAT3 & rotation)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_data.rotation[indexIt->second] = rotation;

		_CalcForwardUpRightVector(indexIt->second);
	}
}

const void TransformManager::SetRotation(const Entity & entity, const DirectX::XMVECTOR & rotation)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		XMStoreFloat3(&_data.rotation[indexIt->second], rotation);

		_CalcForwardUpRightVector(indexIt->second);
	}
}

const void TransformManager::_CalcForwardUpRightVector(const unsigned instance)

{
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Setup where the camera is looking by default.
	XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);




	yaw = XMConvertToRadians(_data.rotation[instance].x);
	pitch = XMConvertToRadians(_data.rotation[instance].y);
	roll = XMConvertToRadians(_data.rotation[instance].z);

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	forward = XMVector3TransformCoord(forward, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);
	XMVECTOR right = XMVector3Cross(up, forward);

	XMStoreFloat3(&_data.up[instance], up);
	XMStoreFloat3(&_data.lookDir[instance], forward);
	XMStoreFloat3(&_data.right[instance], right);


	XMVECTOR pos = XMLoadFloat3(&_data.position[instance]);
	if (_transformChangeCallback2)
		_transformChangeCallback2(_data.Entity[instance], pos, forward, up);
	
}









void TransformManager::_Allocate(const unsigned numItems )
{
	if ( numItems <= _data.Capacity )
		throw("Allocation should only grow to accomodate more items, not fewer!");

	Data newData;
	const unsigned bytes = numItems * (sizeof( Entity ) + 2 * sizeof( XMFLOAT4X4 ) + 4 * sizeof( Instance ) + 5 * sizeof(XMFLOAT3));
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

	newData.position = reinterpret_cast<XMFLOAT3*>(newData.NextSibling + numItems);
	newData.rotation = reinterpret_cast<XMFLOAT3*>(newData.position + numItems);
	newData.up = reinterpret_cast<XMFLOAT3*>(newData.rotation + numItems);;
	newData.lookDir = reinterpret_cast<XMFLOAT3*>(newData.up + numItems);;
	newData.right = reinterpret_cast<XMFLOAT3*>(newData.lookDir + numItems);;



	memcpy( newData.Entity, _data.Entity, _data.Length * sizeof( Entity ) );
	memcpy( newData.Local, _data.Local, _data.Length * sizeof( XMFLOAT4X4 ) );
	memcpy( newData.World, _data.World, _data.Length * sizeof( XMFLOAT4X4 ) );
	memcpy( newData.Parent, _data.Parent, _data.Length * sizeof( Instance ) );
	memcpy( newData.FirstChild, _data.FirstChild, _data.Length * sizeof( Instance ) );
	memcpy( newData.PrevSibling, _data.PrevSibling, _data.Length * sizeof( Instance ) );
	memcpy( newData.NextSibling, _data.NextSibling, _data.Length * sizeof( Instance ) );

	memcpy(newData.position, _data.position, _data.Length * sizeof(XMFLOAT3));
	memcpy(newData.rotation, _data.position, _data.Length * sizeof(XMFLOAT3));
	memcpy(newData.up, _data.up, _data.Length * sizeof(XMFLOAT3));
	memcpy(newData.lookDir, _data.lookDir, _data.Length * sizeof(XMFLOAT3));
	memcpy(newData.right, _data.right, _data.Length * sizeof(XMFLOAT3));

	operator delete(_data.Buffer);

	_data = newData;
}