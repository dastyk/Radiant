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
