#include "ReflectionCubeMap.h"



ReflectionCubeMap::ReflectionCubeMap(ID3D11Device* device, ID3D11DeviceContext* context, float width, float height, Skybox* skybox)
{
	this->device = device;
	this->context = context;
	this->width = width;
	this->height = height;
	this->skybox = skybox;
}


ReflectionCubeMap::~ReflectionCubeMap()
{
	//release, now that it is it saved to the views 
	texture->Release();
	depthTex->Release();
	dynamicCubeMapSRV->Release();
	dynamicCubeDSV->Release();
	for (int i = 0; i < 6; i++) {
		dynamicCubeRTV[i]->Release();
		delete cubeMapCamera[i];
	}
}

void ReflectionCubeMap::BuildDynamicCubeMapView()
{
	//Create the render target cube map texture
	DXGI_SAMPLE_DESC sampleDesc{ 1,0 };
	texDesc.Width = cubeMapSize;
	texDesc.Height = cubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc = sampleDesc;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	device->CreateTexture2D(&texDesc, 0, &texture);
	//create the render target view array
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;
	for (int i = 0; i < 6; i++) {
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		device->CreateRenderTargetView(texture, &rtvDesc, &dynamicCubeRTV[i]);
	}
	//create the shader resource view that will bind to our effect for the cube map 
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = -1;
	srvDesc.TextureCube.MostDetailedMip = 0;
	device->CreateShaderResourceView(texture, &srvDesc, &dynamicCubeMapSRV);
	//create depth/stencil state
	depthTexDesc = {
		(UINT)cubeMapSize,
		(UINT)cubeMapSize,
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
	dsvDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(depthTex, &dsvDesc, &dynamicCubeDSV);

	//create the view port for rendering the cubemap faces
	cubeMapViewPort = {
		0.0f,
		0.0f,
		(float)cubeMapSize,
		(float)cubeMapSize,
		0.0f,
		1.0f
	};
}

void ReflectionCubeMap::BuildCubeFaceCamera(float x, float y, float z)
{
	CalcCameras(x, y, z);
	for (int i = 0; i < 6; i++) {
		cubeMapCamera.push_back(new Camera(width, height, targets[i], ups[i].y, ups[i].z));
		cubeMapCamera[i]->Update();
	}
}

void ReflectionCubeMap::UpdateCubeFaceCamera(float x, float y, float z)
{
	CalcCameras(x, y, z);
	for (int i = 0; i < 6; i++) {
		cubeMapCamera[i]->Update();
	}
}

void ReflectionCubeMap::CalcCameras(float x, float y, float z)
{
	targets = {
		vec3(x + 1, y, z),
		vec3(x - 1, y, z),
		vec3(x, y + 1, z),
		vec3(x, y - 1, z),
		vec3(x, y, z - 1),
		vec3(x, y, z - 1),
	};

	ups = {
		vec3(0, 1, 0),
		vec3(0, 1, 0),
		vec3(0, 0, -1),
		vec3(0, 0, 1),
		vec3(0, 1, 0),
		vec3(0, 1, 0),
	};
}

void ReflectionCubeMap::RenderCubeMap()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	//save the old viewport 
	context->RSGetViewports(&numOfPreviousPorts, &previousViewport);
	//get old render target 
	context->OMGetRenderTargets(1, &previousRenderTarget, &previousDSV);
	//set new view port
	context->RSSetViewports(1, &cubeMapViewPort);
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	for (int i = 0; i < 6; i++) {
		context->ClearRenderTargetView(dynamicCubeRTV[i], color);
		context->ClearDepthStencilView(
			dynamicCubeDSV,
			D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH,
			1.0f,
			0
		);
		context->OMSetRenderTargets(1, &dynamicCubeRTV[i], dynamicCubeDSV);
		skybox->Render(context, cubeMapCamera[i], stride, offset);
	}
	//restore old view ports
	context->RSSetViewports(numOfPreviousPorts, &previousViewport);
	context->OMSetRenderTargets(1, &previousRenderTarget, previousDSV);
	//generate mip chain for cube map
	context->GenerateMips(dynamicCubeMapSRV);
	context->ClearRenderTargetView(previousRenderTarget, color);
	context->ClearDepthStencilView(previousDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	previousRenderTarget->Release();
	previousDSV->Release();
}

ID3D11ShaderResourceView* ReflectionCubeMap::GetShaderResourceView()
{
	return dynamicCubeMapSRV;
}
