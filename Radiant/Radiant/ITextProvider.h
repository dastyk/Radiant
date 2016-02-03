
#ifndef _ITEXT_PROVIDER_H_
#define _ITEXT_PROVIDER_H_
#pragma once
#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>

struct FontType
{
	float left;
	float right;
	uint size;
};
struct Fonts
{
	FontType* Font;
	uint texture;
	uint nroffonts;
};
struct FontData
{
	DirectX::XMFLOAT3 pos;
	std::string text;
	uint FontSize;
	Fonts* font;
	uint VertexBuffer;
};


class ITextProvider
{
public:
	//virtual void GatherJobs( std::function<const Material*(RenderJob&)> ProvideJob ) = 0;
	virtual void GatherTextJobs(std::function<void(FontData&)> ProvideJob) = 0;
	//virtual void GatherCam(CamData& Cam) = 0;
};

#endif