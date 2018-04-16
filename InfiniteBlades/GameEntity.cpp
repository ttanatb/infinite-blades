#include "GameEntity.h"

using namespace DirectX;

void GameEntity::Init()
{
	children = std::vector<GameEntity*>();

	position = vec3(0.0f, 0.0f, 0.0f);
	rotation = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	scale = vec3(1.0f, 1.0f, 1.0f);

	coll = Collider();

	XMMATRIX mat = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, mat);
	XMStoreFloat4x4(&parentWorldMatrix, mat);
	meshPtr = nullptr;
	matPtr = nullptr;
}

GameEntity::GameEntity()
{
	Init();
}

GameEntity::~GameEntity()
{
}

GameEntity::GameEntity(Mesh * mesh, Material* material, vec3 position, vec3 rotation, vec3 scale)
{
	Init();
	meshPtr = mesh;
	matPtr = material;
	this->position = position;
	this->scale = scale;

	coll = Collider(mesh->GetColliderType(), this->position, this->scale, false);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMStoreFloat4(&(this->rotation), quaternion);
}

GameEntity::GameEntity(Mesh * mesh, Material * material, vec3 position, vec3 rotation, float scale)
{
	Init();

	meshPtr = mesh;
	matPtr = material;
	this->position = position;
	this->scale = vec3(scale, scale, scale);

	coll = Collider(mesh->GetColliderType(), this->position, this->scale, false);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMStoreFloat4(&(this->rotation), quaternion);
}

GameEntity::GameEntity(Mesh * mesh, Material * material, ColliderType colliderType, vec3 position, vec3 rotation, float scale)
{
	Init();

	meshPtr = mesh;
	matPtr = material;
	this->position = position;
	this->scale = vec3(scale, scale, scale);

	coll = Collider(colliderType, this->position, this->scale, false);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMStoreFloat4(&(this->rotation), quaternion);
}

GameEntity::GameEntity(Mesh * mesh, Material* material, ColliderType colliderType, vec3 position, vec3 rotation, vec3 scale)
{
	Init();
	meshPtr = mesh;
	matPtr = material;
	this->position = position;
	this->scale = scale;

	coll = Collider(colliderType, position, scale, false);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMStoreFloat4(&(this->rotation), quaternion);
}

void GameEntity::Update()
{
	XMMATRIX mat = XMMatrixIdentity();
	XMVECTOR quaternion = XMLoadFloat4(&rotation);

	if (parent != nullptr) {
		mat = XMLoadFloat4x4(&parentWorldMatrix);
	}

	mat = XMMatrixMultiply(
		XMMatrixMultiply((XMMatrixMultiply(XMMatrixScaling(scale.x, scale.y, scale.z),
			XMMatrixRotationQuaternion(quaternion))),
			XMMatrixTranslation(position.x, position.y, position.z)), mat);
	XMStoreFloat4x4(&worldMatTransposed, DirectX::XMMatrixTranspose(mat));

	for (size_t i = 0; i < children.size(); ++i) {
		XMStoreFloat4x4(&(children[i]->parentWorldMatrix), mat);
	}

	coll.center = this->position;
	coll.dimensions = this->scale;
}

mat4* GameEntity::GetWorldMat() { return &worldMatTransposed; }
vec3 GameEntity::GetPosition() { return position; }
vec4 GameEntity::GetRotation() { return rotation; }
vec3 GameEntity::GetScale() { return scale; }
vec3 GameEntity::GetRotEuler()
{

	return vec3();
}




Mesh * GameEntity::GetMesh() { return meshPtr; }
void GameEntity::SetMesh(Mesh* mesh) { meshPtr = mesh; }

Material * GameEntity::GetMat() { return matPtr; }
void GameEntity::SetMat(Material * newMat) { matPtr = newMat; }

void GameEntity::SetParent(GameEntity * parent)
{
	this->parent = parent;
	parent->children.push_back(this);
}

Collider * GameEntity::GetCollider()
{
	return &coll;
}

void GameEntity::SetPosition(vec3 newPos)
{
	this->position = newPos;
	Update();
}

void GameEntity::SetPosition(float x, float y, float z)
{
	SetPosition(vec3(x, y, z));
}


void GameEntity::SetRotationQuaternion(vec4 quaternion)
{
	this->rotation = quaternion;
	Update();
}

void GameEntity::SetRotationQuaterniont(float x, float y, float z, float w)
{
	SetRotationQuaternion(vec4(x, y, z, w));
}

void GameEntity::TranslateBy(float x, float y, float z)
{
	TranslateBy(vec3(x, y, z));
}

void GameEntity::TranslateBy(vec3 translation)
{
	XMVECTOR newPos = XMVectorAdd(XMLoadFloat3(&translation), XMLoadFloat3(&position));
	XMStoreFloat3(&position, newPos);
}

void GameEntity::RotateOnAxis(float x, float y, float z, float angle)
{
	RotateOnAxis(vec3(x, y, z), angle);
}

void GameEntity::RotateOnAxis(vec3 axis, float angle)
{
	XMVECTOR quaternion = XMQuaternionMultiply(XMLoadFloat4(&rotation),
		XMQuaternionRotationAxis(XMLoadFloat3(&axis), angle));
	XMStoreFloat4(&rotation, quaternion);
}


void GameEntity::ScaleBy(float x, float y, float z)
{
	ScaleBy(vec3(x, y, z));
}

void GameEntity::ScaleBy(vec3 toScale)
{
	XMVECTOR newScale = XMVectorAdd(XMLoadFloat3(&scale), XMLoadFloat3(&toScale));
	XMStoreFloat3(&scale, newScale);
}