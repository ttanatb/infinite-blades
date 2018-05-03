#pragma once
#include "Collider.h"
#include "GameEntity.h"
#include <DirectXMath.h>

class CollisionSolver
{
public:
	static bool DetectCollision(const GameEntity* a, const GameEntity* b);
	static bool DetectCollisionBoxBox(const GameEntity* a, const GameEntity* b);
	static bool DetectCollisionSphereSphere(const GameEntity* a, const GameEntity* b);
	static bool DetectCollisionSphereBox(const GameEntity* a, const GameEntity* b);
};