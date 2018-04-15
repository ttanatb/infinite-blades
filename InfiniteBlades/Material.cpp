#include "Material.h"

Material::Material(SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* fileName)
{
	vertexShader = vShader;
	pixelShader = pShader;

	DirectX::CreateWICTextureFromFile(device, context, fileName, 0, &diffuseSRVptr);
	normalSRVptr = nullptr;

	D3D11_SAMPLER_DESC diffuseSamplerDesc = D3D11_SAMPLER_DESC();
	diffuseSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	diffuseSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&diffuseSamplerDesc, &diffuseSamplerPtr);

	D3D11_SAMPLER_DESC normalSamplerDesc = D3D11_SAMPLER_DESC();
	normalSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	normalSamplerDesc.MaxAnisotropy = 16;
	normalSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&normalSamplerDesc, &normalSamplerPtr);
}
 
Material::Material(SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11Device * device, ID3D11DeviceContext * context, const wchar_t * diffuseFileName, const wchar_t * normalFileName)
{
	vertexShader = vShader;
	pixelShader = pShader;

	DirectX::CreateWICTextureFromFile(device, context, diffuseFileName, 0, &diffuseSRVptr);
	DirectX::CreateWICTextureFromFile(device, context, normalFileName, 0, &normalSRVptr);

	D3D11_SAMPLER_DESC diffuseSamplerDesc = D3D11_SAMPLER_DESC();
	diffuseSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	diffuseSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&diffuseSamplerDesc, &diffuseSamplerPtr);

	D3D11_SAMPLER_DESC normalSamplerDesc = D3D11_SAMPLER_DESC();
	normalSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	normalSamplerDesc.MaxAnisotropy = 16;
	normalSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&normalSamplerDesc, &normalSamplerPtr);
}

Material::~Material()
{
	if (diffuseSRVptr != nullptr) diffuseSRVptr->Release();
	if (diffuseSamplerPtr != nullptr) diffuseSamplerPtr->Release();
	if (normalSRVptr != nullptr) normalSRVptr->Release();
	if (normalSamplerPtr != nullptr) normalSamplerPtr->Release();
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

	pixelShader->SetSamplerState("diffuseSampler", diffuseSamplerPtr);
	pixelShader->SetSamplerState("normalSampler", normalSamplerPtr);
	pixelShader->SetShaderResourceView("diffuseTexture", diffuseSRVptr);
	pixelShader->SetShaderResourceView("normalTexture", normalSRVptr);
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();
}
