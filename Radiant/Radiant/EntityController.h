#ifndef _ENTITYCONTROLLER_H_
#define _ENTITYCONTROLLER_H_

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
#include "PopUpBox.h"

struct ListSelection
{
	unsigned int value;
	std::vector<std::string> values;
	std::function<void()> update;
	ListSelection() {}
	ListSelection(const std::vector<std::string>& v, unsigned int s, std::function<void()> lam) :values(std::move(v)), value(s), update(std::move(lam))
	{}
};
struct Slider
{
	float minv;
	float maxv;
	float width;
	float height;
	float curr;
	bool real;
	std::function<void()> change;
	Entity sliderbar;
	Slider(float minv, float maxv, float width, float height, float curr, bool real, std::function<void()> change) : minv(minv), maxv(maxv), width(width), height(height), curr(curr), real(real), change(std::move(change))
	{

	}
};
struct ScrollList
{
	std::vector<Entity> items;
	Entity scrollbar;
	float width;
	float height;
	float itemHeight;
};
enum class PopUpType
{
	YESNO = 1 << 1,
	OK = 1 << 2
};

class PopUpBox;
class EntityController
{
public:
	struct PopInfo
	{
		Entity e;
		bool poping;
	};
public:
	EntityController(EntityManager& e, StaticMeshManager* mesh , TransformManager* trans, CameraManager* cam, MaterialManager* mat, OverlayManager* o, EventManager* _event, LightManager* l, BoundingManager* b, TextManager* text);
	~EntityController();

	const void ReleaseEntity(const Entity& entity);
	const void BindEventHandler(const Entity& entity, const EventManager::Type& type)const;
	const void BindEvent(const Entity& entity, const EventManager::EventType& type, std::function<void()> callback)const;
	const void ToggleVisible(const Entity& entity, bool visible)const;
	const void ToggleEventChecking(const Entity& entity, bool active)const;
	const std::string& GetValue(const Entity& entity)const;
	const unsigned int& GetListSelectionValue(const Entity& entity)const;
	const float& GetSliderValue(const Entity& entity)const;
	const void AddListSelection(const Entity& entity, ListSelection* listselection);
	const void AddPopUpBox(const Entity& entity, PopUpBox* box);
	const void AddSlider(const Entity& entity, Slider* slider);
	const void ShowPopupBox(const Entity& entity);
	const void Update()const;

	const void SetExclusiveRenderAccess()const;
	const void UnbindFromRenderer()const;

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

	std::unordered_map <Entity, ListSelection*, EntityHasher> _listSelections;
	std::unordered_map <Entity, PopUpBox*, EntityHasher> _popUps;
	std::unordered_map<Entity, ScrollList*, EntityHasher> _scollLists;
	std::unordered_map<Entity, Slider*, EntityHasher> _sliders;
	PopInfo _popInfo;
	float _hoverColorInc = 1.8f;
};

#endif