#ifndef _TRANSFORM_MANAGER_H_
#define _TRANSFORM_MANAGER_H_

#include <functional>
#include <unordered_map>
#include <DirectXMath.h>
#include "Entity.h"

class TransformManager
{
public:
	TransformManager();
	~TransformManager();

	void CreateTransform( Entity entity );
	void BindChild( Entity parent, Entity child );
	void SetTransform( const Entity& entity, const DirectX::XMMATRIX& transform );
	DirectX::XMMATRIX GetTransform( const Entity& entity ) const;

	void SetTransformChangeCallback( std::function<void( Entity, const DirectX::XMMATRIX& )> callback ) { _transformChangeCallback = callback; } // mesh
	void SetTransformChangeCallback2( std::function<void( Entity, const DirectX::XMVECTOR &, const DirectX::XMVECTOR &, const DirectX::XMVECTOR &)> callback ) { _transformChangeCallback2 = callback; } // camera
	void SetTransformChangeCallback3( std::function<void( Entity, const DirectX::XMVECTOR &)> callback ) { _transformChangeCallback3 = callback; } // overlay
	//void SetTransformChangeCallback4( std::function<void( Entity, const DirectX::XMMATRIX& )> callback ) { mTransformChangeCallback4 = callback; } // capsule
	//void SetTransformChangeCallback5( std::function<void( Entity, const DirectX::XMMATRIX& )> callback ) { mTransformChangeCallback5 = callback; } // directional


	const void MoveForward(Entity& entity, float amount);//
	const void MoveBackward(Entity& entity, float amount);//
	const void MoveRight(Entity& entity, float amount);//
	const void MoveLeft(Entity& entity, float amount); //move all this to transform manager
	const void MoveUp(Entity& entity, float amount);//
	const void MoveDown(Entity& entity, float amount);//
	const void RotateYaw(Entity& entity, float radians);//
	const void RotatePitch(Entity& entity, float radians);//

	

	const void SetLookDir(const Entity& entity, const DirectX::XMVECTOR& lookDir);
private:
	struct Instance
	{
		int i;
	};

	struct Data
	{
		unsigned Length; // Number of actual instances
		unsigned Capacity; // Number of allocated instances
		void *Buffer; // Raw data

		Entity *Entity; // Owning entity
		DirectX::XMFLOAT4X4 *Local; // Local transform with respect to parent
		DirectX::XMFLOAT4X4 *World; // Concatenation of local and parent world (final world)
		Instance *Parent; // Parent instance of this instance
		Instance *FirstChild; // First child instance of this instance
		Instance *PrevSibling; // Previous sibling instance of this instance
		Instance *NextSibling; // Next sibling instance of this instance

		DirectX::XMFLOAT3* position;
		DirectX::XMFLOAT3* rotation;

		DirectX::XMFLOAT3* up;
		DirectX::XMFLOAT3* lookDir;
		DirectX::XMFLOAT3* right;
	};

private:
	void _Allocate( unsigned numItems );
	void _Transform( unsigned instance, const DirectX::XMMATRIX& parent );
	const void _CalcForwardUpRightVector(unsigned instance);
private:
	Data _data;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;

	// TODO: Better event system? // Make it so that the callbacks only need to be called when a entity is added(and removed)? by passing pointers? // or perhaps so that data is only collected when needed, e.g. when a mesh
	// should be rendered it ask to get the latest translation.
	std::function<void( Entity, const DirectX::XMMATRIX& )> _transformChangeCallback;
	std::function<void( Entity, const DirectX::XMVECTOR &, const DirectX::XMVECTOR &, const DirectX::XMVECTOR &)> _transformChangeCallback2;
	std::function<void( Entity, const DirectX::XMVECTOR& )> _transformChangeCallback3;
	//std::function<void( Entity, const DirectX::XMMATRIX& )> mTransformChangeCallback4;
	//std::function<void( Entity, const DirectX::XMMATRIX& )> mTransformChangeCallback5;
};

#endif // _TRANSFORM_MANAGER_H_