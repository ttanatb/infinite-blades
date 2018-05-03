#include "Refraction.h"



Refraction::Refraction(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height)
{
	this->device = device;
	this->context = context;
	this->width = width;
	this->height = height;
}


Refraction::~Refraction()
{
}

void Refraction::Init()
{
	// Setup the alternate drawing screen for refraction

	// Setup alternate Render Texture
	ID3D11Texture2D* alternateRenderTexture;
	D3D11_TEXTURE2D_DESC altRenTexDesc = {};
	altRenTexDesc.Width = width;
	altRenTexDesc.Height = height;
	altRenTexDesc.MipLevels = 1;
	altRenTexDesc.ArraySize = 1;
	altRenTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	altRenTexDesc.Usage = D3D11_USAGE_DEFAULT;
	altRenTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	altRenTexDesc.CPUAccessFlags = 0;
	altRenTexDesc.MiscFlags = 0;
	altRenTexDesc.SampleDesc.Count = 1;
	altRenTexDesc.SampleDesc.Quality = 0;
	device->CreateTexture2D(&altRenTexDesc, 0, &alternateRenderTexture);


	// Setup alternate RTV
	D3D11_RENDER_TARGET_VIEW_DESC alternateRTVDesc = {};
	alternateRTVDesc.Format = altRenTexDesc.Format;
	alternateRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	alternateRTVDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(alternateRenderTexture, &alternateRTVDesc, &alternateRTV);

	// Release the temporary texture setup
	alternateRenderTexture->Release();

	// Setup a clamp sampler for refraction
	D3D11_SAMPLER_DESC altSampDesc = {};
	altSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	altSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	altSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	altSampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	altSampDesc.MaxAnisotropy = 16;
	altSampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the sampler object
	device->CreateSamplerState(&altSampDesc, &alternateSampler);

	// Tell input assembler to draw triangles (THIS COULD BE A PROBLEM LINE)
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Refraction::Release()
{
	// If they exist, release the Refraction-specific things
	if (alternateSampler) alternateSampler->Release();
	if (alternateRTV) alternateRTV->Release();
	if (alternateSRV) alternateSRV->Release();
}

ID3D11ShaderResourceView * Refraction::GetSRV()
{
	return nullptr;
}

ID3D11RenderTargetView * Refraction::GetRTV()
{
	return nullptr;
}
