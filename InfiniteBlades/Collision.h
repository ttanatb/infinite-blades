#pragma once
#include "Collider.h"

class Collision
{
public:
	static bool DetectCollision(Collider* collider1, Collider* collider2);
	static bool DetectCollisionBoxBox(Collider* box1, Collider* box2);
	static bool DetectCollisionSphereSphere(Collider* sphere1, Collider* sphere2);
	static bool DetectCollisionSphereBox(Collider* sphere, Collider* box);
};