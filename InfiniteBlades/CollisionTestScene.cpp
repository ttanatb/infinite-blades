#include "CollisionTestScene.h"

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
CollisionTestScene::CollisionTestScene(HINSTANCE hInstance)

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
CollisionTestScene::~CollisionTestScene()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	for (size_t i = 0; i < gameEntities.size(); ++i)
		delete gameEntities[i];
	if (camera != nullptr) delete camera;

	ImGui_ImplDX11_Shutdown();

	MaterialManager::ReleaseInstance();
	MeshManager::ReleaseInstance();
	//InputManager::ReleaseInstance();
	ShaderManager::ReleaseInstance();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void CollisionTestScene::Init()
{
	//Initialize ImGui
	ImGui_ImplDX11_Init(hWnd, device, context);

	prevMousePos.x = width / 2;
	prevMousePos.y = height / 2;

	LoadShaderMeshMat();
	CreateEntities();
	InitInput();

	directionalLight = { vec4(0.1f, 0.5f, 0.1f, 1.0f),
		vec3(1.0f, 1.0f, 0.0f) };
	directionalLight2 = { vec4(0.8f, 0.8f, 0.5f, 1.0f),
		vec3(0.0f, -1.0f, 1.0f) };
	pointLight = { vec4(0.1f, 0.1f, 0.8f, 1.0f),
		vec3(0.0f, 5.0f, -5.0f) };
	ambientLight = vec4(0.1f, 0.1f, 0.1f, 1.0f);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders, loads texture to build materials, and loads meshes
// then adds them to the respective managers
// --------------------------------------------------------
void CollisionTestScene::LoadShaderMeshMat()
{
	//shaders
	shaderMngr = ShaderManager::GetInstancce();
	shaderMngr->Init(device, context);
	shaderMngr->AddVertexShader("vBasic");
	shaderMngr->AddPixelShader("pBasic");

	//hoisting shaders
	SimpleVertexShader* vShader = shaderMngr->GetVertexShader("vBasic");
	SimplePixelShader* pShader = shaderMngr->GetPixelShader("pBasic");

	//materials
	matMngr = MaterialManager::GetInstancce();
	matMngr->Init(device, context);
	matMngr->AddMat("concrete", vShader, pShader, L"Assets/Textures/concrete.jpg");
	matMngr->AddMat("soil", vShader, pShader, L"Assets/Textures/soil.jpg");
	matMngr->AddMat("woodplanks", vShader, pShader, L"Assets/Textures/woodplanks.jpg");

	//meshes
	meshMngr = MeshManager::GetInstancce();
	meshMngr->Init(device);
	meshMngr->AddMesh("helix", "Assets/Models/helix.obj");
	meshMngr->AddMesh("cone", "Assets/Models/cone.obj");
	meshMngr->AddMesh("cylinder", "Assets/Models/cylinder.obj");
	meshMngr->AddMesh("sphere", "Assets/Models/sphere.obj");
	meshMngr->AddMesh("torus", "Assets/Models/torus.obj");
	meshMngr->AddMesh("cube", "Assets/Models/cube.obj");
}

void CollisionTestScene::CreateEntities()
{
	//create camera
	camera = new Camera((float)width, (float)height, vec3(0.0f, 0.0f, -5.0f), 0.0f, 0.0f);

	//create entities
	sphere1 = new GameEntity(meshMngr->GetMesh("sphere"), matMngr->GetMat("woodplanks"), SPHERE,
		vec3(0, 0, 0), vec3(0, 0, 0), 1.0f);
	gameEntities.push_back(sphere1);

	sphere2 = new GameEntity(meshMngr->GetMesh("sphere"), matMngr->GetMat("soil"), SPHERE,
		vec3(0.75, 0, 0), vec3(0, 0, 0), 0.75f);
	gameEntities.push_back(sphere2);
}

void CollisionTestScene::InitInput()
{
	//inputMngr = InputManager::GetInstance();
	//char* usedChars = "WSAD XT";
	//inputMngr->AddKeysToPollFor(usedChars, strlen(usedChars));
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void CollisionTestScene::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->SetWidthHeight((float)width, (float)height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void CollisionTestScene::Update(float deltaTime, float totalTime)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;

	//All Imgui UI stuff must be done between imgui newFrame() and imgui render()
	ImGui_ImplDX11_NewFrame();

	camera->Update();

	sphere1->SetRotationQuaterniont(0.0f, 0.0f, 2 * deltaTime, 0.0f);
	sphere2->SetPosition(pos[0], pos[1], pos[2]);
	CollisionSolver::DetectCollisionSphereSphere(sphere1->GetCollider(), sphere2->GetCollider());

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void CollisionTestScene::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { clear_color.x, clear_color.y, clear_color.z, clear_color.w };

	// Clear the render target and depth buffer (erases what's on the screen)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//draw all the entities
	for (size_t i = 0; i < gameEntities.size(); ++i)
	{
		//hoist the mesh and mat of the entity
		Mesh* meshPtr = gameEntities[i]->GetMesh();
		Material * matPtr = gameEntities[i]->GetMat();

		//early exit
		if (meshPtr == nullptr || matPtr == nullptr) continue;

		/*This is Per-frame data that we can offset into a renderer class we won't have*/
		SimplePixelShader* pixelShader = matPtr->GetPixelShader();
		pixelShader->SetFloat4("ambientColor", ambientLight);
		pixelShader->SetData("directionalLight", &directionalLight, sizeof(DirectionalLight));
		pixelShader->SetData("directionalLight2", &directionalLight2, sizeof(DirectionalLight));
		pixelShader->SetData("pointLight", &pointLight, sizeof(PointLight));
		pixelShader->SetFloat3("cameraPos", camera->GetPos());

		SimpleVertexShader* vertexShader = matPtr->GetVertexShader();
		vertexShader->SetMatrix4x4("view", *(camera->GetViewMatTransposed()));
		vertexShader->SetMatrix4x4("projection", *(camera->GetProjMatTransposed()));

		//prepare per-object data
		matPtr->PrepareMaterial(gameEntities[i]->GetWorldMat());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		ID3D11Buffer * vertexBuffer = meshPtr->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &(vertexBuffer), &stride, &offset);
		context->IASetIndexBuffer(meshPtr->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshPtr->GetIndexCount(), 0, 0);
	}
	// 1. Show a simple window
	{
		static float f = 0.0f;
		ImGui::Text("Collision Test Debugger");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0 to 1
		ImGui::SliderFloat3("Sphere2 Pos", pos, -5, 5); // Edit 3 floats using a slider from -5 to 5
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::Checkbox("Free Look Enabled", &freelookEnabled);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::Render();
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void CollisionTestScene::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	ImGuiIO& io = ImGui::GetIO();

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
void CollisionTestScene::OnMouseUp(WPARAM buttonState, int x, int y)
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
void CollisionTestScene::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);

	if (buttonState && 0x0001 && freelookEnabled)
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
void CollisionTestScene::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion