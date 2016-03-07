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
#include "Lightning.h"
#include "PopUpBox.h"
#include "DecalManager.h"
#include "AnimationManager.h"

struct ListSelection
{
	unsigned int value;
	std::vector<std::string> values;
	std::function<void()> update;
	ListSelection(const std::vector<std::string>& v, unsigned int s, std::function<void()> lam) :values(std::move(v)), value(s), update(std::move(lam))
	{}
};


struct ProgressBar
{
	float value;
	float minV;
	float maxV;
	float width;
	float height;
	ProgressBar(float v, float minV, float maxV, float width, float height):value(v), minV(minV), maxV(maxV),width(width),height(height)
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
	Slider(float minv, float maxv, float width, float height, float curr, bool real, std::function<void()> change) : minv(minv), maxv(maxv), width(width), height(height), curr(curr), real(real), change(std::move(change))
	{

	}
};

enum class ItemType : unsigned
{
	ListSelection = 1U << 0U,
	Slider = 1U << 1U
};

struct Item
{
	Entity e;
	ItemType t;

	Item(const Entity& entity, const ItemType& type) : e(entity), t(type)
	{

	}
};
struct ScrollList
{
	Entity scrollbar;
	std::vector<Item> items;
	float width;
	float height;
	float curr;
	float itemHeight;
	uint first;
	uint last;
	uint count;
	ScrollList(float width, float height, float itemHeight, std::vector<Item>& items) :width(width), height(height), itemHeight(itemHeight), items(std::move(items)), curr(0.0f), first(0), count((uint)(height / itemHeight)), scrollbar(Entity())
	{
		last = (count > 0) ? count - 1 : 0;
	}
};
enum class PopUpType : unsigned
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
	EntityController(EntityManager& e, StaticMeshManager* mesh , TransformManager* trans, CameraManager* cam, MaterialManager* mat, OverlayManager* o, EventManager* _event, LightManager* l, BoundingManager* b, TextManager* text, LightningManager* lightning, DecalManager* decal, AnimationManager* anim);
	~EntityController();

	const void ReleaseEntity(const Entity& entity);
	const void BindEventHandler(const Entity& entity, const EventManager::Type& type)const;
	const void BindEvent(const Entity& entity, const EventManager::EventType& type, std::function<void()> callback)const;
	const void ToggleVisible(const Entity& entity, bool visible)const;
	const void ToggleEventChecking(const Entity& entity, bool active)const;
	const std::string GetValue(const Entity& entity)const;
	const void SetProgressBarValue(const Entity& entity, const unsigned int value);
	const unsigned int GetListSelectionValue(const Entity& entity)const;
	const unsigned int GetProgressBarValue(const Entity& entity)const;
	const float GetSliderValue(const Entity& entity)const;
	const Item* GetScrollListItem(const Entity& entity, const uint& itemID)const;
	const void AddListSelection(const Entity& entity, ListSelection* listselection);
	const void AddProgressBar(const Entity& entity, ProgressBar* progbar);
	const void AddPopUpBox(const Entity& entity, PopUpBox* box);
	const void AddSlider(const Entity& entity, Slider* slider);
	const void AddScrollList(const Entity& entity, ScrollList* list);
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
	LightningManager* Lightning()const;
	DecalManager* Decal()const;
	AnimationManager* Animation()const;
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
	LightningManager* _lightning = nullptr;
	DecalManager* _decal = nullptr;
	AnimationManager* _animation;
	std::unordered_map <Entity, ListSelection*, EntityHasher> _listSelections;
	std::unordered_map <Entity, ProgressBar*, EntityHasher> _progressBars;
	std::unordered_map <Entity, PopUpBox*, EntityHasher> _popUps;
	std::unordered_map<Entity, ScrollList*, EntityHasher> _scrollLists;
	std::unordered_map<Entity, Slider*, EntityHasher> _sliders;
	PopInfo _popInfo;
	float _hoverColorInc = 1.8f;
};

#endif