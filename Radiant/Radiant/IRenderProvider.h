#ifndef _IRENDER_PROVIDER_H_
#define _IRENDER_PROVIDER_H_

#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>
#include "ShaderData.h"

//#include "Material.h"





//struct RenderJob
//{
//	std::uint32_t VertexBuffer;
//	std::uint32_t IndexBuffer;
//	std::uint32_t IndexStart;
//	std::uint32_t IndexCount;
//	DirectX::XMFLOAT4X4 Transform;
//	//Material Material;
//
//	RenderJob() {};
//
//	// Should be created implicitly, but Microsoft is Microsoft.
//	RenderJob( RenderJob&& other )
//	{
//		VertexBuffer = other.VertexBuffer;
//		IndexBuffer = other.IndexBuffer;
//		IndexStart = other.IndexStart;
//		IndexCount = other.IndexCount;
//		Transform = other.Transform;
//		//Material = std::move( other.Material );
//	}
//
//	RenderJob& operator=( RenderJob&& rhs )
//	{
//		if ( this != &rhs )
//		{
//			VertexBuffer = rhs.VertexBuffer;
//			IndexBuffer = rhs.IndexBuffer;
//			IndexStart = rhs.IndexStart;
//			IndexCount = rhs.IndexCount;
//			Transform = rhs.Transform;
//			//Material = std::move( rhs.Material );
//		}
//		return *this;
//	}
//};


struct RenderJob
{
	std::uint32_t IndexStart;
	std::uint32_t IndexCount;
	const ShaderData* ShaderData;

	RenderJob() : IndexStart(0) , IndexCount(0), ShaderData(nullptr)
	{
	}
	// Should be created implicitly, but Microsoft is Microsoft.
	RenderJob(RenderJob&& other) :IndexStart(other.IndexCount) , IndexCount(other.IndexStart), ShaderData(other.ShaderData)
	{
	}
	RenderJob(uint is, uint uc, struct ShaderData& material) :IndexStart(uc), IndexCount(is), ShaderData(&material)
	{

	}
	RenderJob& operator=(RenderJob&& rhs)
	{
		if (this != &rhs)
		{
			IndexStart = rhs.IndexStart;
			IndexCount = rhs.IndexCount;
			ShaderData = rhs.ShaderData;
		}
		return *this;
	}
	RenderJob(const RenderJob& other)
	{
		IndexStart = other.IndexStart;
		IndexCount = other.IndexCount;
		ShaderData = other.ShaderData;
	}
};
struct MeshPart
{
	std::uint32_t IndexStart;
	std::uint32_t IndexCount;
	const ShaderData* Material = nullptr;
	DirectX::XMFLOAT4X4* translation;
	bool Visible;
};

//struct TextureSort
//{
//	std::uint32_t TextureID;
//	std::vector<MeshPart*> data;
//};


typedef std::vector<MeshPart*> RenderJobMap4; // TODO: and/or remove the maps for some better soring method. This one might cause unnecessary copying and memory allocation.
typedef std::map<uint, RenderJobMap4> RenderJobMap3;
typedef std::map<uint, RenderJobMap3> RenderJobMap2;
typedef std::map<uint, RenderJobMap2> RenderJobMap;




class IRenderProvider
{
public:
	//virtual void GatherJobs( std::function<const Material*(RenderJob&)> ProvideJob ) = 0;
//	virtual void GatherJobs( std::function<void(RenderJob&)> ProvideJob ) = 0;
	virtual void GatherJobs(RenderJobMap& jobs) = 0;
};

#endif