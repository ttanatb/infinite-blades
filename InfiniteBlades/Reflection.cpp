#include "Reflection.h"

Reflection::Reflection(ID3D11Device * device, ID3D11DeviceContext * context, int height, int width)
{
	this->height = height;
	this->width = width;
	this->device = device;
	this->context = context;
}

void Reflection::Init()
{
	//Create the render target texture
	DXGI_SAMPLE_DESC sampleDesc{ 1,0 };
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc = sampleDesc;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	device->CreateTexture2D(&texDesc, NULL, &texture);

	//create the render target view 
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(texture, &rtvDesc, &dynamicTexRTV);

	//create the shader resource view that will bind to our effect for the texture 
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(texture, &srvDesc, &dynamicTexSRV);

	//create the depth stencil view 
	//create depth/stencil state
	depthTexDesc = {
		(UINT)width,
		(UINT)height,
		1,
		1,
		DXGI_FORMAT_D32_FLOAT,
		sampleDesc,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_DEPTH_STENCIL,
		0,
		0
	};
	device->CreateTexture2D(&depthTexDesc, 0, &depthTex);
	dsvDesc = {
		depthTexDesc.Format,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0,
	};

	device->CreateDepthStencilView(depthTex, &dsvDesc, &dynamicTexDSV);
}

void Reflection::SetRenderTarget()
{
	context->OMSetRenderTargets(1, &dynamicTexRTV, dynamicTexDSV);
}

void Reflection::ClearRenderTarget(float color[4])
{
	context->ClearRenderTargetView(dynamicTexRTV, color);
	context->ClearDepthStencilView(dynamicTexDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Reflection::ResetRenderTargets() {
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	context->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
}

ID3D11RenderTargetView * Reflection::GetRTV()
{
	return dynamicTexRTV;
}

ID3D11ShaderResourceView * Reflection::GetShaderResourceView()
{
	return dynamicTexSRV;
}

void Reflection::Release()
{
	if(dynamicTexDSV != nullptr) dynamicTexDSV->Release();
	if(dynamicTexRTV != nullptr) dynamicTexRTV->Release();
	if(dynamicTexSRV != nullptr) dynamicTexSRV->Release();
	if(texture != nullptr) texture->Release();
	if(depthTex != nullptr) depthTex->Release();
}

Reflection::~Reflection()
{
}
