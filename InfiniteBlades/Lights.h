#pragma once
#include <DirectXMath.h>

//Diffuse COlor, Location
struct DirectionalLight {
	DirectX::XMFLOAT4 DiffuseColor;
	DirectX::XMFLOAT3 Direction;
};

//Diffuse COlor, Location
struct PointLight {
	DirectX::XMFLOAT4 DiffuseColor;
	DirectX::XMFLOAT3 Location;
};