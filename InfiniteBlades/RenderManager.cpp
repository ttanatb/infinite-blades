#include "RenderManager.h"


RenderManager* RenderManager::instance = nullptr;

RenderManager::RenderManager()
{
	opaqueObjects = std::vector<GameEntity*>();
	transparentObjects = std::vector<GameEntity*>();
	std::vector<DirectionalLight> directionalLightList = std::vector<DirectionalLight>();
	std::vector<PointLight> pointLightList = std::vector<PointLight>();
}

void RenderManager::DrawObjects(std::vector<GameEntity*> list, UINT stride, UINT offset, Camera* camera, mat4 ViewMatrix)
{
	for(size_t i = 0; i < list.size(); ++i)
	{
		Mesh* meshPtr = list[i]->GetMesh();
		Material * matPtr = list[i]->GetMat();

		//early exit
		if (meshPtr == nullptr || matPtr == nullptr) continue;
		SimplePixelShader* pixelShader = matPtr->GetPixelShader();
		pixelShader->SetFloat4("ambientColor", ambientLight);
		//Set Directiona Lights
		for (auto& light : directionaLightMap) 
		{
			pixelShader->SetData(light.first, &light.second, sizeof(light.second));
		}
		//Set Point Lights 
		for (auto& light : pointLightMap) 
		{
			pixelShader->SetData(light.first, &light.second, sizeof(light.second));
		}
		pixelShader->SetFloat3("cameraPos", camera->GetPos());
		//set transparency strength
		pixelShader->SetFloat("transparentStrength", matPtr->GetTransparentStr());
		SimpleVertexShader* vertexShader = matPtr->GetVertexShader();
		vertexShader->SetMatrix4x4("view", ViewMatrix);
		vertexShader->SetMatrix4x4("projection", *(camera->GetProjMatTransposed()));
		if(matPtr->GetTransparentBool())
		{
			matPtr->SetReflectionSRV(reflectionTex->GetShaderResourceView());
			matPtr->PreparePlanarReflectionMaterial(list[i]->GetWorldMat(), camera->GetReflectionMat());
		}
		else
		{
			matPtr->PrepareMaterial(list[i]->GetWorldMat());
		}
		//get vertex buffer
		ID3D11Buffer * vertexBuffer = meshPtr->GetVertexBuffer();
		//set index buffer
		context->IASetVertexBuffers(0, 1, &(vertexBuffer), &stride, &offset);
		context->IASetIndexBuffer(meshPtr->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshPtr->GetIndexCount(), 0, 0);
	}
}

void RenderManager::DrawAllOpaque(Camera * camera, mat4 viewMatrix)
{
	//turn transparency off
	context->OMSetBlendState(NULL, 0, 0xFFFFFFFF);
	//draw opaque object
	DrawObjects(aboveGroundObjects, stride, offset, camera, viewMatrix);
	//draw skybox
	skybox->Render(context, camera, stride, offset);
	//// At the end of the frame, reset render states
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

void RenderManager::InitBlendState()
{
	//blend description 
	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;

	// These control the RGB channels
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	// This is for the alpha channel specifically
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//create blend states
	device->CreateBlendState(&blendDesc, &blendState);
}

//helper function to get distance to camera squared
float distanceToCameraSqr(GameEntity* gameEntity, Camera* camera) {
	return pow(gameEntity->GetPosition().x - camera->GetPos().x, 2) + 
		   pow(gameEntity->GetPosition().y - camera->GetPos().y, 2) +
		   pow(gameEntity->GetPosition().z - camera->GetPos().z, 2);
}

void RenderManager::SortOpqaue(std::vector<GameEntity*> list, Camera* camera)
{
	std::sort(list.begin(), list.end(), 
		[&, camera](GameEntity* a, GameEntity* b) {
			return (distanceToCameraSqr(a, camera) > distanceToCameraSqr(b, camera));
		}
	);
}

void RenderManager::RenderReflectionTexture()
{
	//mat4 reflectionMat;
	float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	//get old render target 
	context->OMGetRenderTargets(1, &previousRenderTarget, &previousDSV);
	//set to reflection render target 
	reflectionTex->SetRenderTarget();
	//clear current reflection render target
	reflectionTex->ClearRenderTarget(color);
	//calc reflection matrix
	camera->CalcReflectionMat(-10.0f);
	//draw opaque objects upside down 
	DrawAllOpaque(camera, *(camera->GetReflectionMat()));
	//reset render target 
	context->OMSetRenderTargets(1, &previousRenderTarget, previousDSV);
	previousRenderTarget->Release();
	previousDSV->Release();
}

void RenderManager::ReleaseReflectionTexture()
{
	reflectionTex->Release();
	delete reflectionTex;
}

RenderManager * RenderManager::GetInstance()
{
	if (instance == nullptr)
		instance = new RenderManager();
	return instance;
}

void RenderManager::ReleaseInstance()
{
	if (instance != nullptr)
		delete instance;
}

void RenderManager::ReleaseBlendState()
{
	if (blendState != nullptr) 
		blendState->Release();
}

void RenderManager::Draw()
{
	RenderReflectionTexture();
	//turn transparency off
	context->OMSetBlendState(NULL, 0, 0xFFFFFFFF);
	//draw opaque object
	DrawObjects(opaqueObjects, stride, offset, camera, *(camera->GetViewMatTransposed()));
	//draw skybox
	skybox->Render(context, camera, stride, offset);
	//turn transparency on
	context->OMSetBlendState(blendState, 0, 0xFFFFFFFF);
	SortOpqaue(transparentObjects, camera);
	DrawObjects(transparentObjects, stride, offset, camera, *(camera->GetViewMatTransposed()));
	//// At the end of the frame, reset render states, shader resource views, depth stencil
	ID3D11ShaderResourceView* nothing[16] = {};
	context->PSSetShaderResources(0, 16, nothing);
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

void RenderManager::InitSkyBox(Skybox * skybox)
{
	this->skybox = skybox;
}

void RenderManager::InitCamera(Camera * camera)
{
	this->camera = camera;
	reflectionTex = new Reflection(device, context, camera->GetHeight(), camera->GetWidth());
	reflectionTex->Init();
}

void RenderManager::Init(ID3D11Device * device, ID3D11DeviceContext * context)
{
	this->device = device;
	this->context = context;
	InitBlendState();
}

void RenderManager::AddToTransparent(GameEntity* gameEntity)
{
	transparentObjects.push_back(gameEntity);
}

void RenderManager::AddToOpqaue(GameEntity* gameEntity)
{
	opaqueObjects.push_back(gameEntity);
}

void RenderManager::AddToReflectionRender(GameEntity * gameEntity)
{
	aboveGroundObjects.push_back(gameEntity);
}

void RenderManager::AddAmbientLight(vec4 ambientLight)
{
	this->ambientLight = ambientLight;
}

void RenderManager::AddDirectionalLight(char* name, DirectionalLight directionalLight)
{
	directionaLightMap.insert(std::pair<char*, DirectionalLight>(name, directionalLight));
}

void RenderManager::AddPointLight(char* name, PointLight pointLight)
{
	pointLightMap.insert(std::pair<char*, PointLight>(name, pointLight));
}


RenderManager::~RenderManager()
{
	ReleaseReflectionTexture();
	ReleaseBlendState();

	//if (alternateRTV) alternateRTV->Release();
	//if (alternateSRV) alternateSRV->Release();
}
