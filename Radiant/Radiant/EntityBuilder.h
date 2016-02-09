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

	const Entity& CreateImage(XMFLOAT3& position, float width, float height, const std::string& texture);
	const Entity& CreateLabel(XMFLOAT3& position, const std::string& text, XMFLOAT4& textColor, float width, float height, const std::string& texture);
	const Entity& CreateButton(XMFLOAT3& position,const std::string& text, XMFLOAT4& textColor, float width, float height, const std::string& texture, std::function<void()> callback);
	const Entity& CreateCamera(XMVECTOR& position);
	const Entity& CreateObject(XMVECTOR & pos, XMVECTOR & rot, XMVECTOR & scale, const std::string& meshtext, const std::string& texture = "Assets/Textures/default_color.png", const std::string& normal = "Assets/Textures/default_normal.png", const std::string& displacement = "Assets/Textures/default_displacement.png");

	const Entity& CreateOverlay(XMVECTOR & pos, float width, float height, const std::string& texture);
	//Entity& CreateInvisibleObject(XMVECTOR& pos, XMVECTOR& rot, XMVECTOR& scale)const;

	EntityController* GetEntityController();

	EntityManager& EntityC();
	StaticMeshManager* Mesh()const;
	TransformManager* Transform()const;
	CameraManager* Camera()const;
	MaterialManager* Material()const;
	OverlayManager* Overlay()const;
	EventManager* Event()const;
	LightManager* Light()const;
	BoundingManager* Bounding()const;
	TextManager* Text()const;

private:
	EntityManager _entity;
	StaticMeshManager* _mesh = nullptr;
	TransformManager* _transform = nullptr;
	CameraManager* _camera = nullptr;
	MaterialManager* _material = nullptr;
	OverlayManager* _overlay = nullptr;
	EventManager* _event = nullptr;
	LightManager* _light = nullptr;
	BoundingManager* _bounding = nullptr;
	TextManager* _text = nullptr;


	EntityController* _controller = nullptr;
};

#endif