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

Material::~Material()
{
	if (srvPtr != nullptr) srvPtr->Release();
	if (samplerPtr != nullptr) samplerPtr->Release();
}

void Material::PrepareMaterial(mat4* viewMat, mat4* projMat, mat4* worldMat)
{
	vertexShader->SetMatrix4x4("world", *worldMat);
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();
	pixelShader->SetSamplerState("basicSampler", samplerPtr);
	pixelShader->SetShaderResourceView("diffuseTexture", srvPtr);
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();
}
