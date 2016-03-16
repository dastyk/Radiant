#ifndef _DECAL_MANAGER_H_
#define _DECAL_MANAGER_H_

#include "IDecalProvider.h"
#include "Entity.h"
#include "MaterialManager.h"
#include "TransformManager.h"
#include <DirectXMath.h>
#include <unordered_map>

class DecalManager : public IDecalProvider
{
public:
	DecalManager(MaterialManager& matman, TransformManager& traman);

	~DecalManager();
	void SetColorTexture(Entity entity, const wchar_t* name);
	void SetNormalTexture(Entity entity, const wchar_t* name);
	void SetEmissiveTexture(Entity entity, const wchar_t* name);
	void SetRoughness(Entity entity, float value);
	void SetMetallic(Entity entity, float value);
	void BindDecal(Entity entity);
	void ReleaseDecal(Entity entity);
	void GatherDecals(DecalVector& decals, DecalGroupVector& dgv);
	void BindToRenderer(bool yes);

private:
	void _MaterialChanged(const Entity& entity, const ShaderData* data);
	void _TransformChanged(const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up);
	std::unordered_map<Entity, Decal, EntityHasher> _entityToDecal;
	std::unordered_map<Entity, DirectX::XMFLOAT4X4, EntityHasher> _cachedWorldTransforms;
	std::unordered_map<int32_t, DecalVector> _decalVectors;
	std::unordered_map<int32_t, DecalGroup> _decalGroups;
	MaterialManager& _materialManager;
	TransformManager& _transformManager;
};

#endif

