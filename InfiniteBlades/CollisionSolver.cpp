#include "CollisionSolver.h"
#include <stdio.h>

using namespace DirectX;

bool CollisionSolver::DetectCollision(const GameEntity * a, const GameEntity * b)
{
	if (a->GetCollider().colliderType == BOX && b->GetCollider().colliderType == BOX)
	{
		return DetectCollisionBoxBox(a, b);
	}
	else if (a->GetCollider().colliderType == SPHERE && b->GetCollider().colliderType == SPHERE)
	{
		return DetectCollisionSphereSphere(a, b);
	}
	else if (a->GetCollider().colliderType == SPHERE && b->GetCollider().colliderType == BOX)
	{
		return DetectCollisionSphereBox(b, a);
	}
	else if (a->GetCollider().colliderType == BOX && b->GetCollider().colliderType == SPHERE)
	{
		return DetectCollisionSphereBox(a, b);
	}
}

// AABB vs. AABB
bool CollisionSolver::DetectCollisionBoxBox(const GameEntity * a, const GameEntity * b)
{
	float resultantX = a->GetPositionFloat().x - b->GetPositionFloat().x;

	float aExtentX = a->GetCollider().max.x;
	float bExtentX = b->GetCollider().max.x;

	if (fabs(resultantX) <= aExtentX + bExtentX)
	{
		float resultantY = a->GetPositionFloat().y - b->GetPositionFloat().y;

		float aExtentY = a->GetCollider().max.y;
		float bExtentY = b->GetCollider().max.y;

		if (fabs(resultantY) <= aExtentY + bExtentY)
		{
			//printf("Colliding: \n");
			return true;
		}
	}
	//printf("Not colliding: \n");
	return false;
}

// Sphere vs. sphere 
bool CollisionSolver::DetectCollisionSphereSphere(const GameEntity * a, const GameEntity * b)
{
	XMFLOAT3 aPos = a->GetPositionFloat();
	XMFLOAT3 bPos = b->GetPositionFloat();
	XMVECTOR resultantVector = XMLoadFloat3(&aPos) - XMLoadFloat3(&bPos);

	XMFLOAT3 distanceSq;
	XMStoreFloat3(&distanceSq, XMVector3LengthSq(resultantVector));

	float radius = a->GetCollider().max.x + b->GetCollider().max.x;

	if (distanceSq.x < radius * radius)
	{
		//printf("Colliding: \n");
		return true;
	}
	//printf("Not colliding: \n");
	return false;
}

// Sphere vs. AABB
bool CollisionSolver::DetectCollisionSphereBox(const GameEntity * a, const GameEntity * b)
{
	XMFLOAT3 aPos = a->GetPositionFloat();
	XMFLOAT3 bPos = b->GetPositionFloat();
	XMVECTOR resultantVector = XMLoadFloat3(&aPos) - XMLoadFloat3(&bPos);

	XMVECTOR closest = resultantVector;

	XMFLOAT3 aMin = a->GetCollider().min;
	XMFLOAT3 aMax = a->GetCollider().max;

	closest = XMVectorClamp(closest, XMLoadFloat3(&aMin), XMLoadFloat3(&aMax));

	XMFLOAT3 distanceSq;
	XMStoreFloat3(&distanceSq, XMVector3LengthSq(resultantVector - closest));

	float radius = b->GetCollider().max.x;

	if (distanceSq.x < radius * radius)
	{
		//printf("Colliding: %f\n", radius);
		return true;
	}
	//printf("Not colliding: %f \n", radius);
	return false;
}