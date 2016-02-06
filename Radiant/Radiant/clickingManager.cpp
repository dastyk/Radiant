#include "ClickingManager.h"

#include "System.h"

ClickingManager::ClickingManager(TransformManager& trans, OverlayManager& overlay)
{
	// Set the callback functions
	trans.SetTransformChangeCallback4([this](const Entity& entity, const DirectX::XMVECTOR & pos)
	{
		_TransformChanged(entity, pos);
	});
	overlay.SetExtentsChangeCallback([this](const Entity& entity, const float width, const float height)
	{
		_ExtentChanged(entity, width, height);
	});
}

ClickingManager::~ClickingManager()
{
}

const void ClickingManager::BindOverlay(const Entity& entity, std::function<void()> callback)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		TraceDebug("Tried to bind clicking component to entity that already had one.");
		return;
	}

	ClickableOverlay cData;
	cData.posX = 0;
	cData.posY = 0;
	cData.height = 0;
	cData.width = 0;
	cData.callback = std::move(callback);
	_entityToIndex[entity] = static_cast<int>(_cOverlays.size());
	_cOverlays.push_back(move(cData));
	return void();
}

const void ClickingManager::DoClick() const
{
	for_each(_cOverlays.begin(), _cOverlays.end(), [this](const ClickableOverlay& o) { _IsClicked(o);});
}

//const bool ClickingManager::IsClicked(Entity & entity)
//{
//	auto indexIt = _entityToIndex.find(entity);
//
//	if (indexIt != _entityToIndex.end())
//	{
//		int posX, posY;
//		System::GetInput()->GetMousePos(posX, posY);
//
//
//		if (posX >= _cOverlays[indexIt->second].posX)
//		{
//			if (posY >= _cOverlays[indexIt->second].posY)
//			{
//				if (posX <= _cOverlays[indexIt->second].width + _cOverlays[indexIt->second].posX)
//				{
//					if (posY <= _cOverlays[indexIt->second].height + _cOverlays[indexIt->second].posY)
//					{
//						return true;
//					}
//				}
//			}
//		}
//	}
//
//
//	TraceDebug("Tried to click a entity with no clicking bound.");
//	return false;
//}

const void ClickingManager::_IsClicked(const ClickableOverlay & overlay) const
{
	auto i = System::GetInput();
	int posX, posY;



	i->GetMousePos(posX, posY);


	if (posX >= overlay.posX)
	{
		if (posY >= overlay.posY)
		{
			if (posX <= overlay.width + overlay.posX)
			{
				if (posY <= overlay.height + overlay.posY)
				{
					overlay.callback();
				}
			}
		}
	}

	return void();
}

const void ClickingManager::_TransformChanged(const Entity & entity, const DirectX::XMVECTOR & pos)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_cOverlays[indexIt->second].posX = DirectX::XMVectorGetX(pos);
		_cOverlays[indexIt->second].posY = DirectX::XMVectorGetY(pos);
	}


	return void();
}

const void ClickingManager::_ExtentChanged(const Entity & entity, const float width, const float height)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_cOverlays[indexIt->second].width = width;
		_cOverlays[indexIt->second].height = height;
	}


	return void();
}
