#include "Material.h"



Material::Material(SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11Device * device, ID3D11DeviceContext * context, const wchar_t * diffuseFileName, const wchar_t * normalFileName, bool transparentBool, float transparentStr, const wchar_t* reflectionFileName)
{
	InitMaterial(vShader, pShader, device, context, diffuseFileName, normalFileName, reflectionFileName);
	this->transparentBool = transparentBool;
	this->transparentStr = transparentStr;
}



Material::~Material()
{
	if (diffuseSRVptr != nullptr) diffuseSRVptr->Release();
	if (diffuseSamplerPtr != nullptr) diffuseSamplerPtr->Release();
	if (reflectionSamplerPtr != nullptr) reflectionSamplerPtr->Release();
	if (normalSRVptr != nullptr) normalSRVptr->Release();
	if (reflectionSRVptr != nullptr) reflectionSRVptr->Release();
}

void Material::InitMaterial(SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11Device * device, ID3D11DeviceContext * context, const wchar_t * diffuseFileName, const wchar_t * normalFileName, const wchar_t* reflectionFileName)
{
	vertexShader = vShader;
	pixelShader = pShader;

	DirectX::CreateWICTextureFromFile(device, context, diffuseFileName, 0, &diffuseSRVptr);
	if (normalFileName != nullptr)
		DirectX::CreateWICTextureFromFile(device, context, normalFileName, 0, &normalSRVptr);
	else
		DirectX::CreateWICTextureFromFile(device, context, L"Assets/Textures/defaultNormal.png", 0, &normalSRVptr);

	if (reflectionFileName != nullptr)
		DirectX::CreateDDSTextureFromFile(device, context, reflectionFileName, 0, &reflectionSRVptr);
	else
		DirectX::CreateDDSTextureFromFile(device, L"Assets/Textures/defaultReflection.dds", 0, &reflectionSRVptr);

	D3D11_SAMPLER_DESC diffuseSamplerDesc = D3D11_SAMPLER_DESC();
	diffuseSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	diffuseSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&diffuseSamplerDesc, &diffuseSamplerPtr);

	D3D11_SAMPLER_DESC reflectionSamplerDesc = D3D11_SAMPLER_DESC();
	reflectionSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	reflectionSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	reflectionSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	reflectionSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	reflectionSamplerDesc.MipLODBias = 0.0f;
	reflectionSamplerDesc.MaxAnisotropy = 1;
	reflectionSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	reflectionSamplerDesc.BorderColor[0] = 0;
	reflectionSamplerDesc.BorderColor[1] = 0;
	reflectionSamplerDesc.BorderColor[2] = 0;
	reflectionSamplerDesc.BorderColor[3] = 0;
	reflectionSamplerDesc.MinLOD = 0;
	reflectionSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&reflectionSamplerDesc, &reflectionSamplerPtr);
}



SimpleVertexShader * Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Material::GetNormalSRV() const
{
	return normalSRVptr;
}

bool Material::GetTransparentBool()
{
	return transparentBool;
}

void Material::SetTransparentState(bool transparentBool)
{
	this->transparentBool = transparentBool;
}

float Material::GetTransparentStr()
{
	return transparentStr;
}

void Material::SetReflectionSRV(ID3D11ShaderResourceView * srv)
{
	this->reflectionPlanarSRVptr = srv;
}

void Material::PrepareMaterial(mat4* worldMat)
{
	if (worldMat != nullptr)
		vertexShader->SetMatrix4x4("world", *worldMat);
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetSamplerState("diffuseSampler", diffuseSamplerPtr);
	pixelShader->SetShaderResourceView("diffuseTexture", diffuseSRVptr);
	pixelShader->SetShaderResourceView("normalTexture", normalSRVptr);
	pixelShader->SetShaderResourceView("skyTexture", reflectionSRVptr);
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();
}

void Material::PreparePlanarReflectionMaterial(mat4 * worldMat, mat4 * viewMat)
{
	vertexShader->SetMatrix4x4("world", *worldMat);
	vertexShader->SetMatrix4x4("reflectionMatrix", *viewMat);
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetSamplerState("diffuseSampler", diffuseSamplerPtr);
	pixelShader->SetSamplerState("reflectionSampler", reflectionSamplerPtr);
	pixelShader->SetShaderResourceView("diffuseTexture", diffuseSRVptr);
	pixelShader->SetShaderResourceView("normalTexture", normalSRVptr);
	pixelShader->SetShaderResourceView("reflectionTexture", reflectionPlanarSRVptr);
	pixelShader->SetShaderResourceView("skyTexture", reflectionSRVptr);
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();
}