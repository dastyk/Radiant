#include "EntityBuilder.h"
#include "Utils.h"


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
	_controller = new EntityController(_entity, _mesh, _transform, _camera, _material, _overlay, _event, _light, _bounding, _text);
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
	if (values.size() == 0)
	{
		TraceDebug("Tried to create listselection with no values");
		std::vector<std::string> v;
		v.push_back("NaN");
		_listSelections.push_front(std::move(ListSelection(v, 0, updatefunc)));
	}
	else
		_listSelections.push_front(std::move(ListSelection(values, startValue, updatefunc)));
	ListSelection* l = &_listSelections.front();

	Entity e = _entity.Create();
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
		[this,text,l]()
	{
		l->value = (l->value > 0) ? l->value - 1 : 0;
		this->_text->ChangeText(text, l->values[l->value]);
		l->update();
	});
	_controller->BindEvent(bl, 
		EventManager::EventType::OnEnter, 
		[this, bl, textColor]()
	{
		XMFLOAT4 color = XMFLOAT4(textColor.x*1.8, textColor.y*1.8, textColor.z*1.8, textColor.w);
		this->_text->ChangeColor(bl, color);
	});
	_controller->BindEvent(bl,
		EventManager::EventType::OnExit,
		[this, bl, textColor]()
	{
		this->_text->ChangeColor(bl, textColor);
	});


	br = CreateButton(
		XMFLOAT3(size1 + size2, 5.0f, 0.0f),
		"->",
		textColor,
		50.0f,
		50.0f,
		"",
		[this, text,l]()
	{
		l->value = (l->value < l->values.size()-1) ? l->value + 1 : l->values.size()-1;
		this->_text->ChangeText(text, l->values[l->value]);	
		l->update();
	});
	_controller->BindEvent(br,
		EventManager::EventType::OnEnter,
		[this, br, textColor]()
	{
		XMFLOAT4 color = XMFLOAT4(textColor.x*1.8, textColor.y*1.8, textColor.z*1.8, textColor.w);
		this->_text->ChangeColor(br, color);
	});
	_controller->BindEvent(br,
		EventManager::EventType::OnExit,
		[this, br, textColor]()
	{
		this->_text->ChangeColor(br, textColor);
	});

	_transform->SetPosition(text, XMFLOAT3(size1+50.0f, 5.0f, 0.0f));

	_transform->BindChild(e, text);
	_transform->BindChild(e, bl);
	_transform->BindChild(e, br);
	_transform->BindChild(e, ename);
	_transform->SetPosition(e, position);

	return e;

}

const Entity & EntityBuilder::CreateOverlay(XMVECTOR & pos, float width, float height, const std::string& texture)
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