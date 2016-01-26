#ifndef _STATIC_MESH_MANAGER_H_
#define _STATIC_MESH_MANAGER_H_

#include "IRenderProvider.h"

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "Graphics.h"
#include "Entity.h"
//#include "Material.h"
//#include "TransformManager.h"

class StaticMeshManager : public IRenderProvider
{
public:
	StaticMeshManager( Graphics& graphics/*, TransformManager& transformManager*/ );
	~StaticMeshManager();

	void GatherJobs( std::function</*const Material**/void(RenderJob&)> ProvideJob );

	void CreateStaticMesh( Entity entity, const char *filename );

	//Material& GetMaterial( Entity entity, std::uint32_t part );
	//void SetMaterial( Entity entity, std::uint32_t part, const Material& material );

private:
	struct MeshPart
	{
		std::uint32_t IndexStart;
		std::uint32_t IndexCount;
		//Material Material;
	};

	struct MeshData
	{
		Entity OwningEntity;
		std::uint32_t VertexBuffer;
		std::uint32_t IndexBuffer;
		DirectX::XMFLOAT4X4 Transform;
		std::vector<MeshPart> Parts;
		Mesh *Mesh;
	};

private:
	void TransformChanged( Entity entity, const DirectX::XMMATRIX& transform );

private:
	std::vector<MeshData> _meshes;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	Graphics& _graphics;
};

#endif // _STATIC_MESH_MANAGER_H_