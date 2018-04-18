#include "Player.h"

using namespace DirectX;
Player::Player(Mesh * mesh, Material * mat, ColliderType colliderType) : GameEntity(mesh, mat, colliderType, vec3(0, 0, 6.0f), vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f))
{
	Init();
}

Player::~Player()
{

}

void Player::Init()
{
	laneIndex = 0;
	currTimer = ANIM_TIME;
	lerpPos = position;
	prevLanePosition = lerpPos;
	prevPosition = lerpPos;
	direction = vec3(1.0f, 0.0f, 0.0f);

	inputMngr = InputManager::GetInstance();
}

void Player::CheckInput(float dt, float t)
{
	//can't input while switching lanes
	if (currTimer < ANIM_TIME) return;

	//if turn left
	if (inputMngr->GetKeyDown('A') && !inputMngr->GetKey('D')) {

		//switch index
		laneIndex -= 1;
		if (laneIndex < -LANE_COUNT / 2) {
			laneIndex = -LANE_COUNT / 2;
			return;
		}

		//update/reset variables
		prevLanePosition = position;
		lerpPos.x = laneIndex * LANE_SIZE / 2.0f + XMScalarSin(t * 5.0f) / 5.0f;
		currTimer = 0.0f;
		direction = vec3(-1.0f, 0.0f, 0.0f);
	}

	//if turn right
	else if (inputMngr->GetKeyUp('D') && !inputMngr->GetKey('A')) {

		//switch index
		laneIndex += 1;
		if (laneIndex > LANE_COUNT / 2) {
			laneIndex = LANE_COUNT / 2;
			return;
		}

		//update/reset variables
		prevLanePosition = position;
		lerpPos.x = laneIndex * LANE_SIZE / 2.0f;
		currTimer = 0.0f;
		direction = vec3(1.0f, 0.0f, 0.0f);
	}
}

void Player::UpdatePos(float dt)
{
	prevPosition = position;
	XMVECTOR newPos;

	XMVECTOR currLerpPos = XMLoadFloat3(&lerpPos);

	//if moving towards new position
	if (currTimer < ANIM_TIME) {

		//position driven by lerp from prev lane pos to new lane pos
		newPos = XMVectorLerp(XMLoadFloat3(&prevLanePosition),
			currLerpPos,
			currTimer / ANIM_TIME);
		XMStoreFloat3(&position, newPos);

		//updates the turn rate
		XMVECTOR newTurnRate = XMVectorScale(XMVectorSubtract(newPos, XMLoadFloat3(&prevPosition)), 0.4f / dt);
		XMStoreFloat3(&turnRate, newTurnRate);
	}
	else {
		//sway based on direction
		XMVECTOR swayVec = XMVectorScale(XMLoadFloat3(&direction), SWAY_AMT);

		//position driven by sway + lerp Pos
		//lerp factor is based on sin function
		newPos = XMVectorLerp(XMVectorSubtract(currLerpPos, swayVec),
			XMVectorAdd(currLerpPos, swayVec),
			XMScalarSin((currTimer - ANIM_TIME) * 3.5f) + 0.5f);
		XMStoreFloat3(&position, newPos);

		//update turn rate
		XMVECTOR newTurnRate = XMVectorScale(XMVectorSubtract(newPos, XMLoadFloat3(&prevPosition)), 1.0f / dt);
		XMStoreFloat3(&turnRate, newTurnRate);
	}

	//update timer
	currTimer += dt;
}

void Player::UpdateForward(float dt)
{
	//hoist previous rotation
	XMVECTOR prevRot = XMLoadFloat4(&rotation);

	//slerp from previous rotation towards turn rate
	XMVECTOR newRot = XMQuaternionSlerp(prevRot,
		XMQuaternionRotationRollPitchYaw(0.0f, turnRate.x * 0.5f, 0.0f),
		dt * 3.0f);

	//save rotation
	XMStoreFloat4(&rotation, newRot);
}

void Player::Update(float dt, float t)
{
	CheckInput(dt, t);
	UpdatePos(dt);
	UpdateForward(dt);

	GameEntity::Update();
}