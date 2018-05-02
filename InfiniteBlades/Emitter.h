#pragma once
#include "GameEntity.h"
#include "Particle.h"
#include "Camera.h"

using namespace DirectX;
class Emitter
	: public GameEntity {
private:
	//Emission Properties
	int maxParticles;
	float emissionRate;	//Emissions per second
	bool loopable = true;
	bool active = true;
	vec4 startColor;
	vec4 endColor;
	vec3 velocity;
	float lifetime;
	float startSize;
	float endSize;
	float timeSinceLastEmit;

	//Type-Specific Properties
	EmitterType type;
	float width;
	float depth;

	//Particle Array
	Particle* particles;
	int firstDeadIndex;
	int firstAliveIndex;
	int livingParticleCount;

	//Graphics fields
	ParticleVertex* localParticleVertices;
	ID3D11Buffer* emitterVertBuffer;
	ID3D11Buffer* emitterIndexBuffer;

	//Render States
	ID3D11DepthStencilState* depthState;
	ID3D11BlendState* blendState;

	void CreateParticles();
	void CreateBuffers(ID3D11Device * device);
	void CreateRenderStates(ID3D11Device * device);
	void CopyLocalParticle(int index);
	void UpdateParticle(int index, float deltaTime);
	void SpawnParticle();
	void UpdateBuffers(ID3D11DeviceContext* context);
public:
	//Default Constructor
	Emitter(ID3D11Device * device, Material* material);

	//Complete Constructor
	Emitter(
		ID3D11Device * device,
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
		vec3 position
	);
	~Emitter();

	void Update(float deltaTime);
	void RenderParticles(ID3D11DeviceContext* context, Camera* camera);
	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	ID3D11DepthStencilState* getDepthState();
	ID3D11BlendState* getBlendState();
	void setVelocity(vec3 v);
	void SetAsPlane(float w, float d);
};

