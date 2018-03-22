#pragma once
#include "DDSTextureLoader.h"
#include "SimpleShader.h"
#include "DXCore.h"
#include "Camera.h"

// For the DirectX Math library
using namespace DirectX;

class Skybox
{
public:
	Skybox(wchar_t* fileName, ID3D11Device* device, 
		D3D11_SAMPLER_DESC samplerDesc,
		D3D11_RASTERIZER_DESC rs,
		D3D11_DEPTH_STENCIL_DESC ds,
		SimpleVertexShader* vs,
		SimplePixelShader* ps);
	void prepeareSkybox(Camera* camera);
	SimpleVertexShader* getVertexShader();
	SimplePixelShader* getPixelShader();
	ID3D11ShaderResourceView* getShaderResourceView();
	ID3D11SamplerState* getSamplerState();
	ID3D11DepthStencilState* getDepthState();
	ID3D11RasterizerState* getRastState();
	~Skybox();
private:
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11SamplerState* sampler;
	ID3D11RasterizerState* rastState;
	ID3D11DepthStencilState* depthState;
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

};

