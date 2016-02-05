#include "EntityBuilder.h"
#include "Utils.h"


EntityBuilder::EntityBuilder()
{
	_transform = new TransformManager();
	_material = new MaterialManager();
	_mesh = new StaticMeshManager(*_transform, *_material);
	_overlay = new OverlayManager(*_transform, *_material);
	_camera = new CameraManager(*_transform);
	_clicking = new ClickingManager(*_transform, *_overlay);
	_light = new LightManager(*_transform);
	_bounding = new BoundingManager(*_transform);
	_text = new TextManager(*_transform);
	_controller = new EntityController;
}


EntityBuilder::~EntityBuilder()
{
	SAFE_DELETE(_text);
	SAFE_DELETE(_bounding);
	SAFE_DELETE(_clicking);
	SAFE_DELETE(_camera);
	SAFE_DELETE(_mesh);
	SAFE_DELETE(_overlay);
	SAFE_DELETE(_transform);
	SAFE_DELETE(_material);
	SAFE_DELETE(_light);
	SAFE_DELETE(_controller);
}


const Entity & EntityBuilder::CreateButton(XMVECTOR & position, const std::string & text, float width, float height, const std::string & texture, std::function<void()> callback)
{
	Entity ent = _entity.Create();
	_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	_overlay->CreateOverlay(ent);
	_transform->CreateTransform(ent);
	_material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	_transform->SetPosition(ent, position);
	_overlay->SetExtents(ent, width, height);
	_clicking->BindOverlay(ent, callback);
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

const Entity & EntityBuilder::CreateObject(XMVECTOR & pos, XMVECTOR & rot, XMVECTOR & scale, const std::string& meshtext, const std::string& texture, const std::string& normal)
{
	Entity ent = _entity.Create();

	_transform->CreateTransform(ent);
	_mesh->CreateStaticMesh(ent, meshtext.c_str());
	_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	_material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	_material->SetEntityTexture(ent, "NormalMap", S2WS(normal).c_str());
	//bounding->CreateBoundingBox(ent);
	_transform->SetPosition(ent, pos);
	_transform->SetRotation(ent, rot);
	_transform->SetScale(ent, scale);

	return ent;
}

const Entity & EntityBuilder::CreateOverlay(XMVECTOR & pos, float width, float height, const std::string& texture)
{
	Entity ent = _entity.Create();
	_material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	_overlay->CreateOverlay(ent);
	_transform->CreateTransform(ent);
	_material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	_transform->SetPosition(ent, pos);
	_overlay->SetExtents(ent, width, height);

	return ent;
}

EntityController * EntityBuilder::GetEntityController()
{
	return _controller;
}
