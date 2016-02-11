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
}



void OverlayManager::GatherOverlayJobs(std::function<void(OverlayData&)> ProvideJob)
{
	for (auto o : _overlays)
	{
		if(o.mat && o.visible)
			ProvideJob(o);
	}

}

const void OverlayManager::CreateOverlay(const Entity& entity)
{
	// Chech if entity already has an overlay.
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		TraceDebug("Tried to add overlay to enitiy that already had one.");
		return;
	}


	// Create new overlay and bind it to the entity.
	OverlayData data;
	data.height = 0;
	data.width = 0;
	data.posX = 0;
	data.posY = 0;
	data.mat = false;
	data.visible = true;
	_entityToIndex[entity] = static_cast<int>(_overlays.size());
	_overlays.push_back(move(data));

	if (_sendOverlayDataPointerCallback)
	{
		for (auto& i : _entityToIndex)
		{
			_sendOverlayDataPointerCallback(i.first, &_overlays[i.second]);
		}
	}
	if (_sendOverlayDataPointerCallback)
		

	return void();
}

const void OverlayManager::SetExtents(const Entity & entity, float width, float height)
{

	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		_overlays[indexIt->second].width = width;
		_overlays[indexIt->second].height = height;
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
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		_overlays[indexIt->second].visible = visible;
	}
}

void OverlayManager::_TransformChanged( const Entity& entity, const XMMATRIX& tran, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		_overlays[indexIt->second].posX = XMVectorGetX(pos);
		_overlays[indexIt->second].posY = XMVectorGetY(pos);
	}
	return void();
}

void OverlayManager::_MaterialChanged(const Entity & entity, const ShaderData* material)
{
	auto meshIt = _entityToIndex.find(entity);

	if (meshIt != _entityToIndex.end())
	{
		_overlays[meshIt->second].material = material;
		_overlays[meshIt->second].mat = true;
	}
}
