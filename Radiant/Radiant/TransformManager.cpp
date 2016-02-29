#include "TransformManager.h"

#include <memory>
#include "System.h"

using namespace std;
using namespace DirectX;

TransformManager::TransformManager()
{
	_Allocate( 20 ); // Something to start with
}

TransformManager::~TransformManager()
{
	operator delete(_transforms);
}

void TransformManager::CreateTransform(const Entity& entity )
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		return;
	}

	if ( _transformCount == _allocatedCount )
		_Allocate( _allocatedCount * 2 );

	uint32_t index = _transformCount++;
	_transforms[index] = TransformComponent();
	
	TransformComponent& current = _transforms[index];
	current.Entity = entity;
	XMStoreFloat4x4( &current.Local, XMMatrixIdentity() );
	XMStoreFloat4x4( &current.World, XMMatrixIdentity() );

	XMStoreFloat3( &current.PositionL, XMVectorSet( 0, 0, 0, 1 ) );
	XMStoreFloat3( &current.PositionW, XMVectorSet( 0, 0, 0, 1 ) );
	XMStoreFloat3( &current.Rotation, XMVectorSet( 0, 0, 0, 0 ) );
	XMStoreFloat3( &current.Scale, XMVectorSet( 1, 1, 1, 0 ) );

	XMStoreFloat3( &current.Forward, XMVectorSet( 0, 0, 1, 0 ) );
	XMStoreFloat3( &current.Up, XMVectorSet( 0, 1, 0, 0 ) );
	XMStoreFloat3( &current.Right, XMVectorSet( 1, 0, 0, 0 ) );

	current.FlyMode = true;

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
		uint32_t parentIndex = _entityToIndex[parent];
		uint32_t childIndex = _entityToIndex[child];

		TransformComponent *parent = _transforms + parentIndex;
		TransformComponent *child = _transforms + childIndex;

		// Child doesn't already have a parent
		if ( child->Parent == nullptr )
		{
			// Set parent of child
			child->Parent = parent;

			TransformComponent *childOfParent = parent->FirstChild;

			// Parent has child(ren): find last valid child
			if ( childOfParent )
			{
				// As long as we have another sibling, traverse to it (walk to the last).
				while ( childOfParent->NextSibling )
					childOfParent = childOfParent->NextSibling;

				// We are now at the last sibling. Have its next sibling be
				// the new child, and set the previous sibling of the new one.
				childOfParent->NextSibling = child;
				child->PrevSibling = childOfParent;
			}
			// Parent doesn't have any children: add directly
			else
			{
				parent->FirstChild = child;
			}
		}
	}
}

const void TransformManager::MoveForward(const Entity& entity, const float amount)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		XMVECTOR dir = XMLoadFloat3( &_transforms[indexIt->second].Forward );
		XMVECTOR pos = XMLoadFloat3( &_transforms[indexIt->second].PositionL );
		XMVECTOR up = XMLoadFloat3( &_transforms[indexIt->second].Up );

		if ( _transforms[indexIt->second].FlyMode )
		{		
			pos = XMVectorAdd(pos, XMVectorScale(dir, amount));
			XMStoreFloat3( &_transforms[indexIt->second].PositionL, pos );
		}
		else
		{
			XMVECTOR dir2 = XMVector3Normalize( XMVectorSet(XMVectorGetX(dir), 0.0f, XMVectorGetZ(dir), 0.0f));
			pos = XMVectorAdd(pos, XMVectorScale(dir2, amount));
			XMStoreFloat3( &_transforms[indexIt->second].PositionL, pos );
		}

		_Transform( &_transforms[indexIt->second], _transforms[indexIt->second].Parent );
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
		XMVECTOR pos = XMLoadFloat3( &_transforms[indexIt->second].PositionL );
		XMVECTOR right = XMLoadFloat3( &_transforms[indexIt->second].Right );
		pos = XMVectorAdd( pos, XMVectorScale( right, amount ) );
		XMStoreFloat3( &_transforms[indexIt->second].PositionL, pos );

		XMVECTOR up = XMLoadFloat3( &_transforms[indexIt->second].Up );
		XMVECTOR dir = XMLoadFloat3( &_transforms[indexIt->second].Forward );

		if ( _transforms[indexIt->second].FlyMode )
		{
			pos = XMVectorAdd(pos, XMVectorScale(XMVector3Normalize(right), amount));
			XMStoreFloat3( &_transforms[indexIt->second].PositionL, pos );
		}
		else
		{
			XMVECTOR right2 = XMVector3Normalize(XMVectorSet(XMVectorGetX(right), 0.0f, XMVectorGetZ(right), 0.0f));
			pos = XMVectorAdd(pos, XMVectorScale(right2, amount));
			XMStoreFloat3(&_transforms[indexIt->second].PositionL, pos);
		}

		_Transform( &_transforms[indexIt->second], _transforms[indexIt->second].Parent );
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
		XMVECTOR pos = XMLoadFloat3(&_transforms[indexIt->second].PositionL);
		XMVECTOR dir = XMLoadFloat3(&_transforms[indexIt->second].Forward);
		XMVECTOR up = XMLoadFloat3(&_transforms[indexIt->second].Up);

		if ( _transforms[indexIt->second].FlyMode )
		{
			pos = XMVectorAdd(pos, XMVectorScale(up, amount));
			XMStoreFloat3(&_transforms[indexIt->second].PositionL, pos);
		}
		else
		{
			XMVECTOR up2 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			pos = XMVectorAdd(pos, XMVectorScale(up2, amount));
			XMStoreFloat3(&_transforms[indexIt->second].PositionL, pos);
		}

		_Transform( &_transforms[indexIt->second], _transforms[indexIt->second].Parent );
	}
}


const void TransformManager::MoveDown(const Entity& entity, const float amount)
{
	MoveUp(entity, -amount);
	return void();
}

const void TransformManager::MoveAlongVector(const Entity& entity, const XMVECTOR amount)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		XMVECTOR pos = XMLoadFloat3(&_transforms[indexIt->second].PositionL);
		pos = XMVectorAdd(pos, amount);
		XMStoreFloat3(&_transforms[indexIt->second].PositionL, pos);
		_Transform(&_transforms[indexIt->second], _transforms[indexIt->second].Parent);
	}
	return void();
}

const void TransformManager::RotateYaw(const Entity& entity, const float radians)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		_transforms[indexIt->second].Rotation.y += radians;

		if ( _transforms[indexIt->second].Rotation.y > 360 )
			_transforms[indexIt->second].Rotation.y -= 360;
		if ( _transforms[indexIt->second].Rotation.y < -360 )
			_transforms[indexIt->second].Rotation.y += 360;
	}

	_CalcForwardUpRightVector(indexIt->second);
}

const void TransformManager::RotatePitch(const Entity& entity, const float radians)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		_transforms[indexIt->second].Rotation.x += radians;

		if ( !_transforms[indexIt->second].FlyMode )
		{
			if ( _transforms[indexIt->second].Rotation.x > 89 )
				_transforms[indexIt->second].Rotation.x = 89;
			if ( _transforms[indexIt->second].Rotation.x < -89 )
				_transforms[indexIt->second].Rotation.x = -89;
		}
		else
		{
			if ( _transforms[indexIt->second].Rotation.x > 360 )
				_transforms[indexIt->second].Rotation.x = 0;
			if ( _transforms[indexIt->second].Rotation.x < -360 )
				_transforms[indexIt->second].Rotation.x = -0;
		}

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
		_transforms[indexIt->second].Rotation.z += radians;

		if ( _transforms[indexIt->second].Rotation.z > 360 )
			_transforms[indexIt->second].Rotation.z = 0;
		if ( _transforms[indexIt->second].Rotation.z < -360 )
			_transforms[indexIt->second].Rotation.z = -0;
		
		_CalcForwardUpRightVector(indexIt->second);
	}
}

const void TransformManager::SetPosition(const Entity & entity, const DirectX::XMFLOAT3 & position)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_transforms[indexIt->second].PositionL = position;
		XMVECTOR pos = XMLoadFloat3(&_transforms[indexIt->second].PositionL);
		XMVECTOR dir = XMLoadFloat3(&_transforms[indexIt->second].Forward);
		XMVECTOR up = XMLoadFloat3(&_transforms[indexIt->second].Up);

		_Transform( &_transforms[indexIt->second], _transforms[indexIt->second].Parent );
	}
}

const void TransformManager::SetPosition(const Entity & entity, const DirectX::XMVECTOR & position)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		XMStoreFloat3( &_transforms[indexIt->second].PositionL, position );
		XMVECTOR dir = XMLoadFloat3(&_transforms[indexIt->second].Forward);
		XMVECTOR up = XMLoadFloat3(&_transforms[indexIt->second].Up);

		_Transform( &_transforms[indexIt->second], _transforms[indexIt->second].Parent );
	}
}

const void TransformManager::SetRotation(const Entity & entity, const DirectX::XMFLOAT3 & rotation)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_transforms[indexIt->second].Rotation = rotation;

		_CalcForwardUpRightVector(indexIt->second);
	}
}

const void TransformManager::SetRotation(const Entity & entity, const DirectX::XMVECTOR & rotation)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		XMStoreFloat3(&_transforms[indexIt->second].Rotation, rotation);

		_CalcForwardUpRightVector(indexIt->second);
	}
}

const void TransformManager::SetScale(const Entity & entity, const DirectX::XMFLOAT3 & scale)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_transforms[indexIt->second].Scale = scale;

		_Transform( &_transforms[indexIt->second], _transforms[indexIt->second].Parent );
	}
}

const void TransformManager::SetScale(const Entity & entity, const DirectX::XMVECTOR & scale)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		XMStoreFloat3( &_transforms[indexIt->second].Scale, scale );

		_Transform( &_transforms[indexIt->second], _transforms[indexIt->second].Parent );
	}
}

const void TransformManager::SetDirection(const Entity& entity, const DirectX::XMVECTOR& direction)
{
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	auto instance = _entityToIndex.find(entity);

	if (instance == _entityToIndex.end())
	{
		return;
	}

	// Setup the vector that points upwards.
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Setup where the camera is looking by default.
	XMVECTOR forward = XMVectorSet(0.0f,0.0f,1.0f,0.0f);
	XMVECTOR ndir = XMVector3Normalize(direction);
	
	XMVECTOR xv = XMVectorSet(XMVectorGetX(ndir), 0.0f,0.0f,0.0f);
	XMVECTOR yv = XMVectorSet(0.0f, XMVectorGetY(ndir), 0.0f, 0.0f);
	XMVECTOR zv = XMVectorSet(0.0f, 0.0f, XMVectorGetZ(ndir), 0.0f);

	_transforms[instance->second].Rotation.y = XMVectorGetX(XMVectorScale( XMVector3AngleBetweenNormals(xv, forward), XMVectorGetX(ndir)));
	_transforms[instance->second].Rotation.x = XMVectorGetX(XMVectorScale(XMVector3AngleBetweenNormals(yv, forward), XMVectorGetY(ndir)));
	_transforms[instance->second].Rotation.z = XMVectorGetX(XMVectorScale(XMVector3AngleBetweenNormals(zv, forward), XMVectorGetZ(ndir)));

	yaw = XMConvertToRadians(_transforms[instance->second].Rotation.y);
	pitch = XMConvertToRadians(_transforms[instance->second].Rotation.x);
	roll = XMConvertToRadians(_transforms[instance->second].Rotation.z);

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	//forward = XMVector3TransformCoord(forward, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);
	XMVECTOR right = XMVector3Cross(up, forward);

	XMStoreFloat3(&_transforms[instance->second].Up, up);
	XMStoreFloat3(&_transforms[instance->second].Forward, forward);
	XMStoreFloat3(&_transforms[instance->second].Right, right);

	_Transform(&_transforms[instance->second], _transforms[instance->second].Parent);
}

const DirectX::XMVECTOR TransformManager::GetPosition(const Entity & entity)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		return XMLoadFloat3( &_transforms[indexIt->second].PositionL );
	}

	return XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

const DirectX::XMVECTOR TransformManager::GetRotation(const Entity & entity)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		return XMLoadFloat3(&_transforms[indexIt->second].Rotation);
	}

	return XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

const DirectX::XMVECTOR TransformManager::GetScale(const Entity & entity)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		return XMLoadFloat3(&_transforms[indexIt->second].Scale);
	}

	return XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

const DirectX::XMVECTOR TransformManager::GetDirection(const Entity & entity)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		return XMLoadFloat3(&_transforms[indexIt->second].Forward);
	}

	return XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
}

const void TransformManager::SetFlyMode(const Entity & entity, bool set)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_transforms[indexIt->second].FlyMode = set;
	}
}

void TransformManager::_CalcForwardUpRightVector(uint32_t instance)
{
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Setup where the camera is looking by default.
	XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	
	yaw = XMConvertToRadians( _transforms[instance].Rotation.y );
	pitch = XMConvertToRadians( _transforms[instance].Rotation.x );
	roll = XMConvertToRadians( _transforms[instance].Rotation.z );

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	forward = XMVector3TransformCoord(forward, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);
	XMVECTOR right = XMVector3Cross(up, forward);

	XMStoreFloat3(&_transforms[instance].Up, up);
	XMStoreFloat3(&_transforms[instance].Forward, forward);
	XMStoreFloat3(&_transforms[instance].Right, right);

	_Transform( &_transforms[instance], _transforms[instance].Parent );
}

void TransformManager::_Update(Entity & entity)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		TransformComponent *parent = _transforms[indexIt->second].Parent;

		// Get parent transform if valid parent instance (ie, it has a parent).
		// If no parent: use identity matrix
		//XMMATRIX parentTransform = parent.i != -1 ? XMLoadFloat4x4(&_data.World[parent.i]) : XMMatrixIdentity();

		_Transform(&_transforms[indexIt->second], parent);
	}
}

void TransformManager::_Transform( TransformComponent* subject, TransformComponent* parent )
{
	XMVECTOR lPos = XMLoadFloat3( &subject->PositionL );
	XMVECTOR wPos = lPos;
	XMVECTOR rot = XMLoadFloat3( &subject->Rotation );
	XMVECTOR scale = XMLoadFloat3( &subject->Scale );

	float yaw = XMConvertToRadians( subject->Rotation.y );
	float pitch = XMConvertToRadians( subject->Rotation.x );
	float roll = XMConvertToRadians( subject->Rotation.z );

	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMMATRIX tran = XMMatrixScalingFromVector(scale); 
	tran *= XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	tran *= XMMatrixTranslationFromVector(lPos);

	XMStoreFloat4x4( &subject->Local, tran );

	if (parent)
	{
		XMVECTOR pwPos = XMLoadFloat3( &parent->PositionW );
		XMMATRIX pw = XMLoadFloat4x4( &parent->World );
		tran *= pw;
		//wPos = XMVectorSetW(wPos, 1.0f);
		//XMVectorMultiply(wPos, tran);
		
		wPos = XMVector3Transform(wPos, pw);
		//wPos = wPos*pw;
		//wPos += pwPos;
	}

	XMStoreFloat3( &subject->PositionW, wPos );
	XMStoreFloat4x4( &subject->World, tran );
	XMVECTOR dir = XMLoadFloat3(&subject->Forward);
	XMVECTOR up = XMLoadFloat3(&subject->Up);

	TransformChanged( subject->Entity, tran, wPos, dir, up );

	TransformComponent *child = subject->FirstChild;
	// while valid child
	while (child)
	{
		_Transform( child, subject );
		child = child->NextSibling;
	}
}

void TransformManager::_Allocate(uint32_t numItems )
{
	if (numItems <= _allocatedCount)
	{
		TraceDebug("Allocation should only grow to accomodate more items, not fewer!");
		return;
	}
	
	uint32_t bytes = numItems * sizeof( TransformComponent );
	TransformComponent *newData = static_cast<TransformComponent*>(operator new(bytes));
	_allocatedCount = numItems;

	memcpy( newData, _transforms, sizeof( TransformComponent ) * _transformCount );

	// Now we have allocated new memory, copied the old data, and freed the old memory.
	// Using the old pointer value, we can now fix pointer references in the array.
	// Only do this if the pointer was actually pointing at something (otherwise
	// nullptrs would become something specific). We fix pointer by getting difference
	// between pointer value and old array (offset) and adding that to the new array.
	for ( auto p = newData; p < newData + _transformCount; ++p )
	{
		if ( p->Parent )
		{
			p->Parent = newData + (p->Parent - _transforms);
		}

		if ( p->FirstChild )
		{
			p->FirstChild = newData + (p->FirstChild - _transforms);
		}

		if ( p->PrevSibling )
		{
			p->PrevSibling = newData + (p->PrevSibling - _transforms);
		}

		if ( p->NextSibling )
		{
			p->NextSibling = newData + (p->NextSibling - _transforms);
		}
	}

	operator delete(_transforms);
	_transforms = newData;
}