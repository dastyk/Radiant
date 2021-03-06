#ifndef _IRENDER_PROVIDER_H_
#define _IRENDER_PROVIDER_H_

#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <unordered_map>
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
	bool inFrustum;
};

//struct TextureSort
//{
//	std::uint32_t TextureID;
//	std::vector<MeshPart*> data;
//};


typedef std::vector<MeshPart*>RJM4; // TODO: and/or remove the maps for some better soring method. This one might cause unnecessary copying and memory allocation.
typedef std::unordered_map<uint64_t, RJM4> RJM3;
typedef std::unordered_map<uint, RJM3> RJM2;
typedef std::unordered_map<uint, RJM2> RJM;




class IRenderProvider
{
public:
	//virtual void GatherJobs( std::function<const Material*(RenderJob&)> ProvideJob ) = 0;
//	virtual void GatherJobs( std::function<void(RenderJob&)> ProvideJob ) = 0;
	virtual void GatherJobs(RJM& jobs) = 0;
};

#endif