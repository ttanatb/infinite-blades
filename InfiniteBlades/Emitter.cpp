#include "Emitter.h"


Emitter::Emitter(ID3D11Device * device, Material* material)
	: GameEntity(nullptr, material, vec3(0, 0, 0.0f), vec3(0, 0, 0), vec3(1.0f, 1.0f, 0.0f))
{
	this->color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->velocity = vec3(0.0f, 3.0f, 0.0f);
	this->lifetime = 10;
	this->loopable = true;
	this->active = true;
	this->maxParticles = 10;
	this->emissionRate = 0.1f;
	this->size = 1;
	this->timeSinceLastEmit = 0;
	this->livingParticleCount = 0;
	this->firstAliveIndex = 0;
	this->firstDeadIndex = 0;
	CreateParticles();
	CreateBuffers(device);
}

Emitter::Emitter(ID3D11Device * device, Material* material, vec4 color, vec3 velocity,
	float lifetime, bool loopable, bool active, int maxParticles, float emissionRate, vec3 position)
	: GameEntity(nullptr, material, position, vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f))
{
	this->color = color;
	this->velocity = velocity;
	this->lifetime = lifetime;
	this->loopable = loopable;
	this->active = active;
	this->maxParticles = maxParticles;
	this->emissionRate = 1.0f / emissionRate;
	this->size = 1;
	this->timeSinceLastEmit = 0;
	this->livingParticleCount = 0;
	this->firstAliveIndex = 0;
	this->firstDeadIndex = 0;
	CreateParticles();
	CreateBuffers(device);
}

Emitter::~Emitter()
{
	if (emitterVertBuffer) emitterVertBuffer->Release();
	if (emitterIndexBuffer) emitterIndexBuffer->Release();
	delete[] particles;
	delete[] localParticleVertices;
}

//Here we will do all emission calculations
void Emitter::Update(ID3D11DeviceContext* context, float deltaTime)
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
			UpdateParticle(deltaTime, i);
		}
	}
	//if first alive is after first dead
	else {
		//update first alive - max
		for (int i = firstAliveIndex; i < maxParticles; i++) {
			UpdateParticle(deltaTime, i);
		}
		//update 0 - first dead
		for (int i = 0; i < firstDeadIndex; i++) {
			UpdateParticle(deltaTime, i);
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

	//TODO: update color and size (if change over time)

	//Update position based on velocity (basic)
	particles[index].Position.x += particles[index].StartVelocity.x * deltaTime;
	particles[index].Position.y += particles[index].StartVelocity.y * deltaTime;
	particles[index].Position.z += particles[index].StartVelocity.z * deltaTime;
}

void Emitter::SpawnParticle()
{
	//Early break if at the limit
	if (livingParticleCount == maxParticles) return;

	//create new particle
	particles[firstDeadIndex].Age = 0;
	particles[firstDeadIndex].Size = size;
	particles[firstDeadIndex].Color = color;
	particles[firstDeadIndex].Position = position;
	particles[firstDeadIndex].StartVelocity = velocity;
	particles[firstDeadIndex].StartVelocity.x += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVelocity.y += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVelocity.z += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;

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
}

ID3D11Buffer * Emitter::getVertexBuffer() { return emitterVertBuffer; }

ID3D11Buffer * Emitter::getIndexBuffer() { return emitterIndexBuffer; }

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

	localParticleVertices[i + 0].Position = particles[index].Position;
	localParticleVertices[i + 1].Position = particles[index].Position;
	localParticleVertices[i + 2].Position = particles[index].Position;
	localParticleVertices[i + 3].Position = particles[index].Position;

	localParticleVertices[i + 0].Size = particles[index].Size;
	localParticleVertices[i + 1].Size = particles[index].Size;
	localParticleVertices[i + 2].Size = particles[index].Size;
	localParticleVertices[i + 3].Size = particles[index].Size;

	localParticleVertices[i + 0].Color = particles[index].Color;
	localParticleVertices[i + 1].Color = particles[index].Color;
	localParticleVertices[i + 2].Color = particles[index].Color;
	localParticleVertices[i + 3].Color = particles[index].Color;
}
