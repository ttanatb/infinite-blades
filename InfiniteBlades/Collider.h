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

	XMFLOAT3 min;
	XMFLOAT3 max;

};