#include <vector>
#include "GameEntity.h"
//#include "Skybox.h"
//#include "Lights.h"
#include "Camera.h"

#pragma once
// Handles refraction on objects
class Refraction
{
public:
	Refraction(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);
	~Refraction();

	void Init();
	void Release();

	ID3D11ShaderResourceView* GetSRV();
	ID3D11RenderTargetView* GetRTV();


private:
	// Post-Process/Refraction
	ID3D11SamplerState* alternateSampler;
	ID3D11ShaderResourceView* alternateSRV;
	ID3D11RenderTargetView* alternateRTV;

	// Screen stuff
	int width;
	int height;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
};

