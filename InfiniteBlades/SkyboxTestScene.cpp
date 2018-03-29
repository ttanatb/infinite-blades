#include "SkyboxTestScene.h"


SkyboxTestScene::SkyboxTestScene(HINSTANCE hInstance)
// feed the handle, the name, width, and height
	: DXCore(hInstance, "Skybox", 1280, 720,

		//show debug states only in debug mode
#if defined(DEBUG) || defined(_DEBUG)
		true)
#else
	false)
#endif
{
	camera = nullptr;
}

SkyboxTestScene::~SkyboxTestScene()
{
	if (sphere != nullptr)
		delete sphere;
	if (camera != nullptr)
		delete camera;
	if (skybox != nullptr)
		delete skybox;

	MaterialManager::ReleaseInstance();
	MeshManager::ReleaseInstance();
	InputManager::ReleaseInstance();
	ShaderManager::ReleaseInstance();
}

void SkyboxTestScene::Init()
{
	LoaderShaderMeshMat();
	InitInput();
	//intialize camera
	camera = new Camera((float)width, (float)height, vec3(0.0f, 0.0f, -5.0f), 0.0f, 0.0f);
	//intialize sphere
	sphere = new GameEntity(meshMngr->GetMesh("sphere"), matMngr->GetMat("concrete"),
		vec3(0, 0, 0), vec3(45, 45, 90), 1.0f);
	
	//skybox shaders
	SimpleVertexShader* vs = shaderMngr->GetVertexShader("SkyBoxVS");
	SimplePixelShader* ps = shaderMngr->GetPixelShader("SkyBoxPS");

	skybox = new Skybox(L"Assets/Textures/SunnyCubeMap.dds", device, vs, ps, meshMngr->GetMesh("cube"));

	directionalLight = { vec4(0.1f, 0.5f, 0.1f, 1.0f),
		vec3(1.0f, 1.0f, 0.0f) };
	pointLight = { vec4(0.1f, 0.5f, 0.1f, 1.0f),
		vec3(1.0f, 1.0f, 0.0f) };
	ambientLight = vec4(0.1f, 0.1f, 0.1f, 1.0f);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SkyboxTestScene::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->SetWidthHeight((float)width, (float)height);
}

void SkyboxTestScene::Update(float deltaTime, float totalTime)
{
	inputMngr->Update();

	if (inputMngr->GetKey('A'))
		camera->MoveAlongRight(-0.001f);
	if (inputMngr->GetKey('D'))
		camera->MoveAlongRight(+0.001f);

	if (inputMngr->GetKey('W'))
		camera->MoveAlongForward(+0.001f);
	if (inputMngr->GetKey('S'))
		camera->MoveAlongForward(-0.001f);

	if (inputMngr->GetKey(' '))
		camera->Move(0, +0.001f, 0);
	if (inputMngr->GetKey('X'))
		camera->Move(0, -0.001f, 0);
	//update game objects
	camera->Update();
	sphere->Update();
	
	sphere->RotateOnAxis(vec3(0.0f, 1.0f, 1.0f), deltaTime);

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

void SkyboxTestScene::Draw(float deltaTime, float totalTime)
{
	//clear the render target and depth buffer
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//draw sphere
	Mesh* meshPtr = sphere->GetMesh();
	Material* matPtr = sphere->GetMat();

	SimplePixelShader* pixelShader = matPtr->GetPixelShader();
	pixelShader->SetFloat4("ambientColor", ambientLight);
	pixelShader->SetData("directionalLight", &directionalLight, sizeof(DirectionalLight));
	pixelShader->SetData("pointLight", &pointLight, sizeof(PointLight));

	SimpleVertexShader* vertexShader = matPtr->GetVertexShader();
	vertexShader->SetMatrix4x4("view", *(camera->GetViewMatTransposed()));
	vertexShader->SetMatrix4x4("projection", *(camera->GetProjMatTransposed()));

	//prepare per-object data
	matPtr->PrepareMaterial(sphere->GetWorldMat());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	ID3D11Buffer * vertexBuffer = meshPtr->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &(vertexBuffer), &stride, &offset);
	context->IASetIndexBuffer(meshPtr->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(meshPtr->GetIndexCount(), 0, 0);

	//render skybox
	skybox->Render(context, camera, stride, offset);
	
	// At the end of the frame, reset render states
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);

	swapChain->Present(0, 0);
}

void SkyboxTestScene::LoaderShaderMeshMat()
{
	//shaders
	shaderMngr = ShaderManager::GetInstancce();
	shaderMngr->Init(device, context);
	shaderMngr->AddVertexShader("vBasic");
	shaderMngr->AddPixelShader("pBasic");
	shaderMngr->AddVertexShader("SkyBoxVS");
	shaderMngr->AddPixelShader("SkyBoxPS");
	

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
	meshMngr->AddMesh("sphere", "Assets/Models/sphere.obj");
	meshMngr->AddMesh("cube", "Assets/Models/cube.obj");

}

void SkyboxTestScene::InitInput()
{
	inputMngr = InputManager::GetInstance();
	char* usedChars = "WSAD XT";
	inputMngr->AddKeysToPollFor(usedChars, strlen(usedChars));
}

#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void SkyboxTestScene::OnMouseDown(WPARAM buttonState, int x, int y)
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
void SkyboxTestScene::OnMouseUp(WPARAM buttonState, int x, int y)
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
void SkyboxTestScene::OnMouseMove(WPARAM buttonState, int x, int y)
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
void SkyboxTestScene::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion
