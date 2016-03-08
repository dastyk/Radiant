#ifndef _ENTITYBUILDER_H_
#define  _ENTITYBUILDER_H_

#pragma once
#include "EntityManager.h"
#include "StaticMeshManager.h"
#include "TransformManager.h"
#include "CameraManager.h"
#include "OverlayManager.h"
#include "EventManager.h"
#include "LightManager.h"
#include "BoundingManager.h"
#include "TextManager.h"
#include "Lightning.h"
#include "DecalManager.h"
#include "AnimationManager.h"

#include <functional>
class EntityController;
enum class PopUpType : unsigned;
struct Item;
class EntityBuilder
{

public:
	EntityBuilder();
	virtual ~EntityBuilder();

	const Entity CreateImage(const XMFLOAT3& position, float width, float height, const std::string& texture);
	const Entity CreateLabel(const XMFLOAT3& position, const std::string& text, const  XMFLOAT4& textColor, float width, float height, const std::string& texture);
	const Entity CreateButton(const XMFLOAT3& position,const std::string& text, const XMFLOAT4& textColor, float width, float height, const std::string& texture, std::function<void()> callback);
	const Entity CreateCamera(const XMVECTOR& position);
	const Entity CreateObject(const XMVECTOR & pos, const XMVECTOR & rot, const XMVECTOR & scale, const std::string& meshtext,
		const std::string& texture = "Assets/Textures/default_color.png", 
		const std::string& normal = "Assets/Textures/default_normal.png", 
		const std::string& displacement = "Assets/Textures/default_displacement.png",
		const std::string& roughness = "Assets/Textures/Floor_Roughness.png");
	const Entity CreateObjectWithEmissive(const XMVECTOR & pos, const XMVECTOR & rot, const XMVECTOR & scale, const std::string& meshtext,
		const std::string& texture = "Assets/Textures/default_color.png",
		const std::string& normal = "Assets/Textures/default_normal.png", 
		const std::string& displacement = "Assets/Textures/default_displacement.png", 
		const std::string& roughness = "Assets/Textures/Floor_Roughness.png",
		const std::string& emissive = "Assets/Textures/default_color.png");
	const Entity CreateListSelection(const XMFLOAT3& position, std::string& text, const std::vector<std::string>& values, const unsigned int startValue, float size1, float size2, std::function<void()> updatefunc, const  XMFLOAT4& textColor = XMFLOAT4(0.1f, 0.3f,0.6f,1.0f));
	const Entity CreateOverlay(const XMFLOAT3& pos, float width, float height, const std::string& texture);
	const Entity CreatePopUp(const PopUpType type, const std::string& text, std::function<void(unsigned int)> callback);
	const Entity CreateSlider(const XMFLOAT3& pos, float width, float height, float minv, float maxv, float defval, float size1, bool real, const std::string& text, float size2, std::function<void()> change, const XMFLOAT4& textColor = XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f));
	const Entity CreateScrollList(const XMFLOAT3& pos, float width, float height, float itemHeight, std::vector<Item>& items);
	const Entity CreateDecal(const XMFLOAT3& pos, const XMFLOAT3& rot, const XMFLOAT3& scale, const std::string& colorTex = "Assets/Textures/default_color.png", const std::string& normalTex = "Assets/Textures/default_normal.png", const std::string& emissiveTex = "Assets/Textures/allzero.png");
	const Entity CreateHealingLight(const XMFLOAT3& pos, const XMFLOAT3& rot, const DirectX::XMFLOAT3& color, float intensity, float outerAngle, float innerAngle, float range);
	const Entity CreateProgressBar(const XMFLOAT3& position,const std::string& text, float textSize, const XMFLOAT4 & colorTex, const float min, const float max, const float start, float width, float height);
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
	LightningManager* Lightning()const;
	DecalManager* Decal()const;
	AnimationManager* Animation()const;

protected:
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
	LightningManager* _lightning = nullptr;
	DecalManager* _decal = nullptr;
	AnimationManager* _animation;

	EntityController* _controller = nullptr;	
	float _hoverColorInc = 1.8f;
};
#include "EntityController.h"

#endif