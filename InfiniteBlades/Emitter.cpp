#include "Emitter.h"


Emitter::Emitter(ID3D11Device * device, Material* material)
	: GameEntity(nullptr, material, vec3(0.f, 0.f, 0.f), vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f))
{
	this->startColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->endColor = vec4(1.0f, 1.0f, 1.0f, 0.0f);
	this->velocity = vec3(0.0f, 3.0f, 0.0f);
	this->lifetime = 1;
	this->loopable = true;
	this->active = true;
	this->maxParticles = 10;
	this->emissionRate = 0.1f;
	this->startSize = 0.2f;
	this->endSize = 0.4f;
	this->timeSinceLastEmit = 0;
	this->livingParticleCount = 0;
	this->firstAliveIndex = 0;
	this->firstDeadIndex = 0;
	this->type = Point;
	this->width = 0;
	this->depth = 0;
	CreateParticles();
	CreateBuffers(device);
	CreateRenderStates(device);
}

Emitter::Emitter(ID3D11Device * device, 
	Material* material, 
	vec4 startColor, 
	vec4 endColor, 
	vec3 velocity, 
	float startSize,
	float endSize,
	float lifetime, 
	bool loopable, 
	bool active, 
	int maxParticles, 
	float emissionRate, 
	vec3 position)
	: GameEntity(nullptr, material, position, vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f))
{
	this->startColor = startColor;
	this->endColor = endColor;
	this->velocity = velocity;
	this->lifetime = lifetime;
	this->loopable = loopable;
	this->active = active;
	this->maxParticles = maxParticles;
	this->emissionRate = 1.0f / emissionRate;
	this->startSize = startSize;
	this->endSize = endSize;
	this->timeSinceLastEmit = 0;
	this->livingParticleCount = 0;
	this->firstAliveIndex = 0;
	this->firstDeadIndex = 0;
	this->type = Point;
	this->width = 0;
	this->depth = 0;
	CreateParticles();
	CreateBuffers(device);
	CreateRenderStates(device);
}

Emitter::~Emitter()
{
	if (emitterVertBuffer) emitterVertBuffer->Release();
	if (emitterIndexBuffer) emitterIndexBuffer->Release();

	//Release render states
	depthState->Release();
	blendState->Release();

	delete[] particles;
	delete[] localParticleVertices;
}

//Here we will do all emission calculations
void Emitter::Update(float deltaTime)
{
	timeSinceLastEmit += deltaTime;

	while (timeSinceLastEmit > emissionRate) {
		SpawnParticle();
		timeSinceLastEmit -= emissionRate;
	}
	
	//check circular buffer
	//if first alive is before first dead
	if (firstAliveIndex < firstDeadIndex) {
		for (int i = firstAliveIndex; i < firstDeadIndex; ++i) {
			UpdateParticle(i, deltaTime);
		}
	}
	//if first alive is after first dead
	else {
		//update first alive - max
		for (int i = firstAliveIndex; i < maxParticles; i++) {
			UpdateParticle(i, deltaTime);
		}
		//update 0 - first dead
		for (int i = 0; i < firstDeadIndex; i++) {
			UpdateParticle(i, deltaTime);
		}
	}
}

void Emitter::UpdateParticle(int index, float deltaTime)
{
	//Check if dead
	if (particles[index].Age >= lifetime) return;

	//Update THEN check if dead again
	particles[index].Age += deltaTime;
	//If it died, shift the first alive index and remove
	if (particles[index].Age >= lifetime)
	{
		firstAliveIndex++;
		firstAliveIndex %= maxParticles;
		livingParticleCount--;
		return;
	}

	//Update position based on velocity (basic)
	particles[index].Position.x += particles[index].StartVelocity.x * deltaTime;
	particles[index].Position.y += particles[index].StartVelocity.y * deltaTime;
	particles[index].Position.z += particles[index].StartVelocity.z * deltaTime;

	//LERP color and size based on age
	float agePercentage = particles[index].Age / lifetime;

	//simple single float lerp for size
	particles[index].Size = startSize + agePercentage * (endSize - startSize);

	//Use XMVectorLerp for a vec4
	//We need to use XMVECTOR because Lerp is only available on this type
	//So we will convert to XMVECTOR, then convert back to XMFLOAT4
	XMVECTOR lerp = XMVectorLerp(XMLoadFloat4(&startColor), XMLoadFloat4(&endColor), agePercentage);
	XMStoreFloat4(&particles[index].Color, lerp);
}

void Emitter::SpawnParticle()
{
	//Early break if at the limit
	if (livingParticleCount == maxParticles) return;

	//create new particle
	particles[firstDeadIndex].Age = 0;
	particles[firstDeadIndex].Size = startSize;
	particles[firstDeadIndex].Color = startColor;
	particles[firstDeadIndex].StartVelocity = velocity;

	//Set velocities in random direction
	particles[firstDeadIndex].StartVelocity.x += ((float)rand() / RAND_MAX) * 0.6f - 0.2f;
	particles[firstDeadIndex].StartVelocity.y += ((float)rand() / RAND_MAX) * 0.6f - 0.2f;
	particles[firstDeadIndex].StartVelocity.z += ((float)rand() / RAND_MAX) * 0.6f - 0.2f;

	switch (type) {
	case Point:
		particles[firstDeadIndex].Position = position;
		break;
	case Plane:
		//Set a random position between the bounds of the **HORIZONTAL** plane
		vec3 newPos;
		newPos.x = position.x + ((float)rand() / RAND_MAX) * ((position.x + (width / 2.0f)) - (position.x - (width / 2.0f))) + (position.x - (width / 2.0f));
		float temp = (position.x + (width / 2.0f));
		newPos.y = position.y;
		newPos.z = position.z + ((float)rand() / RAND_MAX) * ((position.z + (depth / 2.0f)) - (position.z - (depth / 2.0f))) + (position.z - (depth / 2.0f));
		particles[firstDeadIndex].Position = newPos;
		break;
	}

	//Increment
	firstDeadIndex++;
	firstDeadIndex %= maxParticles;
	livingParticleCount++;
}

void Emitter::CreateParticles()
{
	particles = new Particle[maxParticles];
	localParticleVertices = new ParticleVertex[4 * maxParticles];

	//Create local vertices to be sent to GPU
	for (size_t i = 0; i < 4 * maxParticles; i+=4) {
		localParticleVertices[i].UV = vec2(0, 0);
		localParticleVertices[i + 1].UV = vec2(1, 0);
		localParticleVertices[i + 2].UV = vec2(1, 1);
		localParticleVertices[i + 3].UV = vec2(0, 1);
	}
}

void Emitter::RenderParticles(ID3D11DeviceContext * context, Camera* camera)
{
	//set render states
	float blend[4] = { 1,1,1,1 };
	context->OMSetBlendState(blendState, blend, 0xffffffff);
	context->OMSetDepthStencilState(depthState, 0);
	
	//Copy to the dynamic buffer
	UpdateBuffers(context);
	
	UINT stride;
	UINT offset;

	stride = sizeof(ParticleVertex);
	offset = 0;

	//set vertex buffer to active
	context->IASetVertexBuffers(0, 1, &emitterVertBuffer, &stride, &offset);

	//set index buffer to active
	context->IASetIndexBuffer(emitterIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	matPtr->GetVertexShader()->SetMatrix4x4("view", *(camera->GetViewMatTransposed()));
	matPtr->GetVertexShader()->SetMatrix4x4("projection", *(camera->GetProjMatTransposed()));

	//Use non-world PrepareMaterial because our shader doesn't use world space
	matPtr->PrepareMaterial(); 

	//draw the parts of the dynamic buffer we need
	if (firstAliveIndex < firstDeadIndex) {
		context->DrawIndexed(livingParticleCount * 6, firstAliveIndex * 6, 0);
	}
	else {
		//Draw from 0 to dead
		context->DrawIndexed(firstDeadIndex * 6, 0, 0);

		//Draw from alive to max
		context->DrawIndexed((maxParticles - firstAliveIndex) * 6, firstAliveIndex * 6, 0);
	}

	//reset render states back to default
	context->OMSetBlendState(0, blend, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);
}

ID3D11Buffer * Emitter::getVertexBuffer() { return emitterVertBuffer; }

ID3D11Buffer * Emitter::getIndexBuffer() { return emitterIndexBuffer; }

ID3D11DepthStencilState * Emitter::getDepthState(){ return depthState; }

ID3D11BlendState * Emitter::getBlendState() { return blendState; }

void Emitter::setVelocity(vec3 v) { velocity = v; }

void Emitter::SetAsPlane(float w, float d)
{
	type = Plane;
	width = w;
	depth = d;
}

void Emitter::CreateBuffers(ID3D11Device * device)
{
	//Set up vertex buffer description
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	//Enough room so we can potentially store all particles
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;

	device->CreateBuffer(&vbDesc, 0, &emitterVertBuffer);

	//initialize indices
	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(unsigned int) * maxParticles * 6;         // 4 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&ibd, &initialIndexData, &emitterIndexBuffer);
	delete[] indices;
}

void Emitter::CreateRenderStates(ID3D11Device * device)
{
	//particle depth state for blending
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &depthState);
	
	//particle blend state for blending
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &blendState);
}

//Rebuild entire dynamic vertex buffer each frame
void Emitter::UpdateBuffers(ID3D11DeviceContext * context)
{
	//Check Circular Buffer
	if (firstAliveIndex < firstDeadIndex) {
		for (int i = firstAliveIndex; i < firstDeadIndex; i++) {
			CopyLocalParticle(i);
		}
	}
	else {
		//first alive - max
		for (int i = firstAliveIndex; i < maxParticles; i++) {
			CopyLocalParticle(i);
		}
		//0 - first dead
		for (int i = 0; i < firstDeadIndex; i++) {
			CopyLocalParticle(i);
		}
	}

	//Copy to a DYNAMIC buffer
	//we need a local copy of all particle verts
	D3D11_MAPPED_SUBRESOURCE mapped = {};

	//Map() locks the buffer - GPU can't touch until unlocked
	context->Map(
		emitterVertBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);

	//Use mem-copy because it's fast
	memcpy(mapped.pData,
		localParticleVertices,
		sizeof(ParticleVertex) * 4 * maxParticles
	);

	//Unmap (unlock)
	context->Unmap(emitterVertBuffer, 0);
}

void Emitter::CopyLocalParticle(int index)
{
	int i = index * 4;

	for (size_t j = 0; j < 4; j++) {
		localParticleVertices[i + j].Position = particles[index].Position;
		localParticleVertices[i + j].Size = particles[index].Size;
		localParticleVertices[i + j].Color = particles[index].Color;
	}
}
