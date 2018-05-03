#include "GameManager.h"
GameManager* GameManager::instance = nullptr;

GameManager::GameManager()
{
	sceneryList = std::vector<GameEntity*>();
	collectibleList = std::vector<GameEntity*>();
	obstacleList = std::vector<GameEntity*>();
	lanes[0] = -2.f;
	lanes[1] = 0.0f;
	lanes[2] = 2.f;
}

GameManager::~GameManager()
{
	for (size_t i = 0; i < sceneryList.size(); ++i)
		delete sceneryList[i];

	for (size_t i = 0; i < obstacleList.size(); ++i)
		delete obstacleList[i];

	for (size_t i = 0; i < collectibleList.size(); ++i)
		delete collectibleList[i];

	if (instanceWorldMatrices != nullptr) delete[] instanceWorldMatrices;
	if (rockPositions != nullptr) delete[] rockPositions;
}

GameManager * GameManager::GetInstancce()
{
	if (instance == nullptr)
		instance = new GameManager();
	return instance;
}

void GameManager::ReleaseInstance()
{
	if (instance != nullptr)
		delete instance;
	instance = nullptr;
}

void GameManager::AddToScenery(GameEntity * entity)
{
	sceneryList.push_back(entity);
}


void GameManager::AddToObstacle(GameEntity * obstacle)
{
	obstacleList.push_back(obstacle);
}

void GameManager::AddToCollectible(GameEntity * collectible)
{
	collectibleList.push_back(collectible);
}

std::vector<GameEntity*> GameManager::GetSceneryList()
{
	return sceneryList;
}

std::vector<GameEntity*> GameManager::GetCollectibleList()
{
	return collectibleList;
}

std::vector<GameEntity*> GameManager::GetObstacleList()
{
	return obstacleList;
}

DirectX::XMFLOAT4X4 * GameManager::GetWorldMatrices()
{
	return instanceWorldMatrices;
}

void GameManager::InitInstancing(int count)
{
	instanceWorldMatrices = new XMFLOAT4X4[count];
	rockPositions = new XMFLOAT3[count];
	rockCount = count;
	for (int i = 0; i < rockCount; i++) {
		rockPositions[i].x = 6.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (30.0f - 6.0f)));
		if (rand() % 2 == 1) rockPositions[i].x = -rockPositions[i].x;
		rockPositions[i].y = -0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.5f - -0.5f)));
		rockPositions[i].z = (0.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (120.0f - 0.0f))));
	}
}

void GameManager::ResolveCollectibleCollision()
{
	for (int i = 0; i < collectibleList.size(); ++i)
	{
		GameEntity * collectible = collectibleList[i];
		if (CollisionSolver::DetectCollision(player, collectible)) {
			collectible->isActive = false;
			//add score to player or something
		}
	}
}

void GameManager::ResolveObstacleCollision()
{
	for (int i = 0; i < obstacleList.size(); ++i)
	{
		GameEntity * obstacle = obstacleList[i];
		if (CollisionSolver::DetectCollision(player, obstacle)) {
			obstacle->isActive = false;
			//destroy player or something
		}
	}
}

//This should move everything back acc to the speed
void GameManager::UpdateWorld(float deltaTime)
{
	float speed = -3.f * deltaTime;
	float moveBackAmt = 120.0f;
	float rotateSpd = 3.0f * deltaTime;

	for (int i = 0; i < sceneryList.size(); ++i)
	{
		sceneryList[i]->Update();
		sceneryList[i]->TranslateBy(0.0f, 0.0f, speed);

		if (sceneryList[i]->GetPosition().z < 0.0f)
			sceneryList[i]->TranslateBy(0.0f, 0.0f, moveBackAmt);
	}

	for (int i = 0; i < collectibleList.size(); ++i)
	{
		collectibleList[i]->Update();
		collectibleList[i]->TranslateBy(0.0f, 0.0f, speed);
		collectibleList[i]->RotateOnAxis(vec3(0, 1, 0), rotateSpd);

		if (collectibleList[i]->GetPosition().z < 0.0f) {
			collectibleList[i]->SetPosition(lanes[rand() % 3], 0.75f, moveBackAmt);
			collectibleList[i]->isActive = true;

		}
	}

	for (int i = 0; i < obstacleList.size(); ++i)
	{
		obstacleList[i]->Update();
		obstacleList[i]->TranslateBy(0.0f, 0.0f, speed);
		collectibleList[i]->RotateOnAxis(vec3(0, 1, 0), rotateSpd);

		if (obstacleList[i]->GetPosition().z < 0.0f) {
			obstacleList[i]->SetPosition(lanes[rand() % 3], 1.f, moveBackAmt);
			obstacleList[i]->isActive = true;
		}
	}

	for (int i = 0; i < rockCount; i++) {
		rockPositions[i].z += speed;
		if (rockPositions[i].z < 0.0f) {
			rockPositions[i].z += moveBackAmt - 40.0f;
		}

		XMStoreFloat4x4(&instanceWorldMatrices[i], XMMatrixTranspose(XMMatrixTranslationFromVector(XMLoadFloat3(rockPositions + i))));
	}
}

void GameManager::SetPlayer(Player * player)
{
	this->player = player;
}
