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
}

void LightManager::BindCapsuleLight()
{

}

void LightManager::BindAreaRectLight(Entity entity, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, float range, const DirectX::XMFLOAT3& right,
	float rightExtent, float upExtent, const DirectX::XMFLOAT3& color, float intensity)
{

	_entityToAreaRectLight[entity] = AreaRectLight(position, normal, range, right, rightExtent, upExtent, color, intensity);

}

void LightManager::ChangeLightRange(Entity entity, float range)
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		got->second.range = range;
	}
}

void LightManager::ChangeLightIntensity(Entity entity, float intensity)
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

void LightManager::ChangeLightColor(Entity entity, const DirectX::XMFLOAT3& color)
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

XMFLOAT3 LightManager::GetLightColor(Entity entity)
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
		TraceDebug("Tried to remove non-existant pointlight from entity %d", entity.ID);
		return;
	}
	_entityToPointLight.erase(entity);

}

void LightManager::RemoveSpotLight(Entity entity)
{
	auto i = _entityToSpotLight.find(entity);
	if (i == _entityToSpotLight.end())
	{
		TraceDebug("Tried to remove non-existant spotlight from entity %d", entity.ID);
		return;
	}
	_entityToSpotLight.erase(entity);
}

void LightManager::RemoveCapsuleLight( Entity entity )
{
	auto i = _entityToCapsuleLight.find( entity );
	if ( i == _entityToCapsuleLight.end() )
	{
		TraceDebug( "Tried to remove non-existant capsule light from entity %d", entity.ID );
		return;
	}
	_entityToCapsuleLight.erase( entity );
}

void LightManager::RemoveAreaRectLight(Entity entity)
{
	auto i = _entityToAreaRectLight.find(entity);
	if (i == _entityToAreaRectLight.end())
	{
		TraceDebug("Tried to remove non-existant capsule light from entity %d", entity.ID);
		return;
	}
	_entityToAreaRectLight.erase(entity);
}

const void LightManager::SetAsVolumetric(const Entity & entity, bool vol)
{
	auto i = _entityToPointLight.find(entity);
	if (i != _entityToPointLight.end())
	{
		i->second.volumetrick = vol;
	}
	return void();
}

const void LightManager::BindToRenderer(bool exclusive)
{
	if (exclusive)
		System::GetGraphics()->ClearLightProviders();
	System::GetGraphics()->AddLightProvider(this);
}

const void LightManager::ToggleVisible(const Entity & entity, bool visible)
{
	//auto i = _entityToPointLight.find(entity);
	//if (i != _entityToPointLight.end())
	//{
	//	i->second.volumetrick = vol;
	//}
	return void();
}

