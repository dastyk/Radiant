#include "OverlayManager.h"
#include "System.h"

using namespace DirectX;

OverlayManager::OverlayManager(TransformManager& transformManager, MaterialManager& materialManager) 
{
	// Add the manager to the graphics
	System::GetGraphics()->AddOverlayProvider(this);

	// Set the callback functions
	transformManager.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<OverlayManager, &OverlayManager::_TransformChanged>( this );

	materialManager.SetMaterialChangeCallback2([this](Entity entity, ShaderData* material)
	{
		_MaterialChanged(entity, material);
	});
}


OverlayManager::~OverlayManager()
{
	for (auto& o : _entityToOverlay)
		SAFE_DELETE(o.second);
}



void OverlayManager::GatherOverlayJobs(std::function<void(OverlayData*)> ProvideJob)
{
	for (auto& o : _entityToOverlay)
	{
		if(o.second->mat && o.second->visible)
			ProvideJob(o.second);
	}

}

const void OverlayManager::CreateOverlay(const Entity& entity)
{
	// Chech if entity already has an overlay.
	auto indexIt = _entityToOverlay.find(entity);
	if (indexIt != _entityToOverlay.end())
	{
		TraceDebug("Tried to add overlay to enitiy that already had one.");
		return;
	}


	// Create new overlay and bind it to the entity.
	OverlayData* data = nullptr;
	try { data = new OverlayData; }
	catch (std::exception& e) { e; SAFE_DELETE(data); throw ErrorMsg(1600001, L"Failed to create overlay."); }
	data->height = 0;
	data->width = 0;
	data->posX = 0;
	data->posY = 0;
	data->mat = false;
	data->visible = true;
	_entityToOverlay[entity] = data;

	if (_sendOverlayDataPointerCallback)
			_sendOverlayDataPointerCallback(entity, data);

	
		

	return void();
}

const void OverlayManager::SetExtents(const Entity & entity, float width, float height)
{

	auto indexIt = _entityToOverlay.find(entity);
	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->width = width;
		indexIt->second->height = height;
	}
	return void();
}

const void OverlayManager::BindToRenderer(bool exclusive)
{
	if (exclusive)
		System::GetGraphics()->ClearOverlayProviders();
	// Add the manager to the renderer.
	System::GetGraphics()->AddOverlayProvider(this);
}

const void OverlayManager::ToggleVisible(const Entity & entity, bool visible)
{
	auto indexIt = _entityToOverlay.find(entity);
	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->visible = visible;
	}
}

void OverlayManager::_TransformChanged( const Entity& entity, const XMMATRIX& tran, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	auto indexIt = _entityToOverlay.find(entity);
	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->posX = XMVectorGetX(pos);
		indexIt->second->posY = XMVectorGetY(pos);
	}
	return void();
}

void OverlayManager::_MaterialChanged(const Entity & entity, const ShaderData* material)
{
	auto indexIt = _entityToOverlay.find(entity);

	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->material = material;
		indexIt->second->mat = true;
	}
}
