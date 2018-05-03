#pragma once
#include <vector>
#include "Player.h"
#include <iostream>
#include "CollisionSolver.h"
#include "SpriteFont.h"
class GameManager {
private:
	GameManager();
	~GameManager();
	static GameManager* instance;

	std::vector<GameEntity*> sceneryList;
	std::vector<GameEntity*> collectibleList;
	std::vector<GameEntity*> obstacleList;
	std::vector<GameEntity*> fishList;

	float lanes[3];

	int score;
	int health;
	Player* player;

	std::unique_ptr<SpriteBatch> spriteBatch;
	std::unique_ptr<SpriteFont> spriteFont;

  DirectX::XMFLOAT3* rockPositions = nullptr;
	int rockCount = 0;

public:
	static GameManager* GetInstancce();
	static void ReleaseInstance();

	void AddToScenery(GameEntity* entity);
	void AddToObstacle(GameEntity* obstacle);
	void AddToCollectible(GameEntity* collectible);
	void AddToFish(GameEntity* collectible);

	void Init(ID3D11Device * device, ID3D11DeviceContext * context);
	void DrawUI(int height);

	std::vector<GameEntity*> GetSceneryList();
	std::vector<GameEntity*> GetCollectibleList();
	std::vector<GameEntity*> GetObstacleList();
	std::vector<GameEntity*> GetFishList();

	DirectX::XMFLOAT4X4* instanceWorldMatrices = nullptr;

	DirectX::XMFLOAT4X4* GetWorldMatrices(); // = nullptr;
	void InitInstancing(int count);

	void ResolveCollectibleCollision();
	void ResolveObstacleCollision();

	void UpdateWorld(float deltaTime);

	void SetPlayer(Player* player);
};