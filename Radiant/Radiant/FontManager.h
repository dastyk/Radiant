#ifndef _FONT_MANAGER_H_
#define _FONT_MANAGER_H_

#pragma once
#include "General.h"
#include "Entity.h"
#include "TransformManager.h"
#include "Graphics.h"

struct FontType
{
	float left;
	float right;
	uint size;
};
struct Fonts
{
	FontType Font[95];
};
struct FontData
{
	DirectX::XMFLOAT3 pos;
	std::string text;
	uint FontSize;
	Fonts* font;
	uint VertexBuffer;
};

class FontManager
{
public:
	FontManager(TransformManager& trans);
	~FontManager();

	const void BindText(const Entity& entity, std::string& text, std::string& fontName, uint fontSize);

	const void ChangeText(const Entity& entity, std::string& text, std::string& fontName);
	const void ChangeFontSize(const Entity& entity, uint fontSize);
private:

	Fonts* LoadFont(std::string& fontName);


	std::vector<FontData> _textStrings;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	std::unordered_map<std::string, Fonts> _loadedFonts;
};

#endif