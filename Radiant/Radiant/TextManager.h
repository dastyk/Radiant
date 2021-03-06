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

	void GatherTextJobs(TextJob& jobs);

	const void BindText(const Entity& entity, const std::string& text, const std::string& fontName, uint fontSize, const DirectX::XMFLOAT4& Color);

	const void ChangeText(const Entity& entity, const std::string& text);
	const void ChangeFontSize(const Entity& entity, uint fontSize);
	const void ChangeColor(const Entity& entity, const DirectX::XMFLOAT4& Color);
	const void ReleaseText(const Entity& entity);
	const void ToggleVisible(const Entity& entity, bool visible);
	const void BindToRenderer(bool exclusive);
	const std::string GetText(const Entity& entity)const;
	const float GetLength(const Entity& entity);

private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& tran, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up );

	Fonts* LoadFont(const std::string& fontName);

	std::unordered_map<Entity, FontData*, EntityHasher> _entityToData;
	std::unordered_map<std::string, Fonts*> _loadedFonts;
};

#endif