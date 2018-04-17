#pragma once
#include "GameEntity.h"
#include "Particle.h"
class Emitter
	: public GameEntity {
private:
	//Emission
	int maxParticles;
	int emissionRate;	//Emissions per second
	bool loopable = true;
	bool active = true;
	vec4 color;
	vec3 velocity;
	float lifetime;

	//Particle Arrays
	Particle* particles;
	ParticleVertex* localParticleVertices;

	//Graphics fields
	ID3D11Buffer* emitterVertBuffer;
	ID3D11Buffer* emitterIndexBuffer;

	void CreateParticles();
	void EmitParticles(float deltaTime);
	void CreateBuffers(ID3D11Device * device, ID3D11DeviceContext* context);
public:
	Emitter(ID3D11Device * device, ID3D11DeviceContext* context, Material* material);
	Emitter(ID3D11Device * device, ID3D11DeviceContext* context, Material* material, 
		vec4 color, vec3 velocity, float lifetime, 
		bool loopable, bool active, int maxParticles, int emissionRate);
	~Emitter();

	void Update(ID3D11DeviceContext* context, float deltaTime);
	void RenderParticles(ID3D11DeviceContext* context);
	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	void UpdateBuffers(ID3D11DeviceContext* context);
};

