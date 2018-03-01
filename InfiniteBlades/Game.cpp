#include "Game.h"

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
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	camera = nullptr;
	vertexShader = 0;
	pixelShader = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	for (size_t i = 0; i < gameEntities.size(); ++i) {
		delete gameEntities[i];
	}

	Mesh::ReleasePrimitives();

	if (camera != nullptr) delete camera;
	MaterialManager::ReleaseInstance();
	MeshManager::ReleaseInstance();
	Input::ReleaseInstance();

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	LoadMeshAndMat();
	CreateGameEntities();
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
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	camera = new Camera((float)width,
		(float)height,
		vec3(0.0f, 0.0f, -5.0f),
		0.0f, 0.0f);
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::LoadMeshAndMat()
{
	matMngr = MaterialManager::GetInstancce();
	matMngr->Init(device, context);

	matMngr->AddMat("concrete", vertexShader, pixelShader, L"Assets/Textures/concrete.jpg");
	matMngr->AddMat("soil", vertexShader, pixelShader, L"Assets/Textures/soil.jpg");
	matMngr->AddMat("woodplanks", vertexShader, pixelShader, L"Assets/Textures/woodplanks.jpg");

	meshMngr = MeshManager::GetInstancce();
	meshMngr->Init(device);

	meshMngr->AddMesh("helix",		"Assets/Models/helix.obj");
	meshMngr->AddMesh("cone",		"Assets/Models/cone.obj");
	meshMngr->AddMesh("cylinder",	"Assets/Models/cylinder.obj");
	meshMngr->AddMesh("sphere",		"Assets/Models/sphere.obj");
	meshMngr->AddMesh("torus",		"Assets/Models/torus.obj");
	meshMngr->AddMesh("cube",		"Assets/Models/cube.obj");
}

void Game::CreateGameEntities()
{
	gameEntities.push_back(new GameEntity(meshMngr->GetMesh("torus"), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.5f, 0.5f, 0.5f)));
	gameEntities.push_back(new GameEntity(meshMngr->GetMesh("cone"), vec3(-2.5f, -1.0f, 2.0f), vec3(45.0f, 0.0f, 45.0f), vec3(1.0f, 1.0f, 1.0f)));
	gameEntities.push_back(new GameEntity(meshMngr->GetMesh("helix"), vec3(0.0f, -0.5f, 2.0f), vec3(0.0f, 45.0f, 0.0f), vec3(0.2f, 0.2f, 0.2f)));
	gameEntities.push_back(new GameEntity(meshMngr->GetMesh("sphere"), vec3(3.0f, -0.5f, 0.0f), vec3(0.0f, 0.0f, 45.0f), vec3(0.5f, 0.5f, 0.5f)));
	gameEntities.push_back(new GameEntity(meshMngr->GetMesh("torus"), vec3(0.0f, 0.0f, -2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f)));

	gameEntities[0]->SetMat(matMngr->GetMat("woodplanks"));
	gameEntities[1]->SetMat(matMngr->GetMat("concrete"));
	gameEntities[2]->SetMat(matMngr->GetMat("soil"));
	gameEntities[3]->SetMat(matMngr->GetMat("woodplanks"));
	gameEntities[4]->SetMat(matMngr->GetMat("soil"));

	gameEntities[4]->SetParent(gameEntities[0]);
}

void Game::InitInput()
{
	inputMngr = Input::GetInstance();
	char usedChars[6] = { 'W', 'S', 'A', 'D', ' ', 'X' };
	inputMngr->AddKeysToPollFor(usedChars, 6);
}



// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	camera->SetWidthHeight((float)width, (float)height);
	// Update our projection matrix since the window size changed
	//XMMATRIX P = XMMatrixPerspectiveFovLH(
	//	0.25f * 3.1415926535f,	// Field of View Angle
	//	(float)width / height,	// Aspect ratio
	//	0.1f,				  	// Near clip plane distance
	//	100.0f);			  	// Far clip plane distance
	//XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	inputMngr->Update();
	camera->Update();

	for (size_t i = 0; i < gameEntities.size(); ++i) {
		gameEntities[i]->Update();
	}

	gameEntities[0]->TranslateBy(sin(totalTime) / 200.0f, 0.0f, 0.0f);
	gameEntities[0]->RotateOnAxis(vec3(0.0f, 1.0f, 0.0f), deltaTime);
	gameEntities[0]->ScaleBy(sin(totalTime) / 1000.0f, sin(totalTime) / 1000.0f, sin(totalTime) / 1000.0f);
	gameEntities[1]->RotateOnAxis(sin(totalTime), 0.0f, cos(totalTime), deltaTime * 2.0f);
	gameEntities[2]->ScaleBy(sin(totalTime) / 2000.0f, sin(totalTime) / 2000.0f, sin(totalTime) / 2000.0f);
	gameEntities[2]->RotateOnAxis(sin(totalTime), 0.0f, cos(totalTime), deltaTime * 2.0f);
	gameEntities[3]->TranslateBy(0.0f, cos(totalTime * 20.0f) / 500.0f, 0.0f);
	gameEntities[3]->RotateOnAxis(0.0f, 1.0f, 0.0f, deltaTime);
	gameEntities[4]->RotateOnAxis(vec3(0.0f, 0.0f, 1.0f), deltaTime);

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//set per frame data
	pixelShader->SetFloat4("ambientColor", ambientLight);
	pixelShader->SetData("directionalLight", &directionalLight, sizeof(DirectionalLight));
	pixelShader->SetData("directionalLight2", &directionalLight2, sizeof(DirectionalLight));
	pixelShader->SetData("pointLight", &pointLight, sizeof(PointLight));

	pixelShader->SetFloat3("cameraPos", camera->GetPos());
	vertexShader->SetMatrix4x4("view", *(camera->GetViewMatTransposed()));
	vertexShader->SetMatrix4x4("projection", *(camera->GetProjMatTransposed()));

	for (size_t i = 0; i < gameEntities.size(); ++i) {
		Mesh* meshPtr = gameEntities[i]->GetMesh();
		Material * matPtr = gameEntities[i]->GetMat();
		if (meshPtr == nullptr || matPtr == nullptr) continue;

		matPtr->PrepareMaterial(camera->GetViewMatTransposed(),
			camera->GetProjMatTransposed(),
			gameEntities[i]->GetWorldMat());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		ID3D11Buffer * vertexBuffer = meshPtr->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &(vertexBuffer), &stride, &offset);
		context->IASetIndexBuffer(meshPtr->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshPtr->GetIndexCount(), 0, 0);
	}

	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
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
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
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
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
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
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion