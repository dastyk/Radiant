#ifndef _STATIC_MESH_MANAGER_H_
#define _STATIC_MESH_MANAGER_H_

#include "IRenderProvider.h"

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "Graphics.h"
#include "Entity.h"
#include "MaterialManager.h"
#include <map>
#include <Shlwapi.h>
#include "ShaderData.h"

#pragma comment(lib, "Shlwapi.lib")

//#include "Material.h"
#include "TransformManager.h"


class StaticMeshManager : public IRenderProvider
{
public:
	StaticMeshManager(TransformManager& transformManager, MaterialManager& materialManager );
	~StaticMeshManager();

	//void GatherJobs( std::function</*const Material**/void(RenderJob&)> ProvideJob );
	void GatherJobs(RenderJobMap& jobs);

	void ReleaseMesh(Entity entity);
	void CreateStaticMesh( Entity entity, const char *filename);
	void CreateStaticMesh(Entity entity, Mesh* mesh);
	void CreateStaticMesh(Entity entity, const char *filename, std::vector<DirectX::XMFLOAT3>& pos, std::vector<DirectX::XMFLOAT2>& uvs, std::vector<uint>& indices);
	void CreateStaticMesh(Entity entity, const char *filename, std::vector<DirectX::XMFLOAT3>& pos, std::vector<DirectX::XMFLOAT2>& uvs, std::vector<uint>& indices, std::vector<SubMeshInfo>& subMeshInfo);

	const Mesh* GetMesh(const Entity& entity);

	void Hide( Entity entity, std::uint32_t subMesh );
	void Show( Entity entity, std::uint32_t subMesh );
	void ToggleVisibility( Entity entity, std::uint32_t subMesh );
	void SetInFrustum(const Entity& entity, bool infrustum);
	void SetInFrustum(std::vector<Entity>& entites);


	const void BindToRendered(bool exclusive);

	//Material& GetMaterial( Entity entity, std::uint32_t part );
	//void SetMaterial( Entity entity, std::uint32_t part, const Material& material );

private:
	

	struct MeshData
	{
		Entity OwningEntity;
		std::uint32_t VertexBuffer;
		std::uint32_t IndexBuffer;
		DirectX::XMFLOAT4X4 Transform;
		std::vector<MeshPart> Parts;
		Mesh *Mesh;
		bool inFrustum;
	};
private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up );
	void _MaterialChanged(const Entity& entity, const ShaderData* material, int32_t subMesh);
	void MaterialChanged(Entity entity, const ShaderData* material);
	void _SetDefaultMaterials(const Entity& entity, const ShaderData* material);
	void _MaterialCreated( const Entity& entity, const ShaderData* material );

private:
	std::vector<MeshData> _meshes;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	Graphics& _graphics;

	std::unordered_map<std::string, MeshData> _loadedFiles;
};

#endif // _STATIC_MESH_MANAGER_H_