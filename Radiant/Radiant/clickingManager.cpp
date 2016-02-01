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

const void ClickingManager::BindOverlay(Entity & entity)
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

	_entityToIndex[entity] = static_cast<int>(_cOverlays.size());
	_cOverlays.push_back(move(cData));
	return void();
}

const bool ClickingManager::IsClicked(Entity & entity)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		int posX, posY;
		System::GetInput()->GetMousePos(posX, posY);


		if (posX >= _cOverlays[indexIt->second].posX)
		{
			if (posY >= _cOverlays[indexIt->second].posY)
			{
				if (posX <= _cOverlays[indexIt->second].width + _cOverlays[indexIt->second].posX)
				{
					if (posY <= _cOverlays[indexIt->second].height + _cOverlays[indexIt->second].posY)
					{
						return true;
					}
				}
			}
		}
	}


	TraceDebug("Tried to click a entity with no clicking bound.");
	return false;
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
