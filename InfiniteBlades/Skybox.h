#pragma once
#include "DDSTextureLoader.h"
#include "SimpleShader.h"
#include "DXCore.h"
#include "Camera.h"
#include "Mesh.h"
// For the DirectX Math library
using namespace DirectX;

class Skybox
{
public:
	Skybox(wchar_t* fileName, ID3D11Device* device, 
		SimpleVertexShader* vs,
		SimplePixelShader* ps,
		Mesh* mesh);
	void PrepeareSkybox(Camera* camera);
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11SamplerState* GetSamplerState();
	ID3D11DepthStencilState* GetDepthState();
	ID3D11RasterizerState* GetRastState();
	void Render(ID3D11DeviceContext* context, Camera* camera, UINT& stride, UINT& offset);
	~Skybox();
private:
	ID3D11Buffer* skyVB;
	ID3D11Buffer* skyIB;
	Mesh* skyMesh;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11SamplerState* sampler;
	ID3D11RasterizerState* rastState;
	ID3D11DepthStencilState* depthState;
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

};

