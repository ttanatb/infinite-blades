#pragma once

#include <DirectXMath.h>

using namespace DirectX;

enum ColliderType
{
	BOX,
	SPHERE,
	MESH,
	NONE
};

struct Collider
{
	ColliderType collisionType;
	XMFLOAT3 center; // Position of the collider
	XMFLOAT3 dimensions; // Collider dimensions
	bool isTrigger;


	// Default constructor
	Collider()
	{
		collisionType = ColliderType::NONE;
		center = XMFLOAT3(0, 0, 0);
		dimensions = XMFLOAT3(1, 1, 1);
		isTrigger = false;
	}

	// Parameterized constructor 
	Collider(ColliderType colltype, XMFLOAT3 cent, XMFLOAT3 dims, bool trigger)
	{
		collisionType = colltype;
		center = cent;
		dimensions = XMFLOAT3(dims.x / 2, dims.y / 2, dims.z / 2);
		isTrigger = trigger;
	}
};