#include "EntityBuilder.h"
#include "Utils.h"
#include "System.h"

EntityBuilder::EntityBuilder()
{
	_transform = new TransformManager();
	_material = new MaterialManager();
	_mesh = new StaticMeshManager(*_transform, *_material);
	_overlay = new OverlayManager(*_transform, *_material);
	_camera = new CameraManager(*_transform);
	_event = new EventManager(*_overlay);
	_light = new LightManager(*_transform);
	_bounding = new BoundingManager(*_transform);
	_text = new TextManager(*_transform);
	_lightning = new LightningManager( *_transform, *_material );
	_decal = new DecalManager(*_material, *_transform);
	_animation = new AnimationManager;
	_proximityLightning = new ProximityLightningManager( *_transform, *_lightning );
	_controller = new EntityController( _entity, _mesh, _transform, _camera, _material, _overlay, _event, _light, _bounding, _text, _lightning, _decal, _animation, _proximityLightning );
}


EntityBuilder::~EntityBuilder()
{
	SAFE_DELETE( _lightning );
	SAFE_DELETE(_text);
	SAFE_DELETE(_bounding);
	SAFE_DELETE(_event);
	SAFE_DELETE(_camera);
	SAFE_DELETE(_mesh);
	SAFE_DELETE(_overlay);
	SAFE_DELETE(_transform);
	SAFE_DELETE(_material);
	SAFE_DELETE(_light);
	SAFE_DELETE(_controller);
	SAFE_DELETE(_decal);
	SAFE_DELETE(_animation);
	SAFE_DELETE( _proximityLightning );
}


const Entity EntityBuilder::CreateImage(const XMFLOAT3 & position, float width, float height, const std::string & texture)
{
	Entity ent = _entity.Create();
	_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	_overlay->CreateOverlay(ent);
	_transform->CreateTransform(ent);
	_material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	_transform->SetPosition(ent, position);
	_overlay->SetExtents(ent, width, height);

	return ent;
}

const Entity EntityBuilder::CreateLabel(const XMFLOAT3 & position, const std::string & text, const XMFLOAT4 & textColor, float width, float height, const std::string & texture)
{
	Entity ent = _entity.Create();
	_transform->CreateTransform(ent);
	_text->BindText(ent, text, "Assets/Fonts/cooper", 40, textColor);
	if (!texture.empty())
	{
		_overlay->CreateOverlay(ent);
		_overlay->SetExtents(ent, width, height);

		_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
		_material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	}
	_transform->SetPosition(ent, position);


	return ent;
}

const Entity EntityBuilder::CreateButton(const XMFLOAT3 & position, const std::string & text, const XMFLOAT4& textColor, float width, float height, const std::string & texture, std::function<void()> callback)
{
	auto a = System::GetInstance()->GetAudio();
	Entity ent = _entity.Create();
	_overlay->CreateOverlay(ent);
	_transform->CreateTransform(ent);
	_text->BindText(ent, text, "Assets/Fonts/cooper", 40, textColor);
	if (!texture.empty())
	{
		_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
		_material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	}
	_event->BindEvent(ent, EventManager::EventType::LeftClick, callback);
	_transform->SetPosition(ent, position);
	_overlay->SetExtents(ent, width, height);

	_controller->BindEvent(ent,
		EventManager::EventType::OnEnter,
		[ent, this, a, textColor]()
	{
		this->_text->ChangeColor(ent, XMFLOAT4(textColor.x*this->_hoverColorInc, textColor.y*this->_hoverColorInc, textColor.z*this->_hoverColorInc, 1.0f));
		a->PlaySoundEffect(L"menuhover.wav", 1);
	});
	_controller->BindEvent(ent,
		EventManager::EventType::OnExit,
		[ent, this, textColor]()
	{
		this->_text->ChangeColor(ent, textColor);
	});

	return ent;
}

const Entity EntityBuilder::CreateCamera(const XMVECTOR & position)
{
	Entity ent = _entity.Create();

	_transform->CreateTransform(ent);
	_camera->CreateCamera(ent);
	_camera->SetActivePerspective(ent);
	_transform->SetPosition(ent, position);
	_transform->SetFlyMode(ent, false);

	return ent;
}

const Entity EntityBuilder::CreateDecal(const XMFLOAT3 & pos, const XMFLOAT3 & rot, const XMFLOAT3 & scale, const std::string & colorTex, const std::string& normalTex, const std::string& emissiveTex)
{
	Entity ent = _entity.Create();
	_decal->BindDecal(ent);
	_transform->SetScale(ent, XMVectorSet(scale.x, scale.y, scale.z, 0.0f));
	_transform->SetRotation(ent, XMVectorSet(rot.x, rot.y, rot.z, 0.0f));
	_transform->SetPosition(ent, XMVectorSet(pos.x, pos.y, pos.z, 1.0f));

	std::vector<string> pro;
	pro.push_back("gColor");
	pro.push_back("gNormal");
	pro.push_back("gEmissive");


	std::vector<wstring> texs;
	texs.push_back(S2WS(colorTex));
	texs.push_back(S2WS(normalTex));
	texs.push_back(S2WS(emissiveTex));

	_material->SetEntityTexture(ent, pro, texs);

	return ent;
}

const Entity EntityBuilder::CreateHealingLight(const XMFLOAT3 & pos, const XMFLOAT3 & rot, const DirectX::XMFLOAT3 & color, float intensity, float outerAngle, float innerAngle, float range)
{
	Entity ent = _entity.Create();
	_transform->CreateTransform(ent);
	_light->BindSpotLight(ent, color, intensity, outerAngle, innerAngle, range);
	_light->SetAsVolumetric(ent, true);
	_transform->SetPosition(ent, pos);
	_transform->SetRotation(ent, rot);

	float s = range*atanf(outerAngle);
	_transform->SetScale(ent, XMFLOAT3(s, s, range));

	return ent;
}

const Entity EntityBuilder::CreateProgressBar(const XMFLOAT3 & position, const std::string & text, float textSize, const XMFLOAT4 & colorTex, const float min, const float max, const float start, float width, float height)
{
	ProgressBar* b = nullptr;
	try {
		b = new ProgressBar(start, min, max, width, height);
	}
	catch (std::exception& e) { e; throw ErrorMsg(0, L"Failed to create progress bar."); }
	Entity ent = CreateLabel(position, text, colorTex, textSize, height, "");


	float l = width*((b->value - b->minV) / (b->maxV - b->minV));
	Entity bar = CreateOverlay(XMFLOAT3(textSize, 0.0f, 0.0f), l, height, "Assets/Textures/default_color.png");
	
	_transform->BindChild(ent, bar);

	_transform->SetPosition(ent, position);

	_animation->CreateAnimation(ent, "scale", 0.15f, 
		[bar,b,this](float delta, float amount, float offset) 
	{
		_overlay->SetExtents(bar, offset + amount, b->height);
	});


	_controller->AddProgressBar(ent, b);
	return ent;
}

const Entity EntityBuilder::CreateObject(const XMVECTOR & pos, const XMVECTOR & rot, const XMVECTOR & scale, const std::string& meshtext, const std::string& texture, const std::string& normal, const std::string& displacement, const std::string& roughness)
{
	Entity ent = _entity.Create();

	_transform->CreateTransform(ent);
	_mesh->CreateStaticMesh(ent, meshtext.c_str());
	_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	
	std::vector<string> pro;
	pro.push_back("DiffuseMap");
	pro.push_back("NormalMap");
	pro.push_back("DisplacementMap");
	pro.push_back("Roughness");

	std::vector<wstring> texs;
	texs.push_back(S2WS(texture));
	texs.push_back(S2WS(normal));
	texs.push_back(S2WS(displacement));
	texs.push_back(S2WS(roughness));

	_material->SetEntityTexture(ent,pro, texs);

	_material->SetMaterialProperty(ent, 0, "Metallic", 0.0f, "Shaders/GBuffer.hlsl");
	_material->SetMaterialProperty(ent, "TexCoordScaleU", 1.0f, "Shaders/GBuffer.hlsl");
	_material->SetMaterialProperty(ent, "TexCoordScaleV", 1.0f, "Shaders/GBuffer.hlsl");

	_transform->SetPosition(ent, pos);
	_transform->SetRotation(ent, rot);
	_transform->SetScale(ent, scale);

	return ent;
}

const Entity EntityBuilder::CreateObjectWithEmissive(const XMVECTOR & pos, const  XMVECTOR & rot, const XMVECTOR & scale, const std::string & meshtext, const std::string & texture, const std::string & normal, const std::string & displacement, const std::string& roughness, const std::string & emissive)
{
	Entity ent = _entity.Create();

	_transform->CreateTransform(ent);
	_mesh->CreateStaticMesh(ent, meshtext.c_str());
	_material->BindMaterial(ent, "Shaders/GBufferEmissive.hlsl");

	std::vector<string> pro;
	pro.push_back("DiffuseMap");
	pro.push_back("NormalMap");
	pro.push_back("DisplacementMap");
	pro.push_back("Roughness");
	pro.push_back("Emissive");

	std::vector<wstring> texs;
	texs.push_back(S2WS(texture));
	texs.push_back(S2WS(normal));
	texs.push_back(S2WS(displacement));
	texs.push_back(S2WS(roughness));
	texs.push_back(S2WS(emissive));

	_material->SetEntityTexture(ent, pro, texs);

	_transform->SetPosition(ent, pos);
	_transform->SetRotation(ent, rot);
	_transform->SetScale(ent, scale);

	return ent;
}

const Entity EntityBuilder::CreateListSelection(const XMFLOAT3 & position, std::string& name, const std::vector<std::string>& values, const unsigned int startValue, float size1, float size2, std::function<void()> updatefunc, const  XMFLOAT4& textColor)
{
	ListSelection* l = nullptr;

	Entity e = _entity.Create();
	try {
		if (values.size() == 0)
		{
			TraceDebug("Tried to create listselection with no values");
			std::vector<std::string> v;
			v.push_back("NaN");
			l = new ListSelection(v, 0, updatefunc);
		}
		else
			l = new ListSelection(values, startValue, updatefunc);
	}
	catch (std::exception& e) { e; throw ErrorMsg(1500002, L"Could not create listselection"); }
	_controller->AddListSelection(e, l);
	Entity text = _entity.Create();
	Entity bl;
	Entity br;
	//Entity ename = _entity.Create();

	//_transform->CreateTransform(ename);
	_transform->CreateTransform(e);
	_transform->CreateTransform(text);

	_text->BindText(e, name, "Assets/Fonts/cooper", 40, textColor);

	if (l->value >= l->values.size())
	{
		l->value = 0;
		TraceDebug("Tried to set default value out of range.");
	}

	_text->BindText(text, l->values[l->value], "Assets/Fonts/cooper", 40, textColor);

	bl = CreateButton(
		XMFLOAT3(size1, 5.0f, 0.0f),
		"<-",
		textColor,
		50.0f,
		50.0f,
		"",
		[this, text, l, e]()
	{
		l->value = (l->value > 0) ? l->value - 1 : 0;
		this->_text->ChangeText(text, l->values[l->value]);
	//	this->_text->ChangeText(e, l->values[l->value]);
		l->update();
	});


	br = CreateButton(
		XMFLOAT3(size1 + size2, 5.0f, 0.0f),
		"->",
		textColor,
		50.0f,
		50.0f,
		"",
		[this, text, l, e]()
	{
		l->value = static_cast<unsigned int>((l->value < l->values.size() - 1) ? l->value + 1 : l->values.size() - 1);
		this->_text->ChangeText(text, l->values[l->value]);
		//this->_text->ChangeText(e, l->values[l->value]);
		l->update();
	});


	_transform->SetPosition(text, XMFLOAT3(size1 + 50.0f, 5.0f, 0.0f));

	_transform->BindChild(e, text);
	_transform->BindChild(e, bl);
	_transform->BindChild(e, br);
//	_transform->BindChild(e, ename);
	_transform->SetPosition(e, position);

	return e;

}

const Entity EntityBuilder::CreateOverlay(const XMFLOAT3& pos, float width, float height, const std::string& texture)
{
	Entity ent = _entity.Create();
	_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	_overlay->CreateOverlay(ent);
	_transform->CreateTransform(ent);
	_material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	_overlay->SetExtents(ent, width, height);
	_transform->SetPosition(ent, pos);


	return ent;
}


const Entity EntityBuilder::CreatePopUp(PopUpType type, const std::string & text, std::function<void(unsigned int)> callback)
{
	PopUpBox* b = nullptr;
	try { b = new PopUpBox(type, text, callback); }
	catch (std::exception& e) { e;SAFE_DELETE(b); throw ErrorMsg(1500001, L"Could not create popupbox"); }
	Entity e = _entity.Create();

	_controller->AddPopUpBox(e, b);
	return e;
}

const Entity EntityBuilder::CreateSlider(const XMFLOAT3& pos, float width, float height, float minv, float maxv, float defval, float size1, bool real, const std::string& text, float size2, std::function<void()> change, const  XMFLOAT4& textColor)
{
	Slider* s = nullptr;
	try { s = new Slider(minv,maxv,width, height,defval, real, std::move(change)); }
	catch (std::exception& e) { e;SAFE_DELETE(s); throw ErrorMsg(1500003, L"Could not create slider"); }

	Entity ent = _entity.Create();
	_transform->CreateTransform(ent);
	Entity rail = CreateImage(XMFLOAT3(size2, height / 2.0f - height / 16.0f, 0.0f), width+ height / 4.0f, height / 8.0f, "Assets/Textures/default_color.png");
	Entity slidebar = CreateImage(XMFLOAT3(size2+width*((defval-minv)/(maxv-minv)), height/2.0f- height/4.0f, 0.0f), height / 2.0f, height / 2.0f, "Assets/Textures/default_color.png");
	Entity la = CreateLabel(XMFLOAT3(0.0f,0.0f,0.0f), text, textColor, size2, height, "");
	Entity vtext = CreateLabel(XMFLOAT3(width + size2 + height / 2.0f, 0.0f, 0.0f), (real) ? to_string((double)defval) : to_string((int)defval), textColor, size1, height, "");
	
	_transform->BindChild(ent, slidebar);
	_transform->BindChild(ent, la);
	_transform->BindChild(ent, vtext);
	_transform->BindChild(ent, rail);

	_transform->SetPosition(ent, pos);

	_controller->AddSlider(ent, s);

	auto i = System::GetInput();
	_event->BindEvent(slidebar, EventManager::EventType::Drag,
		[s,this, slidebar,i,size2,vtext]() 
	{
		int x, y;
		i->GetMouseDiff(x, y);
		float currp = s->width*((s->curr - s->minv) / (s->maxv - s->minv));
		currp += x;
		s->curr = (currp / s->width)*(s->maxv - s->minv) + s->minv;
		if (s->curr >= s->maxv)
			s->curr = s->maxv;
		else if (s->curr <= s->minv)
			s->curr = s->minv;
		else
		{
			this->_transform->SetPosition(slidebar, XMFLOAT3(size2 + currp, s->height / 2.0f - s->height / 4.0f, 0.0f));
			
			
		}
		s->change();
		this->_text->ChangeText(vtext, (s->real) ? to_string((double)s->curr) : to_string((int)s->curr));
	});

	return ent;

}

const Entity EntityBuilder::CreateScrollList(const XMFLOAT3 & pos, float width, float height, float itemHeight, std::vector<Item>& items)
{
	ScrollList* l = nullptr;
	try { l = new ScrollList(width, height, itemHeight, items); }
	catch (std::exception& e) { e;SAFE_DELETE(l); throw ErrorMsg(1500004, L"Could not create scrolllist"); }


	Entity list;

	_transform->CreateTransform(list);
	_overlay->CreateOverlay(list);
	_overlay->SetExtents(list, width, height);
	for (auto& i : l->items)
	{
		_transform->BindChild(list, i.e);
		_controller->ToggleVisible(i.e, false);
		_controller->ToggleEventChecking(i.e, false);
	}
	for (uint i = l->first; i < l->count; i++)
	{
		auto& it = l->items[i];
		_transform->SetPosition(it.e, XMFLOAT3(0.0f, i*l->itemHeight, 0.0f));
		_controller->ToggleVisible(it.e, true);
		_controller->ToggleEventChecking(it.e, true);
	}


	l->scrollbar = CreateImage(XMFLOAT3(width, 0.0f,0.0f),50.0f, 50.0f, "Assets/Textures/default_color.png");

	_transform->BindChild(list, l->scrollbar);

	//auto i = System::GetInput();
	//_event->BindEventToEntity(l->scrollbar, EventManager::Type::Overlay);
	//_event->BindEvent(l->scrollbar, EventManager::EventType::Drag, 
	//	[l, this, i]()
	//{
	//	int x, y;
	//	i->GetMouseDiff(x, y);
	//	l->curr += (float)x;
	//	if (l->curr >= l->height)
	//		l->curr = l->height;
	//	else if (l->curr <= 0)
	//		l->curr = 0;

	//	this->_transform->SetPosition(l->scrollbar, XMFLOAT3(l->width, l->curr, 0.0f));

	//	uint newp = (uint)(l->height / l->curr);
	//	if (l->first < newp)
	//	{
	//		uint diff = newp - l->first;
	//		for (uint i = 0; i < diff; i++)
	//		{
	//			_controller->ToggleVisible(l->items[l->first+i].e, false);
	//			_controller->ToggleEventChecking(l->items[l->first+i].e, false);

	//			_controller->ToggleVisible(l->items[l->last+i+1].e, true);
	//			_controller->ToggleEventChecking(l->items[l->last+i+1].e, true);
	//		}
	//

	//		l->first += diff;
	//		l->last += diff;

	//		for (uint i = l->first; i < l->count; i++)
	//		{
	//			_transform->SetPosition(l->items[i].e, XMFLOAT3(0.0f, i*l->itemHeight, 0.0f));
	//		}
	//	}
	//	else if (l->first > newp)
	//	{
	//		uint diff = l->first - newp;
	//		for (uint i = 0; i < diff; i++)
	//		{
	//			_controller->ToggleVisible(l->items[newp + i].e, true);
	//			_controller->ToggleEventChecking(l->items[newp + i].e, true);

	//			_controller->ToggleVisible(l->items[l->last + i].e, false);
	//			_controller->ToggleEventChecking(l->items[l->last + i].e, false);
	//		}


	//		l->first -= diff;
	//		l->last -= diff;

	//		for (uint i = l->first; i < l->count; i++)
	//		{
	//			_transform->SetPosition(l->items[i].e, XMFLOAT3(0.0f, i*l->itemHeight, 0.0f));
	//		}
	//	}


	//});




	_transform->SetPosition(list, pos);

	return list;
}




EntityController * EntityBuilder::GetEntityController()
{
	return _controller;
}

EntityManager& EntityBuilder::EntityC()
{
	return _entity;
}
StaticMeshManager* EntityBuilder::Mesh()const
{
	return _mesh;
}
TransformManager* EntityBuilder::Transform()const
{
	return _transform;
}
CameraManager* EntityBuilder::Camera()const
{
	return _camera;
}
MaterialManager* EntityBuilder::Material()const
{
	return _material;
}
OverlayManager* EntityBuilder::Overlay()const
{
	return _overlay;
}
EventManager* EntityBuilder::Event()const
{
	return _event;
}
LightManager* EntityBuilder::Light()const
{
	return _light;
}
BoundingManager* EntityBuilder::Bounding()const
{
	return _bounding;
}
TextManager* EntityBuilder::Text()const
{
	return _text;
}
LightningManager* EntityBuilder::Lightning()const
{
	return _lightning;
}
DecalManager * EntityBuilder::Decal() const
{
	return _decal;
}

AnimationManager * EntityBuilder::Animation() const
{
	return _animation;
}

ProximityLightningManager * EntityBuilder::ProximityLightning() const
{
	return _proximityLightning;
}
