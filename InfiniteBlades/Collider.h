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
	ColliderType colliderType;
	XMFLOAT3 center; // Position of the collider
	XMFLOAT3 dimensions; // Collider dimensions
	bool isTrigger;

	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;

	// Default constructor
	Collider()
	{
		colliderType = ColliderType::NONE;
		center = XMFLOAT3(0, 0, 0);
		dimensions = XMFLOAT3(1, 1, 1);
		isTrigger = false;
	}

	// Parameterized constructor 
	Collider(ColliderType colltype, XMFLOAT3 cent, XMFLOAT3 dims, bool trigger)
	{
		colliderType = colltype;
		center = cent;

		//Take scale from transform and divide by two for dimensions
		XMStoreFloat3(&dimensions, XMLoadFloat3(&dims) / 2);

		isTrigger = trigger;
	}
};