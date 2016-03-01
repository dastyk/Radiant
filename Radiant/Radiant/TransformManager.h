#ifndef _TRANSFORM_MANAGER_H_
#define _TRANSFORM_MANAGER_H_

#include <unordered_map>
#include <DirectXMath.h>
#include "Entity.h"
#include "Event.h"

class TransformManager
{
public:
	TransformManager();
	~TransformManager();

	void CreateTransform(const Entity& entity );
	void BindChild(const Entity& parent, const Entity& child );

	const void MoveForward(const Entity& entity,const float amount);//
	const void MoveBackward(const Entity& entity, const float amount);//
	const void MoveRight(const Entity& entity, const float amount);//
	const void MoveLeft(const Entity& entity, const float amount); 
	const void MoveUp(const Entity& entity, const float amount);//
	const void MoveDown(const Entity& entity, const float amount);//
	const void MoveAlongVector(const Entity& entity, const DirectX::XMVECTOR amount);
	const void MoveAlongVector(const Entity& entity, const DirectX::XMVECTOR dir, float amount);
	const void RotateYaw(const Entity& entity, const float radians);//
	const void RotatePitch(const Entity& entity, const float radians);//
	const void RotateRoll(const Entity& entity, const float radians);

	const void SetPosition(const Entity& entity, const DirectX::XMFLOAT3& position);
	const void SetPosition(const Entity& entity, const DirectX::XMVECTOR& position);
	const void SetRotation(const Entity& entity, const DirectX::XMFLOAT3& rotation);
	const void SetRotation(const Entity& entity, const DirectX::XMVECTOR& rotation);
	const void SetScale(const Entity& entity, const DirectX::XMFLOAT3& scale);
	const void SetScale(const Entity& entity, const DirectX::XMVECTOR& scale);
	const void SetDirection(const Entity& entity, const DirectX::XMVECTOR& scale);
	
	const DirectX::XMVECTOR GetPosition(const Entity& entity);
	const DirectX::XMVECTOR GetRotation(const Entity& entity);
	const DirectX::XMVECTOR GetScale(const Entity& entity);
	const DirectX::XMVECTOR GetDirection(const Entity& entity);
	const DirectX::XMVECTOR GetRight(const Entity& entity);
	const DirectX::XMVECTOR GetUp(const Entity& entity);

	const void SetFlyMode(const Entity& entity, bool set);

	// Functions to get position (world, local), rotation quaternion, and scale. Extract from matrix
	// Don't pass scale to children

public:
	Event<void(const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up)> TransformChanged;

private:
	struct TransformComponent
	{
		Entity Entity; // Owning entity
		TransformComponent *Parent = nullptr; // Parent transform (whose reference system we are relative to).
		TransformComponent *FirstChild = nullptr; // Use this to get other children
		TransformComponent *PrevSibling = nullptr; // The previous child of our parent
		TransformComponent *NextSibling = nullptr; // The next child of our parent

		DirectX::XMFLOAT4X4 Local; // Local transform with respect to parent
		DirectX::XMFLOAT4X4 World; // Concatenation of local and parent World (final world)

		DirectX::XMFLOAT3 PositionL;
		DirectX::XMFLOAT3 PositionW;
		DirectX::XMFLOAT3 Rotation;
		DirectX::XMFLOAT3 Scale;

		DirectX::XMFLOAT3 Forward;
		DirectX::XMFLOAT3 Up;
		DirectX::XMFLOAT3 Right;

		bool FlyMode;
	};

private:
	void _Update( Entity& entity );
	void _Allocate(std::uint32_t numItems );
	void _Transform( TransformComponent* subject, TransformComponent* parent );
	void _CalcForwardUpRightVector( std::uint32_t instance );

private:
	std::unordered_map<Entity, std::uint32_t, EntityHasher> _entityToIndex; // Maps entities to what position they have in the main storage
	TransformComponent *_transforms = nullptr; // Main storage
	std::uint32_t _transformCount = 0; // Number of actual components
	std::uint32_t _allocatedCount = 0; // Number of allocated components
};

#endif // _TRANSFORM_MANAGER_H_