#include "Material.h"


Material::Material(SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* fileName)
{
	vertexShader = vShader;
	pixelShader = pShader;

	DirectX::CreateWICTextureFromFile(device, context, fileName, 0, &srvPtr);

	D3D11_SAMPLER_DESC samplerDesc = D3D11_SAMPLER_DESC();
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, &samplerPtr);
}

Material::Material(SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* fileName, bool transparentBool)
{
	vertexShader = vShader;
	pixelShader = pShader;

	DirectX::CreateWICTextureFromFile(device, context, fileName, 0, &srvPtr);

	D3D11_SAMPLER_DESC samplerDesc = D3D11_SAMPLER_DESC();
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, &samplerPtr);
	if (transparentBool) {
		D3D11_BLEND_DESC bd = CreateTransparentDesc();
		//create blend states
		device->CreateBlendState(&bd, &transparentBlendState);
		context->OMSetBlendState(transparentBlendState, 0, 0xFFFFFFFF);
	}
}
 
Material::~Material()
{
	if (srvPtr != nullptr) srvPtr->Release();
	if (samplerPtr != nullptr) samplerPtr->Release();
}

SimpleVertexShader * Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Material::GetPixelShader()
{
	return pixelShader;
}

void Material::PrepareMaterial(mat4* worldMat)
{
	vertexShader->SetMatrix4x4("world", *worldMat);
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetSamplerState("basicSampler", samplerPtr);
	pixelShader->SetShaderResourceView("diffuseTexture", srvPtr);
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();
}

D3D11_BLEND_DESC Material::CreateTransparentDesc()
{
	D3D11_BLEND_DESC bd = {};
	bd.RenderTarget[0].BlendEnable = true;

	// These control the RGB channels
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	// This is for the alpha channel specifically
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return bd;
}
