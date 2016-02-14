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
	_controller = new EntityController(_entity,_mesh, _transform, _camera, _material, _overlay, _event, _light, _bounding, _text);
}


EntityBuilder::~EntityBuilder()
{
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

}


const Entity & EntityBuilder::CreateImage(XMFLOAT3 & position, float width, float height, const std::string & texture)
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

const Entity & EntityBuilder::CreateLabel(XMFLOAT3 & position, const std::string & text, XMFLOAT4 & textColor, float width, float height, const std::string & texture)
{
	Entity ent = _entity.Create();
	
	_overlay->CreateOverlay(ent);
	_transform->CreateTransform(ent);
	_text->BindText(ent, text, "Assets/Fonts/cooper", 40, textColor);
	if (texture != "")
	{
		_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
		_material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	}
	_transform->SetPosition(ent, position);
	_overlay->SetExtents(ent, width, height);

	return ent;
}

const Entity & EntityBuilder::CreateButton(XMFLOAT3 & position, const std::string & text, XMFLOAT4& textColor, float width, float height, const std::string & texture, std::function<void()> callback)
{
	auto a = System::GetInstance()->GetAudio();
	Entity ent = _entity.Create();
	_overlay->CreateOverlay(ent);
	_transform->CreateTransform(ent);
	_text->BindText(ent, text, "Assets/Fonts/cooper", 40, textColor);
	if (texture != "")
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

const Entity & EntityBuilder::CreateCamera(XMVECTOR & position)
{
	Entity ent = _entity.Create();

	_transform->CreateTransform(ent);
	_camera->CreateCamera(ent);
	_camera->SetActivePerspective(ent);
	_transform->SetPosition(ent, position);
	_transform->SetFlyMode(ent, false);

	return ent;
}



const Entity & EntityBuilder::CreateObject(XMVECTOR & pos, XMVECTOR & rot, XMVECTOR & scale, const std::string& meshtext, const std::string& texture, const std::string& normal, const std::string& displacement)
{
	Entity ent = _entity.Create();

	_transform->CreateTransform(ent);
	_mesh->CreateStaticMesh(ent, meshtext.c_str());
	_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	_material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	_material->SetEntityTexture(ent, "NormalMap", S2WS(normal).c_str());
	_material->SetEntityTexture(ent, "DisplacementMap", S2WS(displacement).c_str());
	//bounding->CreateBoundingBox(ent);
	_transform->SetPosition(ent, pos);
	_transform->SetRotation(ent, rot);
	_transform->SetScale(ent, scale);

	return ent;
}

const Entity & EntityBuilder::CreateListSelection(const XMFLOAT3 & position, std::string& name, const std::vector<std::string>& values, const unsigned int startValue, float size1, float size2, std::function<void()> updatefunc, XMFLOAT4& textColor)
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
	Entity ename = _entity.Create();

	_transform->CreateTransform(ename);
	_transform->CreateTransform(e);
	_transform->CreateTransform(text);

	_text->BindText(ename, name, "Assets/Fonts/cooper", 40, textColor);

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
		this->_text->ChangeText(e, l->values[l->value]);
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
		l->value = (l->value < l->values.size() - 1) ? l->value + 1 : l->values.size() - 1;
		this->_text->ChangeText(text, l->values[l->value]);
		this->_text->ChangeText(e, l->values[l->value]);
		l->update();
	});


	_transform->SetPosition(text, XMFLOAT3(size1 + 50.0f, 5.0f, 0.0f));

	_transform->BindChild(e, text);
	_transform->BindChild(e, bl);
	_transform->BindChild(e, br);
	_transform->BindChild(e, ename);
	_transform->SetPosition(e, position);

	return e;

}

const Entity& EntityBuilder::CreateOverlay(XMFLOAT3& pos, float width, float height, const std::string& texture)
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


const Entity& EntityBuilder::CreatePopUp(PopUpType type, const std::string & text, std::function<void(unsigned int)> callback)
{
	PopUpBox* b = nullptr;
	try { b = new PopUpBox(type, text, callback); }
	catch (std::exception& e) { e;SAFE_DELETE(b); throw ErrorMsg(1500001, L"Could not create popupbox"); }
	Entity e = _entity.Create();

	_controller->AddPopUpBox(e, b);
	return e;
}

const Entity EntityBuilder::CreateSlider(XMFLOAT3& pos, float width, float height, float minv, float maxv, float defval, float size1, bool real, const std::string& text, float size2, std::function<void()> change, XMFLOAT4& textColor)
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

const Entity EntityBuilder::CreateScrollList(XMFLOAT3 & pos, float width, float height, float itemHeight, std::vector<Item>& items)
{
	return Entity();
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