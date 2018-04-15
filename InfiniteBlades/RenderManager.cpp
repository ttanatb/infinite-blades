#include "RenderManager.h"


RenderManager* RenderManager::instance = nullptr;

RenderManager::RenderManager()
{
	opaqueObjects = std::vector<GameEntity*>();
	transparentObjects = std::vector<GameEntity*>();
	std::vector<DirectionalLight> directionalLightList = std::vector<DirectionalLight>();
	std::vector<PointLight> pointLightList = std::vector<PointLight>();
}

void RenderManager::DrawObjects(std::vector<GameEntity*> list, UINT stride, UINT offset, Camera* camera)
{
	for(size_t i = 0; i < list.size(); i++)
	{
		Mesh* meshPtr = list[i]->GetMesh();
		Material * matPtr = list[i]->GetMat();

		//early exit
		if (meshPtr == nullptr || matPtr == nullptr) continue;

		/*This is Per-frame data that we can offset into a renderer class we won't have*/
		SimplePixelShader* pixelShader = matPtr->GetPixelShader();
		pixelShader->SetFloat4("ambientColor", ambientLight);
		//Set Directiona Lights
		for (int i = 0; i < directionalLightList.size(); i++) {
			//set directional light name
			std::string dirLightName = "directionalLight";
			if (directionalLightList.size() > 1) {
				std::string dirLightName = "directionalLight" + std::to_string(i);
			}
			//send directional light data 
			pixelShader->SetData(dirLightName, &directionalLightList[i], sizeof(directionalLightList[i]));
		}
		//Set Point Lights 
		for (int i = 0; i < pointLightList.size(); i++) {
			std::string pointLightName = "pointLight";
			if (directionalLightList.size() > 1) {
				std::string dirLightName = "pointLight" + std::to_string(i);
			}
			pixelShader->SetData(pointLightName, &pointLightList[i], sizeof(pointLightList[i]));
		}
		pixelShader->SetFloat3("cameraPos", camera->GetPos());
		//set transparency strength
		pixelShader->SetFloat("transparentStrength", matPtr->GetTransparentStr());
		SimpleVertexShader* vertexShader = matPtr->GetVertexShader();
		vertexShader->SetMatrix4x4("view", *(camera->GetViewMatTransposed()));
		vertexShader->SetMatrix4x4("projection", *(camera->GetProjMatTransposed()));

		//prepare per-object data
		if (matPtr->GetTransparentBool())
		{
			matPtr->PrepareMaterialReflection(list[i]->GetWorldMat(), skybox->GetShaderResourceView());
		}
		else
		{
			matPtr->PrepareMaterial(list[i]->GetWorldMat());
		}
		ID3D11Buffer * vertexBuffer = meshPtr->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &(vertexBuffer), &stride, &offset);
		context->IASetIndexBuffer(meshPtr->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshPtr->GetIndexCount(), 0, 0);
	}
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
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	//turn transparency off
	context->OMSetBlendState(NULL, 0, 0xFFFFFFFF);
	//draw opaque object
	DrawObjects(opaqueObjects, stride, offset, camera);
	//draw skybox
	skybox->Render(context, camera, stride, offset);
	//turn transparency on
	context->OMSetBlendState(blendState, 0, 0xFFFFFFFF);
	SortOpqaue(transparentObjects, camera);
	DrawObjects(transparentObjects, stride, offset, camera);
}

void RenderManager::InitSkyBox(Skybox * skybox)
{
	this->skybox = skybox;
}

void RenderManager::InitCamera(Camera * camera)
{
	this->camera = camera;
}

void RenderManager::Init(ID3D11Device * device, ID3D11DeviceContext * context)
{
	this->device = device;
	this->context = context;
	InitBlendState();
}

void RenderManager::AddToTransparent(GameEntity* gameEntity)
{
	if(gameEntity != nullptr)
		transparentObjects.push_back(gameEntity);
}

void RenderManager::AddToOpqaue(GameEntity* gameEntity)
{
	if (gameEntity != nullptr)
		opaqueObjects.push_back(gameEntity);
}

void RenderManager::AddAmbientLight(vec4 ambientLight)
{
	this->ambientLight = ambientLight;
}

void RenderManager::AddDirectionalLight(DirectionalLight directionalLight)
{
	directionalLightList.push_back(directionalLight);
}

void RenderManager::AddPointLight(PointLight pointLight)
{
	pointLightList.push_back(pointLight);
}


RenderManager::~RenderManager()
{
	ReleaseBlendState();
}
