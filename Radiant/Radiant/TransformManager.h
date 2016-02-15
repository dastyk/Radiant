#ifndef _TRANSFORM_MANAGER_H_
#define _TRANSFORM_MANAGER_H_

#include <functional>
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
	const void RotateYaw(const Entity& entity, const float radians);//
	const void RotatePitch(const Entity& entity, const float radians);//
	const void RotateRoll(const Entity& entity, const float radians);

	const void SetPosition(const Entity& entity, const DirectX::XMFLOAT3& position);
	const void SetPosition(const Entity& entity, const DirectX::XMVECTOR& position);
	const void SetRotation(const Entity& entity, const DirectX::XMFLOAT3& rotation);
	const void SetRotation(const Entity& entity, const DirectX::XMVECTOR& rotation);
	const void SetScale(const Entity& entity, const DirectX::XMFLOAT3& scale);
	const void SetScale(const Entity& entity, const DirectX::XMVECTOR& scale);
	
	const DirectX::XMVECTOR& GetPosition(const Entity& entity);
	const DirectX::XMVECTOR& GetRotation(const Entity& entity);
	const DirectX::XMVECTOR& GetScale(const Entity& entity);

	const void SetFlyMode(const Entity& entity, bool set);

public:
	Event<void(const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up)> TransformChanged;

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


		DirectX::XMFLOAT3* lPosition;

		DirectX::XMFLOAT3* wPosition;
		DirectX::XMFLOAT3* rotation;
		DirectX::XMFLOAT3* scale;

		DirectX::XMFLOAT3* up;
		DirectX::XMFLOAT3* lookDir;
		DirectX::XMFLOAT3* right;

		bool* flyMode;
	};

private:
	const void _Update(Entity& entity);
	void _Allocate(const unsigned numItems );
	void _Transform(const  unsigned instance, Instance parent);
	const void _CalcForwardUpRightVector(const unsigned instance);

private:
	Data _data;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
};

#endif // _TRANSFORM_MANAGER_H_