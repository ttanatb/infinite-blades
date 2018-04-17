#include "Emitter.h"



Emitter::Emitter(ID3D11Device * device, ID3D11DeviceContext* context, Material* material) 
	: GameEntity(nullptr, material, vec3(0, 0, 6.0f), vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f))
{
	this->color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->velocity = vec3(0.0f, 1.0f, 0.0f);
	this->lifetime = 10;
	this->loopable = true;
	this->active = true;
	this->maxParticles = 10;
	this->emissionRate = 10;
	CreateParticles();
	CreateBuffers(device, context);
}

Emitter::Emitter(ID3D11Device * device, ID3D11DeviceContext * context, Material* material, vec4 color, vec3 velocity,
	float lifetime, bool loopable, bool active, int maxParticles, int emissionRate)
	: GameEntity(nullptr, material, vec3(0, 0, 6.0f), vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f))
{
	this->color = color;
	this->velocity = velocity;
	this->lifetime = lifetime;
	this->loopable = loopable;
	this->active = active;
	this->maxParticles = maxParticles;
	this->emissionRate = emissionRate;
	CreateParticles();
	CreateBuffers(device, context);
}

Emitter::~Emitter()
{
	if (emitterVertBuffer) emitterVertBuffer->Release();
	delete[] particles;
	delete[] localParticleVertices;
}

//Here we will do all emission calculations
void Emitter::Update()
{

}

void Emitter::CreateParticles()
{
	particles = new Particle[maxParticles];
	localParticleVertices = new ParticleVertex[4];
	for (size_t i = 0; i < maxParticles; ++i) {
		particles[i] = {};
		particles[i].Color = color;
		particles[i].Position = position;
		particles[i].Age = lifetime;
		particles[i].StartVelocity = velocity;
	}

	//Create local vertices to be sent to GPU
	for (size_t i = 0; i < 4; ++i) {
		localParticleVertices[i] = {};
		localParticleVertices[i].Color = color;
		localParticleVertices[i].Size = scale.x;
	}

	//top left
	localParticleVertices[0].Position = vec3(-0.5, 0.5, 0);
	localParticleVertices[0].UV = vec2(0, 1);

	//top right
	localParticleVertices[1].Position = vec3(0.5, 0.5, 0);
	localParticleVertices[1].UV = vec2(1, 1);

	//bottom right
	localParticleVertices[2].Position = vec3(0.5, -0.5, 0);
	localParticleVertices[2].UV = vec2(1, 0);

	//bottom left
	localParticleVertices[3].Position = vec3(-0.5, 0.5, 0);
	localParticleVertices[3].UV = vec2(0, 1);
}

ID3D11Buffer * Emitter::getVertexBuffer() { return emitterVertBuffer; }

void Emitter::CreateBuffers(ID3D11Device * device, ID3D11DeviceContext* context)
{
	//Set up buffer description
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Enough room so we can potentially store all particles
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;

	device->CreateBuffer(&vbDesc, 0, &emitterVertBuffer);

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

	//Unmap
	context->Unmap(emitterVertBuffer, 0);
}
