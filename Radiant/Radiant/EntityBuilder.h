#ifndef _ENTITYBUILDER_H_
#define  _ENTITYBUILDER_H_

#pragma once
#include "EntityController.h"
#include <functional>

class EntityBuilder
{
public:
	EntityBuilder();
	~EntityBuilder();


	const Entity& CreateButton(XMFLOAT3& position,const std::string& text, XMFLOAT4& textColor, float width, float height, const std::string& texture, std::function<void()> callback);
	const Entity& CreateCamera(XMVECTOR& position);
	const Entity& CreateObject(XMVECTOR & pos, XMVECTOR & rot, XMVECTOR & scale, const std::string& meshtext, const std::string& texture, const std::string& normal);
	const Entity& CreateOverlay(XMVECTOR & pos, float width, float height, const std::string& texture);
	//Entity& CreateInvisibleObject(XMVECTOR& pos, XMVECTOR& rot, XMVECTOR& scale)const;

	EntityController* GetEntityController();

	

private:
	EntityManager _entity;
	StaticMeshManager* _mesh = nullptr;
	TransformManager* _transform = nullptr;
	CameraManager* _camera = nullptr;
	MaterialManager* _material = nullptr;
	OverlayManager* _overlay = nullptr;
	ClickingManager* _clicking = nullptr;
	LightManager* _light = nullptr;
	BoundingManager* _bounding = nullptr;
	TextManager* _text = nullptr;


	EntityController* _controller = nullptr;
};

#endif