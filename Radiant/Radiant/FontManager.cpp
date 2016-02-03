#include "FontManager.h"
#include "Utils.h"
#include "System.h"

FontManager::FontManager(TransformManager& trans)
{
}


FontManager::~FontManager()
{
}

const void FontManager::BindText(const Entity & entity, std::string & text, std::string& fontName, uint fontSize)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		TraceDebug("Tried to bind text component to entity that already had one.");
		return;
	}

	auto g = System::GetGraphics();

	FontData data;

	data.font = LoadFont(fontName);
	data.text = text;
	data.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	data.FontSize = fontSize;
	//data.VertexBuffer = g->CreateTextBuffer();
	return void();
}

const void FontManager::ChangeText(const Entity & entity, std::string & text, std::string& fontName)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		
		return;
	}
	TraceDebug("Tried to change text for an entity that had no text component.");
}

const void FontManager::ChangeFontSize(const Entity & entity, uint fontSize)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{

		return;
	}
	TraceDebug("Tried to change fontsize for an entity that had no text component.");
}

Fonts * FontManager::LoadFont(std::string & fontName)
{
	return nullptr;
}
