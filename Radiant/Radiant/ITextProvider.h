
#ifndef _ITEXT_PROVIDER_H_
#define _ITEXT_PROVIDER_H_
#pragma once
#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>
#include <map>
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
	uint offset;
	uint refSize;
	uint tsize;
};
struct FontData
{
	DirectX::XMFLOAT3 pos;
	std::string text;
	float FontSize;
	Fonts* font;
	uint VertexBuffer;
	DirectX::XMFLOAT4 Color;
	bool visible;
};

typedef std::map<uint, std::vector<FontData*>> TextJob;

class ITextProvider
{
public:
	//virtual void GatherJobs( std::function<const Material*(RenderJob&)> ProvideJob ) = 0;
	virtual void GatherTextJobs(TextJob& jobs) = 0;
	//virtual void GatherCam(CamData& Cam) = 0;
};

#endif