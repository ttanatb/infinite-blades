#include "LiveReflectionTestScene.h"

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
LiveReflectionTestScene::LiveReflectionTestScene(HINSTANCE hInstance)

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
LiveReflectionTestScene::~LiveReflectionTestScene()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	for (size_t i = 0; i < gameEntities.size(); ++i)
		delete gameEntities[i];
	if (camera != nullptr) delete camera;
	if (skybox != nullptr) delete skybox;

	MaterialManager::ReleaseInstance();
	MeshManager::ReleaseInstance();
	InputManager::ReleaseInstance();
	ShaderManager::ReleaseInstance();
	RenderManager::ReleaseInstance();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void LiveReflectionTestScene::Init()
{
	LoadShaderMeshMat();
	CreateEntities();
	//intialize render manager
	renderMngr = RenderManager::GetInstance();
	renderMngr->Init(device, context, backBufferRTV, depthStencilView);
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

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LiveReflectionTestScene::AddEntityToRender()
{
	for (int i = 0; i < gameEntities.size(); ++i)
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
}

// --------------------------------------------------------
// Loads shaders, loads texture to build materials, and loads meshes
// then adds them to the respective managers
// --------------------------------------------------------
void LiveReflectionTestScene::LoadShaderMeshMat()
{
	//shaders
	shaderMngr = ShaderManager::GetInstancce();
	shaderMngr->Init(device, context);
	shaderMngr->AddVertexShader("vBasic");
	shaderMngr->AddPixelShader("pBasic");
	shaderMngr->AddVertexShader("SkyBoxVS");
	shaderMngr->AddPixelShader("SkyBoxPS");
	shaderMngr->AddVertexShader("vReflection");
	shaderMngr->AddPixelShader("pReflection");

	//hoisting shaders
	SimpleVertexShader* vShader = shaderMngr->GetVertexShader("vBasic");
	SimplePixelShader* pShader = shaderMngr->GetPixelShader("pBasic");
	SimpleVertexShader* vReflectShader = shaderMngr->GetVertexShader("vReflection");
	SimplePixelShader* pReflectShader = shaderMngr->GetPixelShader("pReflection");

	//materials
	matMngr = MaterialManager::GetInstancce();
	matMngr->Init(device, context);
	matMngr->AddMat("ship", vShader, pShader, L"Assets/Textures/shipAlbedo.png");
	matMngr->AddMat("ice", vReflectShader, pReflectShader, L"Assets/Textures/ice.jpg", L"Assets/Textures/iceNormals.jpg", true, 0.650f, L"Assets/Textures/SunnyCubeMap.dds");
	matMngr->AddMat("snow", vShader, pShader, L"Assets/Textures/snow.jpg", L"Assets/Textures/snowNormals.jpg");

	//meshes
	meshMngr = MeshManager::GetInstancce();
	meshMngr->Init(device);
	meshMngr->AddMesh("cube", "Assets/Models/cube.obj");
	meshMngr->AddMesh("ship", "Assets/Models/ship.obj");
	meshMngr->AddMesh("floor", "Assets/Models/floor.obj");
	meshMngr->AddMesh("snow", "Assets/Models/snowFloor.obj");
}

void LiveReflectionTestScene::CreateEntities()
{
	//create camera
	camera = new Camera((float)width, (float)height, vec3(0.0f, 2.5f, 0.0f), 0.20f, 0.0f);

	for (int i = 0; i < 4; ++i) {
		gameEntities.push_back(new GameEntity(meshMngr->GetMesh("snow"), matMngr->GetMat("snow"),
			vec3(0, 0, 30.0f * static_cast<float>(i)), vec3(0, 0, 0), vec3(1, 1, 1)));
		gameEntities.push_back(new GameEntity(meshMngr->GetMesh("floor"), matMngr->GetMat("ice"),
			vec3(0, 0, 30.0f * static_cast<float>(i)), vec3(0, 0, 0), vec3(1, 1, 1)));
		gameEntities.push_back(new GameEntity(meshMngr->GetMesh("cube"), matMngr->GetMat("ship"),
			vec3(0, -5, 30.0f * static_cast<float>(i)), vec3(0, 0, 0), vec3(1, 1, 1)));
	}


	//create entities
	//gameEntities.push_back(new GameEntity(meshMngr->GetMesh("torus"), matMngr->GetMat("woodplanks"),
	//	vec3(2, 1, 1), vec3(45, 45, 0), 0.69f));

	//gameEntities.push_back(new GameEntity(meshMngr->GetMesh("cone"), matMngr->GetMat("concrete"),
	//	vec3(1, -1, 1), vec3(45, 90, 45), 0.9f));

	//gameEntities.push_back(new GameEntity(meshMngr->GetMesh("helix"), matMngr->GetMat("soil"),
	//	vec3(0, 0, 5), vec3(45, 0, 45), 0.85f));

	//gameEntities.push_back(new GameEntity(meshMngr->GetMesh("sphere"), matMngr->GetMat("woodplanks"),
	//	vec3(-1, 1, 0), vec3(45, 45, 90), 0.8f));

	//gameEntities.push_back(new GameEntity(meshMngr->GetMesh("torus"), matMngr->GetMat("soil"),
	//	vec3(1, 1, 1), vec3(45, 0, 45), vec3(0.7f, 0.6f, 0.8f)));

	player = new Player(meshMngr->GetMesh("ship"), matMngr->GetMat("ship"));
	gameEntities.push_back(player);

	skybox = new Skybox(L"Assets/Textures/SunnyCubeMap.dds",
		device,
		shaderMngr->GetVertexShader("SkyBoxVS"),
		shaderMngr->GetPixelShader("SkyBoxPS"),
		meshMngr->GetMesh("cube"));
}

void LiveReflectionTestScene::InitInput()
{
	inputMngr = InputManager::GetInstance();
	char* usedChars = "AD";
	inputMngr->AddKeysToPollFor(usedChars, strlen(usedChars));
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void LiveReflectionTestScene::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->SetWidthHeight((float)width, (float)height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void LiveReflectionTestScene::Update(float deltaTime, float totalTime)
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

	for (size_t i = 0; i < gameEntities.size() - 1; ++i) {
		gameEntities[i]->Update();
		gameEntities[i]->TranslateBy(0.0f, 0.0f, -0.01f);

		if (gameEntities[i]->GetPosition().z < 0.0f) {
			gameEntities[i]->TranslateBy(0.0f, 0.0f, 120.0f);
		}
	}

	player->Update(deltaTime, totalTime);

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void LiveReflectionTestScene::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	renderMngr->Draw();
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

	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void LiveReflectionTestScene::OnMouseDown(WPARAM buttonState, int x, int y)
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
void LiveReflectionTestScene::OnMouseUp(WPARAM buttonState, int x, int y)
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
void LiveReflectionTestScene::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	if (buttonState & 0x0001) {
		camera->RotateAroundUp((x - prevMousePos.x) / 1000.0f);
		camera->RotateAroundRight((y - prevMousePos.y) / 1000.0f);
	}


	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void LiveReflectionTestScene::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion