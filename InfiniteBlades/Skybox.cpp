#include "Skybox.h"


Skybox::Skybox(wchar_t* fileName, ID3D11Device* device, 
	D3D11_SAMPLER_DESC samplerDesc, 
	D3D11_RASTERIZER_DESC rs,
	D3D11_DEPTH_STENCIL_DESC ds,
	SimpleVertexShader* vs,
	SimplePixelShader* ps)
{
	// Load the sky box from a DDS file
	CreateDDSTextureFromFile(device, fileName, 0, &shaderResourceView);
	device->CreateSamplerState(&samplerDesc, &sampler);
	device->CreateRasterizerState(&rs, &rastState);
	device->CreateDepthStencilState(&ds, &depthState);
	pixelShader = ps;
	vertexShader = vs;
}

void Skybox::prepeareSkybox(Camera* camera)
{
	//set skybox vertex shader
	vertexShader->SetMatrix4x4("view", *camera->GetViewMatTransposed());
	vertexShader->SetMatrix4x4("projection", *camera->GetProjMatTransposed());
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();
	//set skybox pixel shader
	pixelShader->SetShaderResourceView("SkyTexture", shaderResourceView);
	pixelShader->SetSamplerState("BasicSampler", sampler);
	pixelShader->SetShader();
}

SimpleVertexShader * Skybox::getVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Skybox::getPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Skybox::getShaderResourceView()
{
	return shaderResourceView;
}

ID3D11SamplerState * Skybox::getSamplerState()
{
	return sampler;
}

ID3D11DepthStencilState * Skybox::getDepthState()
{
	return depthState;
}

ID3D11RasterizerState * Skybox::getRastState()
{
	return rastState;
}


Skybox::~Skybox()
{
	sampler->Release();
	shaderResourceView->Release();
	rastState->Release();
	depthState->Release();
}
