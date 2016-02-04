#ifndef _TEXT_MANAGER_H_
#define _TEXT_MANAGER_H_

#pragma once
#include "General.h"
#include "Entity.h"
#include "TransformManager.h"
#include "Graphics.h"
#include "ITextProvider.h"

class TextManager : public ITextProvider
{
public:
	TextManager(TransformManager& trans);
	~TextManager();

	void GatherTextJobs(TextJob2& jobs);

	const void BindText(const Entity& entity, std::string text, std::string fontName, uint fontSize);

	const void ChangeText(const Entity& entity, std::string text, std::string fontName);
	const void ChangeFontSize(const Entity& entity, uint fontSize);
private:

	Fonts* LoadFont(std::string& fontName);


	std::vector<FontData> _textStrings;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	std::unordered_map<std::string, Fonts*> _loadedFonts;
};

#endif