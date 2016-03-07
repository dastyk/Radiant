#include "EnemyBuilder.h"

#define ENEMY_TYPE_NORMAL_COLOR XMFLOAT3(250.0f / 255.0f, 244.0f/255.0f, 70.0f/255.0f)
#define ENEMY_TYPE_TELEPORTER_COLOR XMFLOAT3(57.0f/255.0f, 232.0f/255.0f, 191.0f/255.0f)
#define ENEMY_TYPE_MINI_GUN_COLOR XMFLOAT3(37.0f/255.0f, 80.0f/255.0f, 0.0f/255.0f)

EnemyBuilder::EnemyBuilder()
{

}
EnemyBuilder::~EnemyBuilder()
{

}
EnemyWithStates* EnemyBuilder::AddNewEnemy(const XMFLOAT3 &position)
{
	int thisEnemy = rand() % NROFENEMYTYPES + 1;
	switch (thisEnemy)
	{
	case 3:
		return AddNewEnemy(position, ENEMY_TYPE_MINI_GUN);

	case 2:
		return AddNewEnemy(position, ENEMY_TYPE_TELEPORTER);
		break;

	default:
		return AddNewEnemy(position, ENEMY_TYPE_NORMAL);
		break;

	}
	
}
EnemyWithStates* EnemyBuilder::AddNewEnemy(const XMFLOAT3 &position, const EnemyTypes typeOfEnemy)
{
	EnemyWithStates* newEnemyWithStates = new EnemyWithStates();
	switch (typeOfEnemy)
	{
		case ENEMY_TYPE_NORMAL:
		{
			Entity newEntity;
			newEntity = _builder->EntityC().Create();

			_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), STARTRANGELIGHT*3.0f, ENEMY_TYPE_NORMAL_COLOR, STARTINTENSITYLIGHT);
			_builder->Light()->SetAsVolumetric(newEntity, true);
			_builder->Light()->ChangeLightBlobRange(newEntity, STARTBLOBRANGELIGHT);
			_builder->Transform()->CreateTransform(newEntity);
			_builder->Bounding()->CreateBoundingSphere(newEntity, STARTBLOBRANGELIGHT*0.3f);
			_builder->Transform()->SetPosition(newEntity, XMVectorSet(position.x, position.y, position.z, 1.0f));
			newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);
			newEnemyWithStates->_thisEnemyStateController = new AIStateController();
			newEnemyWithStates->_thisEnemyStateController->AddState(new AIPatrolState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));
			newEnemyWithStates->_thisEnemyStateController->AddState(new AIAttackState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));
			newEnemyWithStates->_thisEnemyStateController->AddState(new AITransitionState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));
			break;

		}

		case ENEMY_TYPE_TELEPORTER:
		{

			Entity newEntity;
			newEntity = _builder->EntityC().Create();

			_builder->Mesh()->CreateStaticMesh(newEntity, "Assets/Models/BallHorizontal.arf");
			_builder->Material()->BindMaterial(newEntity, "Shaders/GBuffer.hlsl");

			std::vector<string> pro;
			pro.push_back("DiffuseMap");
			pro.push_back("NormalMap");
			pro.push_back("DisplacementMap");
			pro.push_back("Roughness");

			std::vector<wstring> texs;
			texs.push_back(L"Assets/Textures/Ball.png");
			texs.push_back(L"Assets/Textures/Floor_NM.png");
			texs.push_back(L"Assets/Textures/Floor_Disp.png");
			texs.push_back(L"Assets/Textures/Floor_Roughness.png");

			_builder->Material()->SetEntityTexture(newEntity, pro, texs);

			_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), STARTRANGELIGHT*3.0f, ENEMY_TYPE_TELEPORTER_COLOR, STARTINTENSITYLIGHT);
			_builder->Light()->SetAsVolumetric(newEntity, true);
			_builder->Light()->ChangeLightBlobRange(newEntity, STARTBLOBRANGELIGHT);
			_builder->Transform()->CreateTransform(newEntity);
			_builder->Bounding()->CreateBoundingSphere(newEntity, STARTBLOBRANGELIGHT *0.3f / 0.01f);
			_builder->Transform()->SetPosition(newEntity, XMVectorSet(position.x, position.y, position.z, 1.0f));
			newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);
			newEnemyWithStates->_thisEnemyStateController = new AIStateController();
			newEnemyWithStates->_thisEnemyStateController->AddState(new AITeleportMoveState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));

			newEnemyWithStates->_thisEnemy->SetScaleFactor(0.01f);
			_builder->Transform()->SetScale(newEntity, XMFLOAT3(0.01f, 0.01f, 0.01f));

			break;
		}

		case ENEMY_TYPE_MINI_GUN:
		{
			Entity newEntity;
			newEntity = _builder->EntityC().Create();
		
			
			_builder->Mesh()->CreateStaticMesh(newEntity, "Assets/Models/AncientSymbolTriangulated.arf");
			_builder->Material()->BindMaterial(newEntity, "Shaders/GBuffer.hlsl");

			std::vector<string> pro;
			pro.push_back("DiffuseMap");
			pro.push_back("NormalMap");
			pro.push_back("DisplacementMap");
			pro.push_back("Roughness");

			std::vector<wstring> texs;
			texs.push_back(L"Assets/Textures/Floor_Dif.png");
			texs.push_back(L"Assets/Textures/Floor_NM.png");
			texs.push_back(L"Assets/Textures/Floor_Disp.png");
			texs.push_back(L"Assets/Textures/Floor_Roughness.png");

			_builder->Material()->SetEntityTexture(newEntity, pro, texs);

		
			_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), STARTRANGELIGHT*3.0f, ENEMY_TYPE_MINI_GUN_COLOR, STARTINTENSITYLIGHT);
			_builder->Light()->SetAsVolumetric(newEntity, true);
			_builder->Light()->ChangeLightBlobRange(newEntity, STARTBLOBRANGELIGHT);
			_builder->Transform()->CreateTransform(newEntity);
			_builder->Transform()->SetScale(newEntity, XMFLOAT3(0.04f, 0.04f, 0.04f));


			_builder->Bounding()->CreateBoundingSphere(newEntity, STARTBLOBRANGELIGHT *0.3f / 0.04f);
			_builder->Transform()->SetPosition(newEntity, XMVectorSet(position.x, position.y, position.z, 1.0f));
			newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);
			newEnemyWithStates->_thisEnemy->SetScaleFactor(0.04f);

			newEnemyWithStates->_thisEnemyStateController = new AIStateController();
			newEnemyWithStates->_thisEnemyStateController->AddState(new AIMiniGunLightState(AI_STATE_NONE, _controller, newEnemyWithStates->_thisEnemy, _builder));
			break;
		}
	}
	return newEnemyWithStates;
}