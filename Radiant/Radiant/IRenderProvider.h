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
	ShaderData ShaderData;

	RenderJob() {}
	// Should be created implicitly, but Microsoft is Microsoft.
	RenderJob(RenderJob&& other)
	{
		IndexStart = other.IndexStart;
		IndexCount = other.IndexCount;
		ShaderData = std::move( other.ShaderData );
	}
	RenderJob(uint is, uint uc, struct ShaderData material)
	{
		IndexStart = is;
		IndexCount = uc;
		ShaderData = material;
	}
	RenderJob& operator=(RenderJob&& rhs)
	{
		if (this != &rhs)
		{
			IndexStart = rhs.IndexStart;
			IndexCount = rhs.IndexCount;
			ShaderData = std::move( rhs.ShaderData );
		}
		return *this;
	}
};

typedef std::vector<RenderJob> RenderJobMap4; // TODO: Change this to pointer, and/or remove the maps for some better soring method. This one might cause unnecessary copying.
typedef std::map<void*, RenderJobMap4> RenderJobMap3;
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