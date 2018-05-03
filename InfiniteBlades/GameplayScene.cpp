#include "GameplayScene.h"

//remove later
#include <iostream>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
GameplayScene::GameplayScene(HINSTANCE hInstance)

//feed the handle, the name, width, and height
	: DXCore(hInstance, "Infinite Blades", 1280, 720,

		//show debug states only in debug mode
#if defined(DEBUG) || defined(_DEBUG)
		true)
#else
	false)
#endif
{
	// Initialize fields
	camera = nullptr;
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
GameplayScene::~GameplayScene()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	if (camera != nullptr) delete camera;
	if (skybox != nullptr) delete skybox;
	if (snowEmitter != nullptr) delete snowEmitter;
	if (player != nullptr) {
		delete player;
		player = nullptr;
	}

	MaterialManager::ReleaseInstance();
	MeshManager::ReleaseInstance();
	InputManager::ReleaseInstance();
	ShaderManager::ReleaseInstance();
	RenderManager::ReleaseInstance();
	GameManager::ReleaseInstance();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void GameplayScene::Init()
{
	LoadShaderMeshMat();
	CreateEntities();
	//intialize render manager
	renderMngr = RenderManager::GetInstance();
	renderMngr->Init(device, context);
	renderMngr->InitSkyBox(skybox);
	renderMngr->InitCamera(camera);
	AddEntityToRender();
	//intialize input
	InitInput();


	directionalLight = { vec4(0.2f, 0.3f, 0.2f, 1.0f),
		vec3(1.0f, 1.0f, 0.0f) };
	directionalLight2 = { vec4(0.2f, 0.2f, 0.3f, 1.0f),
		vec3(-1.0f, -2.0f, 0.0f) };
	pointLight = { vec4(0.2f, 0.2f, 0.05f, 1.0f),
		vec3(0.0f, 5.0f, 10.0f) };
	ambientLight = vec4(0.1f, 0.1f, 0.1f, 1.0f);

	renderMngr->AddDirectionalLight("directionalLight", directionalLight);
	renderMngr->AddDirectionalLight("directionalLight2", directionalLight2);
	renderMngr->AddPointLight("pointLight", pointLight);
	renderMngr->AddAmbientLight(ambientLight);

	gameMngr->Init(device, context);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void GameplayScene::AddEntityToRender()
{
	renderMngr->InitInstancedRendering(new GameEntity(meshMngr->GetMesh("stone"), matMngr->GetMat("snowPile"),
		vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 1, 1)), 50);

	renderMngr->AddToOpqaue(player);
	renderMngr->AddToOpqaue(gameMngr->GetCollectibleList());
	renderMngr->AddToOpqaue(gameMngr->GetObstacleList());
	renderMngr->AddToOpaqueAndTransparent(gameMngr->GetSceneryList());
	renderMngr->AddToOpqaue(gameMngr->GetFishList());

	renderMngr->AddToReflectionRender(player);
	renderMngr->AddToReflectionRender(gameMngr->GetCollectibleList());
	/*
	for (int i = 0; i < gameEntities.size(); i++)
	{
		if (gameEntities[i]->GetMat()->GetTransparentBool()) {
			renderMngr->AddToTransparent(gameEntities[i]);
		}
		else
		{
			renderMngr->AddToOpqaue(gameEntities[i]);
		}
	}
	renderMngr->AddToReflectionRender(player);
	*/
}

// --------------------------------------------------------
// Loads shaders, loads texture to build materials, and loads meshes
// then adds them to the respective managers
// --------------------------------------------------------
void GameplayScene::LoadShaderMeshMat()
{
	//shaders
	shaderMngr = ShaderManager::GetInstancce();
	shaderMngr->Init(device, context);
	shaderMngr->AddVertexShader("vBasic");
	shaderMngr->AddPixelShader("pBasic");
	shaderMngr->AddVertexShader("SkyBoxVS");
	shaderMngr->AddPixelShader("SkyBoxPS");
	shaderMngr->AddVertexShader("vParticle");
	shaderMngr->AddPixelShader("pParticle");
	shaderMngr->AddVertexShader("vReflection");
	shaderMngr->AddPixelShader("pReflection");
	shaderMngr->AddVertexShader("vSnow");
	shaderMngr->AddPixelShader("pSnow");
	shaderMngr->AddHullShader("hSnow");
	shaderMngr->AddDomainShader("dSnow");

	//hoisting shaders
	SimpleVertexShader* vShader = shaderMngr->GetVertexShader("vBasic");
	SimplePixelShader* pShader = shaderMngr->GetPixelShader("pBasic");

	//materials
	matMngr = MaterialManager::GetInstancce();
	matMngr->Init(device, context);
	matMngr->AddMat("ship", vShader, pShader, L"Assets/Textures/shipAlbedo.png");
	matMngr->AddMat("goldfish", vShader, pShader, L"Assets/Textures/goldfish_albedo.jpg");
	matMngr->AddMat("shark", vShader, pShader, L"Assets/Textures/shark_albedo.png");
	matMngr->AddMat("snow", vShader, pShader, L"Assets/Textures/snow.jpg", L"Assets/Textures/snowNormals.jpg");
	matMngr->AddMat("snowPile", shaderMngr->GetVertexShader("vSnow"), shaderMngr->GetHullShader("hSnow"), shaderMngr->GetDomainShader("dSnow"), shaderMngr->GetPixelShader("pSnow"), L"Assets/Textures/stoneDiffuse.jpg", L"Assets/Textures/stoneNormals.jpg");
	matMngr->AddMat("metal", vShader, pShader, L"Assets/Textures/coin.jpg", L"Assets/Textures/coinNormals.jpg");
	matMngr->AddMat("crate", vShader, pShader, L"Assets/Textures/crate_texture_color.jpg");

	matMngr->AddMat("ice", 
		shaderMngr->GetVertexShader("vReflection"),
		shaderMngr->GetPixelShader("pReflection"),
		L"Assets/Textures/ice.jpg", 
		L"Assets/Textures/iceNormals.jpg", 
		true, 
		0.80f, 
		L"Assets/Textures/SkyCubeMap.dds");
	
	matMngr->AddMat("particle", 
		shaderMngr->GetVertexShader("vParticle"),
		shaderMngr->GetPixelShader("pParticle"),
		L"Assets/Textures/white_circle.png");

	//meshes
	meshMngr = MeshManager::GetInstancce();
	meshMngr->Init(device); 
	meshMngr->AddMesh("cube", "Assets/Models/cube.obj");
	meshMngr->AddMesh("ship", "Assets/Models/ship.obj");
	meshMngr->AddMesh("floor", "Assets/Models/floor.obj");
	meshMngr->AddMesh("snow", "Assets/Models/snowFloor.obj");
	meshMngr->AddMesh("goldfish", "Assets/Models/goldfish.obj");
	meshMngr->AddMesh("crate", "Assets/Models/crate.obj");
	meshMngr->AddMesh("coin", "Assets/Models/coin.obj");
	meshMngr->AddMesh("shark", "Assets/Models/shark.obj");
	meshMngr->AddMesh("stone", "Assets/Models/stone.obj");
}

void GameplayScene::CreateEntities()
{
	//create camera
	camera = new Camera((float)width, (float)height, vec3(0.0f, 2.5f, 0.0f), 0.20f, 0.0f);
	gameMngr = GameManager::GetInstancce();

	for (int i = 0; i < 4; ++i)
	{
		gameMngr->AddToScenery(new GameEntity(meshMngr->GetMesh("snow"), matMngr->GetMat("snow"),
			vec3(0, 0, 30.0f * static_cast<float>(i)), vec3(0, 0, 0), vec3(1, 1, 1)));
		gameMngr->AddToScenery(new GameEntity(meshMngr->GetMesh("floor"), matMngr->GetMat("ice"),
			vec3(0, 0, 30.0f * static_cast<float>(i)), vec3(0, 0, 0), vec3(1, 1, 1)));
	}

	//for (int i = 0; i < 4; i++) {
	//	gameEntities.push_back(new GameEntity(meshMngr->GetMesh("goldfish"), matMngr->GetMat("goldfish"),
	//		vec3(1, -1, 30.0f * static_cast<float>(i)), vec3(0, 0, 0), vec3(1, 1, 1)));
	//	gameEntities.push_back(new GameEntity(meshMngr->GetMesh("shark"), matMngr->GetMat("shark"),
	//		vec3(-1, -1, 30.0f * static_cast<float>(i)), vec3(0, 0, 0), vec3(1, 1, 1)));
	//}

	player = new Player(meshMngr->GetMesh("ship"), matMngr->GetMat("ship"), ColliderType::SPHERE); //gameEntity[8]
	player->CalculateCollider();
	gameMngr->SetPlayer(player);

	snowEmitter = new Emitter(
		device,								//device
		matMngr->GetMat("particle"),		//material
		vec4(0.85f, 0.95f, 1.00f, 1.0f),	//start color
		vec4(0.85f, 0.95f, 1.00f, 0.5f),	//end color
		vec3(0.f, -3.f, -2.f),				//velocity
		0.05f,								//start size
		0.05f,								//end size
		4,									//lifetime (in seconds)
		true,								//is loopable
		true,								//is active
		400,								//max amount of particles
		100,								//emissions per second (for smooth emission, use maxParticles/lifetime)
		vec3(0, 5, 0)						//position
	);
	snowEmitter->SetAsPlane(10, 20);		//Sets as a (horizontal) plane (width, depth), will emit as point otherwise

	float lanes[3] = { -2.5f, 0.0f, 2.f };
	for (int i = 0; i < 5; ++i)
	{
		float x = lanes[rand() % 3];
		GameEntity* collectible = new GameEntity(meshMngr->GetMesh("coin"), matMngr->GetMat("metal"), ColliderType::SPHERE,
			vec3(x, 0.75f, 25.0f * static_cast<float>(i) + 22.5f), vec3(0, 0, 0.0f), vec3(1.f, 1.f, 1.f));
		collectible->CalculateCollider();
		gameMngr->AddToCollectible(collectible);
	}

	for (int i = 0; i < 5; ++i)
	{
		float x = lanes[rand() % 3];
		GameEntity* obstacle = new GameEntity(meshMngr->GetMesh("crate"), matMngr->GetMat("crate"), ColliderType::SPHERE,
			vec3(x, 1.f, 25.0f * static_cast<float>(i) + 32.5f), vec3(0, 0, 0.0f), vec3(1.f, 1.f, 1.f));
		obstacle->CalculateCollider();
		gameMngr->AddToObstacle(obstacle);
	}

	gameMngr->AddToFish(new GameEntity(meshMngr->GetMesh("goldfish"), matMngr->GetMat("goldfish"),
		vec3(-7, -1, 7.0f), vec3(0, 0, 0), vec3(1, 1, 1)));
	gameMngr->AddToFish(new GameEntity(meshMngr->GetMesh("shark"), matMngr->GetMat("shark"),
		vec3(-12, -1, 5.0f), vec3(0, 0, 0), vec3(1, 1, 1)));

	skybox = new Skybox(L"Assets/Textures/SkyCubeMap.dds", 
		device, 
		shaderMngr->GetVertexShader("SkyBoxVS"),
		shaderMngr->GetPixelShader("SkyBoxPS"),
		meshMngr->GetMesh("cube"));
}

void GameplayScene::InitInput()
{
	inputMngr = InputManager::GetInstance();
	char* usedChars = "AD";
	inputMngr->AddKeysToPollFor(usedChars, strlen(usedChars));
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void GameplayScene::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->SetWidthHeight((float)width, (float)height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void GameplayScene::Update(float deltaTime, float totalTime)
{
	inputMngr->Update();

	/*
	if (inputMngr->GetKey('A'))
		camera->MoveAlongRight(-0.01f);
	if (inputMngr->GetKey('D'))
		camera->MoveAlongRight(+0.01f);

	if (inputMngr->GetKey('W'))
		camera->MoveAlongForward(+0.01f);
	if (inputMngr->GetKey('S'))
		camera->MoveAlongForward(-0.01f);

	if (inputMngr->GetKey(' '))
		camera->Move(0, +0.01f, 0);
	if (inputMngr->GetKey('X'))
		camera->Move(0, -0.01f, 0);
	*/
	camera->Update();

	gameMngr->UpdateWorld(deltaTime);
	player->Update(deltaTime, totalTime);

	gameMngr->ResolveCollectibleCollision();
	gameMngr->ResolveObstacleCollision();

	snowEmitter->Update(deltaTime);

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void GameplayScene::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	renderMngr->Draw();
	snowEmitter->RenderParticles(context, camera);
	//UINT stride = sizeof(Vertex);
	//UINT offset = 0;

	////draw all the entities
	//for (size_t i = 0; i < gameEntities.size(); ++i)
	//{
	//	//hoist the mesh and mat of the entity
	//	Mesh* meshPtr = gameEntities[i]->GetMesh();
	//	Material * matPtr = gameEntities[i]->GetMat();

	//	//early exit
	//	if (meshPtr == nullptr || matPtr == nullptr) continue;

	//	/*This is Per-frame data that we can offset into a renderer class we won't have*/
	//	SimplePixelShader* pixelShader = matPtr->GetPixelShader();
	//	pixelShader->SetFloat4("ambientColor", ambientLight);
	//	pixelShader->SetData("directionalLight", &directionalLight, sizeof(DirectionalLight));
	//	pixelShader->SetData("directionalLight2", &directionalLight2, sizeof(DirectionalLight));
	//	pixelShader->SetData("pointLight", &pointLight, sizeof(PointLight));
	//	pixelShader->SetFloat3("cameraPos", camera->GetPos());

	//	SimpleVertexShader* vertexShader = matPtr->GetVertexShader();
	//	vertexShader->SetMatrix4x4("view", *(camera->GetViewMatTransposed()));
	//	vertexShader->SetMatrix4x4("projection", *(camera->GetProjMatTransposed()));

	//	//prepare per-object data
	//	matPtr->PrepareMaterial(gameEntities[i]->GetWorldMat());

	//	ID3D11Buffer * vertexBuffer = meshPtr->GetVertexBuffer();
	//	context->IASetVertexBuffers(0, 1, &(vertexBuffer), &stride, &offset);
	//	context->IASetIndexBuffer(meshPtr->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	//	context->DrawIndexed(meshPtr->GetIndexCount(), 0, 0);
	//}

	////render skybox
	//skybox->Render(context, camera, stride, offset);
	gameMngr->DrawUI(this->height);

	//// At the end of the frame, reset render states
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);

	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void GameplayScene::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	//left click


	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void GameplayScene::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void GameplayScene::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	
	//if (buttonState & 0x0001) {
	//	camera->RotateAroundUp((x - prevMousePos.x) / 1000.0f);
	//	camera->RotateAroundRight((y - prevMousePos.y) / 1000.0f);
	//}
	

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void GameplayScene::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion