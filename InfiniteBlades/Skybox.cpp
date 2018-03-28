#include "Skybox.h"


Skybox::Skybox(wchar_t* fileName, ID3D11Device* device, 
	SimpleVertexShader* vs,
	SimplePixelShader* ps,
	Mesh* mesh)
{

	//intialize skybox
	D3D11_SAMPLER_DESC samplerDesc = {}; // The {} part zeros out the struct!
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Setting this allows for mip maps to work! (if they exist)
	//restrizer description
	D3D11_RASTERIZER_DESC rs = {};
	rs.FillMode = D3D11_FILL_SOLID;
	rs.CullMode = D3D11_CULL_FRONT;
	//depth stencil
	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// Load the sky box from a DDS file
	CreateDDSTextureFromFile(device, fileName, 0, &shaderResourceView);
	device->CreateSamplerState(&samplerDesc, &sampler);
	device->CreateRasterizerState(&rs, &rastState);
	device->CreateDepthStencilState(&ds, &depthState);
	pixelShader = ps;
	vertexShader = vs;
	skyMesh = mesh;
	skyIB = mesh->GetIndexBuffer();
	skyVB = mesh->GetVertexBuffer();
}

void Skybox::PrepeareSkybox(Camera* camera)
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

SimpleVertexShader * Skybox::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Skybox::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Skybox::GetShaderResourceView()
{
	return shaderResourceView;
}

ID3D11SamplerState * Skybox::GetSamplerState()
{
	return sampler;
}

ID3D11DepthStencilState * Skybox::GetDepthState()
{
	return depthState;
}

ID3D11RasterizerState * Skybox::GetRastState()
{
	return rastState;
}

void Skybox::Render(ID3D11DeviceContext* context, Camera * camera, UINT& stride, UINT& offset)
{
	context->IASetVertexBuffers(0, 1, &skyVB, &stride, &offset);
	context->IASetIndexBuffer(skyIB, DXGI_FORMAT_R32_UINT, 0);
	//set pixel and vertex shader
	PrepeareSkybox(camera);
	//set rastarizer and depth
	context->RSSetState(GetRastState());
	context->OMSetDepthStencilState(GetDepthState(), 0);
	context->DrawIndexed(skyMesh->GetIndexCount(), 0, 0);
}




Skybox::~Skybox()
{
	sampler->Release();
	shaderResourceView->Release();
	rastState->Release();
	depthState->Release();
}
