#pragma once
#include <vector>
#include "Player.h"
#include <iostream>
#include "CollisionSolver.h"
class GameManager {
private:
	GameManager();
	~GameManager();
	static GameManager* instance;

	std::vector<GameEntity*> sceneryList;
	std::vector<GameEntity*> collectibleList;
	std::vector<GameEntity*> obstacleList;

	Player* player;
public:
	static GameManager* GetInstancce();
	static void ReleaseInstance();

	void AddToScenery(GameEntity* entity);
	void AddToObstacle(GameEntity* obstacle);
	void AddToCollectible(GameEntity* collectible);

	std::vector<GameEntity*> GetSceneryList();
	std::vector<GameEntity*> GetCollectibleList();
	std::vector<GameEntity*> GetObstacleList();

	void ResolveCollectibleCollision();
	void ResolveObstacleCollision();

	void UpdateWorld(float deltaTime);

	void SetPlayer(Player* player);
};