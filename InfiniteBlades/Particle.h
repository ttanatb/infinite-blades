#pragma once
#include <DirectXMath.h>

//For Particle Simulation
struct Particle
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 StartVelocity;
	float Size;
	float Age;
};

//For Particle Drawing (to be sent to GPU)
struct ParticleVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 UV;
	DirectX::XMFLOAT3 Color;
	float Size;
};