#pragma once
#include "InputManager.h"
#include "GameEntity.h"
#include "Collider.h"
class Player :
	public GameEntity {
private:
	const int LANE_COUNT = 3;
	const float LANE_SIZE = 4.0f;
	const float ANIM_TIME = 0.75f;
	const float SWAY_AMT = 0.15f;

	int laneIndex;
	float currTimer;
	vec3 turnRate;
	vec3 lerpPos;
	vec3 prevLanePosition;
	vec3 direction;
	vec3 prevPosition;

	InputManager* inputMngr;

	void Init();

	void CheckInput(float dt, float t);
	void UpdatePos(float dt);
	void UpdateForward(float dt);
public:
	Player();
	Player(Mesh* mesh, Material* mat, ColliderType colliderType);
	~Player();

	void Update(float dt, float t);
};