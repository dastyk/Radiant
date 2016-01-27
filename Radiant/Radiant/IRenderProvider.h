#ifndef _IRENDER_PROVIDER_H_
#define _IRENDER_PROVIDER_H_

#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>

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
	DirectX::XMFLOAT4X4 Transform;
	//Material Material;
	RenderJob() {}
	// Should be created implicitly, but Microsoft is Microsoft.
	RenderJob(RenderJob&& other)
	{
		IndexStart = other.IndexStart;
		IndexCount = other.IndexCount;
		Transform = other.Transform;
		//Material = std::move( other.Material );
	}
	RenderJob(uint is, uint uc, DirectX::XMFLOAT4X4 t)
	{
		IndexStart = is;
		IndexCount = uc;
		Transform = t;
		//Material = std::move( other.Material );
	}
	RenderJob& operator=(RenderJob&& rhs)
	{
		if (this != &rhs)
		{
			IndexStart = rhs.IndexStart;
			IndexCount = rhs.IndexCount;
			Transform = rhs.Transform;
			//Material = std::move( rhs.Material );
		}
		return *this;
	}
};

typedef std::vector<RenderJob> RenderJobMap4;
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