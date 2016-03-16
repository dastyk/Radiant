#include "EnemyBuilder.h"

#define ENEMY_TYPE_NORMAL_COLOR XMFLOAT3(250.0f / 255.0f, 244.0f/255.0f, 70.0f/255.0f)
#define ENEMY_TYPE_TELEPORTER_COLOR XMFLOAT3(57.0f/255.0f, 232.0f/255.0f, 191.0f/255.0f)
#define ENEMY_TYPE_MINI_GUN_COLOR XMFLOAT3(37.0f/255.0f, 80.0f/255.0f, 0.0f/255.0f)
#define ENEMY_TYPE_SHADOW_COLOR XMFLOAT3(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f)
#define ENEMY_TYPE_PROXIMITY_SITH_COLOR XMFLOAT3(148.0f / 255.0f, 0.0f, 1.0f)

EnemyBuilder::EnemyBuilder()
{

}
EnemyBuilder::~EnemyBuilder()
{

}
EnemyWithStates* EnemyBuilder::AddNewEnemy(const XMFLOAT3 &position)
{
	EnemyTypes thisEnemy = static_cast<EnemyTypes>(1U << (rand() % NROFENEMYTYPES));

	return AddNewEnemy( position, thisEnemy);
	
}
EnemyWithStates* EnemyBuilder::AddNewEnemy(const XMFLOAT3 &position, const EnemyTypes typeOfEnemy)
{
	EnemyWithStates* newEnemyWithStates = new EnemyWithStates();
	switch (typeOfEnemy)
	{
	case EnemyTypes::ENEMY_TYPE_NORMAL:
		{
			Entity newEntity;
			newEntity = _builder->EntityC().Create();

			_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), STARTRANGELIGHT*3.0f, ENEMY_TYPE_NORMAL_COLOR, STARTINTENSITYLIGHT);
			_builder->Light()->SetAsVolumetric(newEntity, true);
			_builder->Light()->ChangeLightBlobRange(newEntity, STARTBLOBRANGELIGHT);
			_builder->Transform()->CreateTransform(newEntity);
			_builder->Bounding()->CreateBoundingSphere(newEntity, STARTBLOBRANGELIGHT*0.5f);
			_builder->Transform()->SetPosition(newEntity, XMVectorSet(position.x, position.y, position.z, 1.0f));
			newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);
			newEnemyWithStates->_thisEnemyStateController = new AIStateController();
			newEnemyWithStates->_thisEnemyStateController->AddState(new AIPatrolState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));
			newEnemyWithStates->_thisEnemyStateController->AddState(new AIAttackState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));
			newEnemyWithStates->_thisEnemyStateController->AddState(new AITransitionState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));

			
			//Add 10 bricks around the light
			XMVECTOR enemyPos = XMVectorSet(position.x, position.y, position.z, 1.0f);
			const int nrOfBricks = 5;
		

			std::vector<string> pro;
			pro.push_back("DiffuseMap");
			pro.push_back("NormalMap");
			pro.push_back("Roughness");

			std::vector<wstring> texs;
			texs.push_back(L"Assets/Textures/Enemy_Brick_Dif_01.png");
			texs.push_back(L"Assets/Textures/Enemy_Brick_NM.png");
			texs.push_back(L"Assets/Textures/Enemy_Brick_Roughness.png");
			for (int i = 0; i < nrOfBricks; ++i)
			{
				
				XMVECTOR offset = XMVectorSet(static_cast<float>((rand() % 100) - 50), static_cast<float>((rand() % 100) - 50), static_cast<float>((rand() % 100) - 50), 0.0f);
				offset = XMVector3Normalize(offset);
				offset = XMVectorScale(offset, 0.33f);
				
				Entity brick = _builder->EntityC().Create();

				unsigned int br = rand() % 2 + 1;

				_builder->Mesh()->CreateStaticMesh(brick, ("Assets/Models/Enemy_Brick_0" + to_string(br) + ".arf").c_str());
				_builder->Material()->BindMaterial(brick, "Shaders/GBuffer.hlsl");
				_builder->Material()->SetEntityTexture(brick, pro, texs);
				_builder->Bounding()->CreateBoundingSphere(brick, _builder->Mesh()->GetMesh(brick));
				_builder->Transform()->CreateTransform(brick);
				
				_builder->Transform()->SetDirection(brick, -offset);
				if (br == 2)
					_builder->Transform()->RotateRoll(brick, 90.0f);

				_builder->Transform()->SetScale(brick, XMVectorSet(0.1f, 0.1f, 0.1f, 0.0f));
				
				Entity brickRot = _builder->EntityC().Create();
				_builder->Transform()->CreateTransform(brickRot);
				_builder->Transform()->BindChild(newEntity, brickRot);
				_builder->Transform()->BindChild(brickRot, brick);
				unsigned int rot = rand() % 3;

				_builder->Animation()->CreateAnimation(brickRot, "rotate", 90.0f, 
					[this, brickRot, rot](float delta, float amount, float offset) 
				{
					switch (rot)
					{
					case 0:
						_builder->Transform()->RotatePitch(brickRot, delta);
						break;
					case 1:
						_builder->Transform()->RotateYaw(brickRot, delta);
						break;
					case 2:
						_builder->Transform()->RotateRoll(brickRot, delta);
						break;
					default:
						break;
					}
				},
					[this, brickRot]()
				{
					_builder->Animation()->PlayAnimation(brickRot, "rotate", 360.0f*20.0f);
				});
				_builder->Animation()->PlayAnimation(brickRot, "rotate", 360.0f*20.0f);


				_builder->Transform()->SetPosition(brick, offset);
				newEnemyWithStates->_thisEnemy->AddChild(brick);
				newEnemyWithStates->_thisEnemy->AddChild(brickRot);
			}


			break;

		}

		case EnemyTypes::ENEMY_TYPE_TELEPORTER:
		{

			Entity newEntity;
			newEntity = _builder->EntityC().Create();


			Entity emesh = _builder->EntityC().Create();

			_builder->Mesh()->CreateStaticMesh(emesh, "Assets/Models/BallHorizontal.arf");
			_builder->Material()->BindMaterial(emesh, "Shaders/GBuffer.hlsl");

			std::vector<string> pro;
			pro.push_back("DiffuseMap");
			pro.push_back("NormalMap");
			pro.push_back("Roughness");

			std::vector<wstring> texs;
			texs.push_back(L"Assets/Textures/Ball.png");
			texs.push_back(L"Assets/Textures/Enemy_Brick_NM.png");
			texs.push_back(L"Assets/Textures/Enemy_Brick_Roughness.png");

			_builder->Material()->SetEntityTexture(emesh, pro, texs);

			_builder->Transform()->CreateTransform(emesh);
			_builder->Transform()->CreateTransform(newEntity);
			_builder->Transform()->BindChild(newEntity, emesh);


			_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), STARTRANGELIGHT*3.0f, ENEMY_TYPE_TELEPORTER_COLOR, STARTINTENSITYLIGHT);
			_builder->Light()->SetAsVolumetric(newEntity, true);
			_builder->Light()->ChangeLightBlobRange(newEntity, STARTBLOBRANGELIGHT);

			_builder->Bounding()->CreateBoundingSphere(newEntity, _builder->Mesh()->GetMesh(emesh));
			_builder->Bounding()->CreateBoundingSphere(emesh, _builder->Mesh()->GetMesh(emesh));

			_builder->Transform()->SetPosition(newEntity, XMVectorSet(position.x, position.y, position.z, 1.0f));
			newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);
			newEnemyWithStates->_thisEnemyStateController = new AIStateController();
			newEnemyWithStates->_thisEnemyStateController->AddState(new AITeleportMoveState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));

			newEnemyWithStates->_thisEnemy->SetScaleFactor(0.0075f);
			_builder->Transform()->SetScale(newEntity, XMFLOAT3(0.005f, 0.005f, 0.005f));


			_builder->Animation()->CreateAnimation(emesh, "rotate", 90.0f,
				[this, emesh](float delta, float amount, float offset)
			{
				_builder->Transform()->RotateYaw(emesh, delta);
			},
				[this, emesh]()
			{
				_builder->Animation()->PlayAnimation(emesh, "rotate", 360.0f*20.0f);
			});
			_builder->Animation()->PlayAnimation(emesh, "rotate", 360.0f*20.0f);
			
			newEnemyWithStates->_thisEnemy->AddChild(emesh);

			break;
		}

		case EnemyTypes::ENEMY_TYPE_MINI_GUN:
		{
			Entity newEntity;
			newEntity = _builder->EntityC().Create();
		
			
			_builder->Mesh()->CreateStaticMesh(newEntity, "Assets/Models/BallFlipped.arf");
			_builder->Material()->BindMaterial(newEntity, "Shaders/GBuffer.hlsl");

			std::vector<string> pro;
			pro.push_back("DiffuseMap");
			pro.push_back("NormalMap");
			pro.push_back("Roughness");

			std::vector<wstring> texs;
			texs.push_back(L"Assets/Textures/Ball.png");
			texs.push_back(L"Assets/Textures/Enemy_Brick_NM.png");
			texs.push_back(L"Assets/Textures/Enemy_Brick_Roughness.png");

			_builder->Material()->SetEntityTexture(newEntity, pro, texs);

		
			_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), STARTRANGELIGHT*3.0f, ENEMY_TYPE_MINI_GUN_COLOR, STARTINTENSITYLIGHT);
			_builder->Light()->SetAsVolumetric(newEntity, true);
			_builder->Light()->ChangeLightBlobRange(newEntity, STARTBLOBRANGELIGHT);
			_builder->Transform()->CreateTransform(newEntity);

			_builder->Bounding()->CreateBoundingSphere(newEntity, _builder->Mesh()->GetMesh(newEntity));
			_builder->Transform()->SetPosition(newEntity, XMVectorSet(position.x, position.y, position.z, 1.0f));
			_builder->Transform()->SetScale(newEntity, XMFLOAT3(0.005f, 0.005f, 0.005f));


			newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);
			newEnemyWithStates->_thisEnemy->SetScaleFactor(0.01f);

			newEnemyWithStates->_thisEnemyStateController = new AIStateController();
			newEnemyWithStates->_thisEnemyStateController->AddState(new AIMiniGunLightState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));
			break;
		}

		case EnemyTypes::ENEMY_TYPE_SHADOW:
		{
			Entity newEntity;
			newEntity = _builder->EntityC().Create();

			_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), STARTRANGELIGHT*3.0f, ENEMY_TYPE_SHADOW_COLOR, STARTINTENSITYLIGHT);
			_builder->Light()->SetAsVolumetric(newEntity, true);
			_builder->Light()->ChangeLightBlobRange(newEntity, STARTBLOBRANGELIGHT);
			_builder->Transform()->CreateTransform(newEntity);
			_builder->Bounding()->CreateBoundingSphere(newEntity, STARTBLOBRANGELIGHT *0.3f);
			_builder->Transform()->SetPosition(newEntity, XMVectorSet(position.x, position.y, position.z, 1.0f));
			newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);

			newEnemyWithStates->_thisEnemyStateController = new AIStateController();
			newEnemyWithStates->_thisEnemyStateController->AddState(new AIPatrolState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));
			newEnemyWithStates->_thisEnemyStateController->AddState(new AIWalkIntoTheLightAttackState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));

			break;
		}

		case EnemyTypes::ENEMY_TYPE_PROXIMITY_SITH:
		{
			Entity e = _builder->EntityC().Create();

			_builder->Light()->BindPointLight( e, XMFLOAT3( 0.0f, 0.0f, 0.0f ), STARTRANGELIGHT * 3.0f, ENEMY_TYPE_PROXIMITY_SITH_COLOR, STARTINTENSITYLIGHT );
			_builder->Light()->SetAsVolumetric( e, true );
			_builder->Light()->ChangeLightBlobRange( e, STARTBLOBRANGELIGHT );
			_builder->Transform()->CreateTransform( e );
			_builder->Bounding()->CreateBoundingSphere( e, STARTBLOBRANGELIGHT*0.3f );
			_builder->Transform()->SetPosition( e, XMVectorSet( position.x, position.y, position.z, 1.0f ) );
			_builder->ProximityLightning()->Add( e );
			newEnemyWithStates->_thisEnemy = new Enemy( e, _builder );
			newEnemyWithStates->_thisEnemyStateController = new AIStateController();
			newEnemyWithStates->_thisEnemyStateController->AddState( new AIPatrolState( AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder ) );
			newEnemyWithStates->_thisEnemyStateController->AddState( new AIAttackState( AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder ) );
			newEnemyWithStates->_thisEnemyStateController->AddState( new AITransitionState( AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder ) );
		}
	}
	_builder->Transform()->RotateRoll(newEnemyWithStates->_thisEnemy->GetEntity(), 0.0f);
	return newEnemyWithStates;
}