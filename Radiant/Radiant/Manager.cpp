#include "Manager.h"

Entity & ManagerWrapper::CreateCamera(XMVECTOR & position) const
{
	Entity ent;

	transform->CreateTransform(ent);
	camera->CreateCamera(ent);
	camera->SetActivePerspective(ent);
	transform->SetPosition(ent, position);
	transform->SetFlyMode(ent, false);

	return ent;
}

Entity & ManagerWrapper::CreateObject(XMVECTOR & pos, XMVECTOR & rot, XMVECTOR & scale, std::string& meshtext, std::string& texture, std::string& normal) const
{
	Entity ent;

	transform->CreateTransform(ent);
	mesh->CreateStaticMesh(ent, meshtext.c_str());
	material->BindMaterial(ent, "Shaders/GBuffer.hlsl");
	material->SetMaterialProperty(ent, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	material->SetEntityTexture(ent, "DiffuseMap", S2WS(texture).c_str());
	material->SetEntityTexture(ent, "NormalMap", S2WS(normal).c_str());
	//bounding->CreateBoundingBox(ent);
	transform->SetPosition(ent, pos);
	transform->SetRotation(ent, rot);
	transform->SetScale(ent, scale);

	return ent;
}
