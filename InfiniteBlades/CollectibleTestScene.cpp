#include "CollectibleTestScene.h"

//remove later
#include <iostream>

// For the DirectX Math library
using namespace DirectX;

#if defined(DEBUG) || defined(_DEBUG)
//ImGui temp variables
static float pos[3] = { 0.0f, 0.0f, 0.0f }; // Slider for object positions
#endif
// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
CollectibleTestScene::CollectibleTestScene(HINSTANCE hInstance)

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
CollectibleTestScene::~CollectibleTestScene()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	if (camera != nullptr) delete camera;
	if (skybox != nullptr) delete skybox;
	if (player != nullptr) {
		delete player;
		player = nullptr;
	}

	ImGui_ImplDX11_Shutdown();

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
void CollectibleTestScene::Init()
{
	//Initialize ImGui
	ImGui_ImplDX11_Init(hWnd, device, context);
	renderMngr = RenderManager::GetInstance();
	LoadShaderMeshMat();
	CreateEntities();
	InitInput();
	renderMngr->Init(device, context);
	renderMngr->InitSkyBox(skybox);
	renderMngr->InitCamera(camera);
	AddEntityToRender();

	directionalLight = { vec4(0.8f, 0.85f, 0.9f, 1.0f),
		vec3(-0.2f, -1.0f, 0.3f) };
	ambientLight = vec4(0.1f, 0.1f, 0.2f, 1.0f);

	renderMngr->AddDirectionalLight("directionalLight", directionalLight);
	renderMngr->AddAmbientLight(ambientLight);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gameMngr->Init(device, context);
}

void CollectibleTestScene::AddEntityToRender()
{
	renderMngr->InitInstancedRendering(new GameEntity(meshMngr->GetMesh("stone"), matMngr->GetMat("snowPile"),
		vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 1, 1)), 50);

	renderMngr->AddToOpqaue(player);
	renderMngr->AddToOpqaue(gameMngr->GetCollectibleList());
	renderMngr->AddToOpqaue(gameMngr->GetObstacleList());
	renderMngr->AddToOpaqueAndTransparent(gameMngr->GetSceneryList());

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
void CollectibleTestScene::LoadShaderMeshMat()
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
	matMngr->AddMat("snow", vShader, pShader, L"Assets/Textures/snow.jpg", L"Assets/Textures/snowNormals.jpg");
	matMngr->AddMat("metal", vShader, pShader, L"Assets/Textures/coin.jpg", L"Assets/Textures/coinNormals.jpg");
	matMngr->AddMat("crate", vShader, pShader, L"Assets/Textures/crate_texture_color.jpg");
	matMngr->AddMat("snowPile", shaderMngr->GetVertexShader("vSnow"), shaderMngr->GetHullShader("hSnow"), shaderMngr->GetDomainShader("dSnow"), shaderMngr->GetPixelShader("pSnow"), L"Assets/Textures/stoneDiffuse.jpg", L"Assets/Textures/stoneNormals.jpg");

	matMngr->AddMat("ice",
		shaderMngr->GetVertexShader("vReflection"),
		shaderMngr->GetPixelShader("pReflection"),
		L"Assets/Textures/ice.jpg",
		L"Assets/Textures/iceNormals.jpg",
		true,
		0.80f,
		L"Assets/Textures/SunnyCubeMap.dds");
	//meshes
	meshMngr = MeshManager::GetInstancce();
	meshMngr->Init(device);
	meshMngr->AddMesh("cube", "Assets/Models/cube.obj");
	meshMngr->AddMesh("ship", "Assets/Models/ship.obj");
	meshMngr->AddMesh("floor", "Assets/Models/floor.obj");
	meshMngr->AddMesh("snow", "Assets/Models/snowFloor.obj");
	meshMngr->AddMesh("crate", "Assets/Models/crate.obj");
	meshMngr->AddMesh("stone", "Assets/Models/stone.obj");
	meshMngr->AddMesh("coin", "Assets/Models/coin.obj");
}

void CollectibleTestScene::CreateEntities()
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

	player = new Player(meshMngr->GetMesh("ship"), matMngr->GetMat("ship"), ColliderType::SPHERE); //gameEntity[8]
	player->CalculateCollider();
	gameMngr->SetPlayer(player);
	//gameEntities.push_back(player);

	// Collectibles
	float lanes[3] = { -2.f, 0.0f, 2.f };
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

	skybox = new Skybox(L"Assets/Textures/SunnyCubeMap.dds",
		device,
		shaderMngr->GetVertexShader("SkyBoxVS"),
		shaderMngr->GetPixelShader("SkyBoxPS"),
		meshMngr->GetMesh("cube"));
}

void CollectibleTestScene::InitInput()
{
	inputMngr = InputManager::GetInstance();
	char* usedChars = "AD";
	inputMngr->AddKeysToPollFor(usedChars, strlen(usedChars));
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void CollectibleTestScene::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->SetWidthHeight((float)width, (float)height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void CollectibleTestScene::Update(float deltaTime, float totalTime)
{
	//inputMngr->Update();
	//ImGuiIO& io = ImGui::GetIO();
	//io.DeltaTime = deltaTime;
	//All Imgui UI stuff must be done between imgui newFrame() and imgui render()
	//ImGui_ImplDX11_NewFrame();
	inputMngr->Update();

	camera->Update();
	//player->SetPosition(pos[0], pos[1], pos[2]);

	gameMngr->UpdateWorld(deltaTime);
	player->Update(deltaTime, totalTime);

	gameMngr->ResolveCollectibleCollision();
	gameMngr->ResolveObstacleCollision();

	// Coin Pickups
	// Loop Coins
	/*
	if (gameEntities[11]->GetPosition().z < 0)
	{
		// Reset Position
		gameEntities[9]->SetPosition(vec3(0, 1, gameEntities[9]->GetPosition().z));
		gameEntities[10]->SetPosition(vec3(0, 1, gameEntities[10]->GetPosition().z));
		gameEntities[11]->SetPosition(vec3(0, 1, gameEntities[11]->GetPosition().z));
		gameEntities[12]->SetPosition(vec3(0, 1, gameEntities[12]->GetPosition().z));
		gameEntities[13]->SetPosition(vec3(0, 1, gameEntities[13]->GetPosition().z));

		// Reset Collected
		for (int i = 9; i < 13; i++)
		{
			if (collected[i - 9].to_ulong() == 1)
			{
				collected[i - 9].set(0, 0);
			}
		}
	}
	*/

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void CollectibleTestScene::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	renderMngr->Draw();


	// At the end of the frame, reset render states
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);

	// 1. Show a simple window
	//{
	//	static float f = 0.0f;
	//	ImGui::Text("Collision Test Debugger");
	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0 to 1
	//	ImGui::SliderFloat3("Player Pos", pos, -20, 20); // Edit 3 floats using a slider from -5 to 5
	//	ImGui::ColorEdit3("clear color", (float*)&clear_color);
	//	ImGui::Checkbox("Free Look Enabled", &freelookEnabled);
	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//}
	//ImGui::Render();

	gameMngr->DrawUI(this->height);

	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void CollectibleTestScene::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	//ImGuiIO& io = ImGui::GetIO();

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
void CollectibleTestScene::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	ImGuiIO& io = ImGui::GetIO();

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void CollectibleTestScene::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	//ImGuiIO& io = ImGui::GetIO();
	//io.MousePos = ImVec2((float)x, (float)y);

	if (buttonState && 0x0001)
	{
		camera->RotateAroundUp((x - prevMousePos.x) / 1000.0f);
		camera->RotateAroundRight((y - prevMousePos.y) / 1000.0f);

		// Save the previous mouse position, so we have it for the future
		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void CollectibleTestScene::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion