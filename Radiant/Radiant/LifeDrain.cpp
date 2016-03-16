#include "LifeDrain.h"
#include "Shodan.h"
#include "System.h"

LifeDrain::LifeDrain(EntityBuilder* builder, Entity player, List<EnemyWithStates>* _Entities, Player* thePlayer) : Power(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 10.0f;
	_enemies = _Entities;
	_powerLevel = 0;
	_duration = 0.0f;
	_amountDrained = 30.0f;
	_type = power_id_t::LIFEDRAIN;
	_player = thePlayer;

	_powerEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_powerEntity);
	_builder->Bounding()->CreateBoundingSphere(_powerEntity, 0.05f);
	_builder->Light()->BindPointLight(_powerEntity, XMFLOAT3(0, 0, 0), 10.0f, XMFLOAT3(1.0f, 0.0f, 1.0f), 0);
	_builder->Transform()->BindChild(player, _powerEntity);
	_builder->Transform()->MoveDown(_powerEntity, 0.1f);
	_builder->Transform()->MoveForward(_powerEntity, 0.1f);

	_powerName = "Life Drain";
	_description = "Damages enemies around you and heal yourself";
}

LifeDrain::~LifeDrain()
{
	_builder->GetEntityController()->ReleaseEntity(_powerEntity);
}

void LifeDrain::Update(Entity playerEntity, float deltaTime)
{
	_timeSinceLastActivation += deltaTime;

	static float frameTimer = 0.0f;

	frameTimer += deltaTime;

	if (_duration > 0)
	{
		int nrOfEnemies = 0;
		_builder->Light()->ChangeLightIntensity(_powerEntity, 10.0f);
		_duration -= deltaTime;

		for (int i = 0; i < _enemies->Size(); i++)
		{
			XMVECTOR distance = XMVector3Length(_builder->Transform()->GetPosition(_enemies->GetCurrentElement()->_thisEnemy->GetEntity()) - _builder->Transform()->GetPosition(playerEntity));
			
			if (XMVectorGetX(distance) <= 3.0f)
			{
				nrOfEnemies += 1;

				_enemies->GetCurrentElement()->_thisEnemyStateController->OnHit(deltaTime * _amountDrained);
				if (_enemies->GetCurrentElement()->_thisEnemy->GetHealth() <= 0.0f)
				{
					_enemies->GetCurrentElement()->_thisEnemyStateController->ThisOneDied();
					_builder->Lightning()->Remove(_enemies->GetCurrentElement()->_thisEnemy->GetEntity());
					_enemies->RemoveCurrentElement();
					i--;
				}
				else
				{
					if (_enemies->GetCurrentElement()->_thisEnemy->GetCurrentStatusEffects() != STATUS_EFFECT_LIGHTNING_CHAINED)
					{
						_builder->Lightning()->Remove(_enemies->GetCurrentElement()->_thisEnemy->GetEntity());
						_builder->Lightning()->CreateLightningBolt(_enemies->GetCurrentElement()->_thisEnemy->GetEntity(), _powerEntity);
						_enemies->GetCurrentElement()->_thisEnemyStateController->OnHit(0.0f, STATUS_EFFECT_LIGHTNING_CHAINED, 0.2f);
					}

					if (frameTimer >= 0.166667f)
					{
						_builder->Lightning()->Animate(_enemies->GetCurrentElement()->_thisEnemy->GetEntity());
					}
				}

				_player->AddHealth(deltaTime * _amountDrained);

				if (_player->GetHealth() > 100.0f)
				{
					_player->SetHealth(100.0f);
				}
			}
			else
			{
				_builder->Lightning()->Remove(_enemies->GetCurrentElement()->_thisEnemy->GetEntity());
			}
			_enemies->MoveCurrent();
		}

		if (frameTimer >= 0.166667f)
		{
			frameTimer = 0.0f;
		}
	}
	else
	{
		for (int i = 0; i < _enemies->Size(); i++)
		{
			_builder->Lightning()->Remove(_enemies->GetCurrentElement()->_thisEnemy->GetEntity());
			_enemies->MoveCurrent();
		}
		_builder->Light()->ChangeLightIntensity(_powerEntity, 0.0f);
	}

}

float LifeDrain::Activate(bool& exec, float currentLight)
{
	if (_cooldown - _timeSinceLastActivation <= 0.0f && currentLight >= 4.0f)
	{
		_timeSinceLastActivation = 0.0f;
		_duration = 4.0f;

		exec = true;
		return 4.0f;
	}
	exec = false;
	return 0.0f;
}

bool LifeDrain::Upgrade()
{
	if (_powerLevel <= 5)
	{

		_powerLevel++;
		switch (_powerLevel % 2)
		{
		case 0:
		{
			_cooldown -= 1.0f;

			return true;
		}
		default:
		{
			_amountDrained += 10.0;
			return true;
		}
		}
	}

	return false;
}

