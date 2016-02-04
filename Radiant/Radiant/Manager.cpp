#include "Manager.h"

Entity ManagerWrapper::CreateCamera(XMVECTOR & position)
{
	Entity ent = entity.Create();

	transform->CreateTransform(ent);
	camera->CreateCamera(ent);
	camera->SetActivePerspective(ent);
	transform->SetPosition(ent, position);
	transform->SetFlyMode(ent, false);

	return ent;
}

Entity ManagerWrapper::CreateObject(XMVECTOR & pos, XMVECTOR & rot, XMVECTOR & scale, std::string meshtext, std::string texture, std::string normal) 
{
	Entity ent = entity.Create();

	transform->CreateTransform(ent);
	mesh->CreateStaticMesh(ent, meshtext.c_str());
	material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	material->SetEntityTexture(ent, "NormalMap", S2WS(normal).c_str());
	//bounding->CreateBoundingBox(ent);
	transform->SetPosition(ent, pos);
	transform->SetRotation(ent, rot);
	transform->SetScale(ent, scale);

	return ent;
}

Entity ManagerWrapper::CreateOverlay(XMVECTOR & pos, float width, float height, std::string texture)
{
	Entity ent = entity.Create();
	material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	overlay->CreateOverlay(ent);
	transform->CreateTransform(ent);
	material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	transform->SetPosition(ent, pos);
	overlay->SetExtents(ent, width, height);

	return ent;
}

const void ManagerWrapper::UnbindFromRenderer()
{
	overlay->BindToRenderer(true);
	camera->BindToRenderer(true);
	mesh->BindToRendered(true);
	light->BindToRenderer(true);
}