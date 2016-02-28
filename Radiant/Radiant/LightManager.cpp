#include "LightManager.h"
#include "System.h"

using namespace DirectX;

LightManager::LightManager(TransformManager& transformManager) : _graphics(*System::GetGraphics())
{
	_graphics.AddLightProvider(this);

	transformManager.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<LightManager, &LightManager::_TransformChanged>( this );
}

LightManager::~LightManager()
{

}

void LightManager::BindPointLight(Entity entity, const DirectX::XMFLOAT3& pos, float range,
	const DirectX::XMFLOAT3& color, float intensity)
{
	_entityToPointLight[entity] = std::move(PointLight(pos, range, color, intensity, true));
}

void LightManager::BindSpotLight(Entity entity, const DirectX::XMFLOAT3 & color, float intensity, float outerAngle, float innerAngle, float range)
{
	_entityToSpotLight[entity] = SpotLight();
	SpotLight& s = _entityToSpotLight[entity];
	s.PositionVS = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	s.DirectionVS = XMFLOAT3( 0.0f, 0.0f, 1.0f );
	s.Color = color;
	s.Intensity = intensity;
	s.CosOuter = cosf( 0.5f * outerAngle );
	s.CosInner = cosf( 0.5f * innerAngle );
	s.RangeRcp = 1.0f / range;
	s.volumetrick = 0;

}

void LightManager::BindCapsuleLight()
{

}

void LightManager::BindAreaRectLight(Entity entity, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, float range, const DirectX::XMFLOAT3& right,
	float rightExtent, float upExtent, const DirectX::XMFLOAT3& color, float intensity)
{

	_entityToAreaRectLight[entity] = AreaRectLight(position, normal, range, right, rightExtent, upExtent, color, intensity);

}

void LightManager::ChangeLightRange(const Entity& entity, float range)
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		got->second.range = range;
	}
}

void LightManager::ChangeLightIntensity(const Entity& entity, float intensity)
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		got->second.intensity = intensity;
	}
	auto got2 = _entityToSpotLight.find(entity);

	if (got2 != _entityToSpotLight.end())
	{
		got2->second.Intensity = intensity;
	}
}

void LightManager::ChangeLightColor(const Entity& entity, const DirectX::XMFLOAT3& color)
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		got->second.color = color;
	}
	auto got2 = _entityToSpotLight.find(entity);

	if (got2 != _entityToSpotLight.end())
	{
		got2->second.Color = color;
	}
}

XMFLOAT3 LightManager::GetLightColor(const Entity& entity)
{
	auto got = _entityToPointLight.find(entity);
	if (got != _entityToPointLight.end())
	{
		return got->second.color;
	}

	auto got2 = _entityToSpotLight.find(entity);
	if (got2 != _entityToSpotLight.end())
	{
		return got2->second.Color;
	}

	auto got3 = _entityToCapsuleLight.find(entity);
	if (got3 != _entityToCapsuleLight.end())
	{
		return got3->second.Color;
	}

	auto got4 = _entityToAreaRectLight.find(entity);
	if (got4 != _entityToAreaRectLight.end())
	{
		return got4->second.Color;
	}
}

float LightManager::GetLightIntensity(const Entity& entity)
{
	auto got = _entityToPointLight.find(entity);
	if (got != _entityToPointLight.end())
	{
		return got->second.intensity;
	}

	auto got2 = _entityToSpotLight.find(entity);
	if (got2 != _entityToSpotLight.end())
	{
		return got2->second.Intensity;
	}

	auto got3 = _entityToCapsuleLight.find(entity);
	if (got3 != _entityToCapsuleLight.end())
	{
		return got3->second.Intensity;
	}

	auto got4 = _entityToAreaRectLight.find(entity);
	if (got4 != _entityToAreaRectLight.end())
	{
		return got4->second.Intensity;
	}
}

float LightManager::GetLightRange(const Entity& entity)
{
	auto got = _entityToPointLight.find(entity);
	if (got != _entityToPointLight.end())
	{
		return got->second.range;
	}

	auto got2 = _entityToSpotLight.find(entity);
	if (got2 != _entityToSpotLight.end())
	{
		return got2->second.RangeRcp;
	}

	auto got3 = _entityToCapsuleLight.find(entity);
	if (got3 != _entityToCapsuleLight.end())
	{
		return got3->second.RangeRcp;
	}

	auto got4 = _entityToAreaRectLight.find(entity);
	if (got4 != _entityToAreaRectLight.end())
	{
		return got4->second.Range;
	}
}
void LightManager::ChangeLightBlobRange(const Entity & entity, float range)
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		got->second.blobRange = range;
	}
}

void LightManager::_TransformChanged( const Entity& entity, const XMMATRIX& tran, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		DirectX::XMStoreFloat3(&got->second.position, pos);
	}
	auto got2 = _entityToSpotLight.find(entity);

	if (got2 != _entityToSpotLight.end())
	{
		DirectX::XMStoreFloat3(&got2->second.PositionVS, pos);
		DirectX::XMStoreFloat3(&got2->second.DirectionVS, dir);
		DirectX::XMStoreFloat4x4(&got2->second.world, tran);
	}

	auto got3 = _entityToAreaRectLight.find(entity);
	if (got3 != _entityToAreaRectLight.end())
	{
		DirectX::XMStoreFloat3(&got3->second.Position, pos);

	}
}

void LightManager::GatherLights(PointLightVector& pointLights, SpotLightVector& spotLights, CapsuleLightVector& capsuleLights, AreaRectLightVector& areaRectLights )
{
	for (auto &plights : _entityToPointLight)
	{
		if(plights.second.inFrustum)
			if(plights.second.visible)
		pointLights.push_back(&plights.second);
	}

	for ( auto &slights : _entityToSpotLight )
	{
		spotLights.push_back( &slights.second );
	}

	for ( auto& clights : _entityToCapsuleLight ) // Lots of copying
	{
		capsuleLights.push_back(& clights.second );
	}
	for (auto &arlights : _entityToAreaRectLight)
	{
		areaRectLights.push_back(&arlights.second);
	}
}

void LightManager::RemovePointLight(Entity entity)
{
	auto i = _entityToPointLight.find(entity);
	if (i == _entityToPointLight.end())
	{
		return;
	}
	_entityToPointLight.erase(entity);

}

void LightManager::RemoveSpotLight(Entity entity)
{
	auto i = _entityToSpotLight.find(entity);
	if (i == _entityToSpotLight.end())
	{
		return;
	}
	_entityToSpotLight.erase(entity);
}

void LightManager::RemoveCapsuleLight( Entity entity )
{
	auto i = _entityToCapsuleLight.find( entity );
	if ( i == _entityToCapsuleLight.end() )
	{
		return;
	}
	_entityToCapsuleLight.erase( entity );
}

void LightManager::RemoveAreaRectLight(Entity entity)
{
	auto i = _entityToAreaRectLight.find(entity);
	if (i == _entityToAreaRectLight.end())
	{
		return;
	}
	_entityToAreaRectLight.erase(entity);
}

const void LightManager::SetAsVolumetric(const Entity & entity, int vol)
{
	auto i = _entityToPointLight.find(entity);
	if (i != _entityToPointLight.end())
	{
		i->second.volumetrick = vol;
	}

	auto i2 = _entityToSpotLight.find(entity);
	if (i2 != _entityToSpotLight.end())
	{
		i2->second.volumetrick = vol;
	}
	return void();
}

const void LightManager::BindToRenderer(bool exclusive)
{
	if (exclusive)
		System::GetGraphics()->ClearLightProviders();
	System::GetGraphics()->AddLightProvider(this);
}


const void LightManager::ToggleVisible(const Entity& entity, int visible)
{
	auto meshIt = _entityToPointLight.find(entity);

	if (meshIt != _entityToPointLight.end())
	{
		meshIt->second.visible = visible;
	}
}

void LightManager::SetInFrustum(const Entity & entity, int infrustum)
{
	auto meshIt = _entityToPointLight.find(entity);

	if (meshIt != _entityToPointLight.end())
	{
		meshIt->second.inFrustum = infrustum;
	}
}

void LightManager::SetInFrustum(std::vector<Entity>& entites)
{
	for (auto& e : _entityToPointLight)
{
		e.second.inFrustum = false;
}

	for (auto& e : entites)
	{
		SetInFrustum(e, true);
	}
}
