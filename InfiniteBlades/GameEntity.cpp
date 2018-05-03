#include "GameEntity.h"

using namespace DirectX;

void GameEntity::Init()
{
	children = std::vector<GameEntity*>();

	position = vec3(0.0f, 0.0f, 0.0f);
	rotation = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	scale = vec3(1.0f, 1.0f, 1.0f);

	coll.colliderType = ColliderType::NONE;

	XMMATRIX mat = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, mat);
	XMStoreFloat4x4(&parentWorldMatrix, mat);
	meshPtr = nullptr;
	matPtr = nullptr;

	isActive = true;

	//CalculateCollider();
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

	coll.colliderType = colliderType;

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

	coll.colliderType = colliderType;

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

}

XMFLOAT3 GameEntity::GetPositionFloat() const
{
	return (XMFLOAT3)position;
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

Collider GameEntity::GetCollider() const
{
	return coll;
}

void GameEntity::SetCollider(XMFLOAT3 min, XMFLOAT3 max)
{
	XMVECTOR vecToMin = XMVector3Transform(XMLoadFloat3(&min), XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z));
	XMVECTOR vecToMax = XMVector3Transform(XMLoadFloat3(&max), XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z));

	XMStoreFloat3(&coll.min, vecToMin);
	XMStoreFloat3(&coll.max, vecToMax);
}

void GameEntity::CalculateCollider()
{
	if (coll.colliderType != ColliderType::NONE)
	{
		std::vector<Vertex> vertices = meshPtr->GetVertices();
		XMFLOAT3 min = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
		XMFLOAT3 max = XMFLOAT3(FLT_MIN, FLT_MIN, FLT_MIN);
		for (int i = 0; i < meshPtr->GetVertexCount(); i++)
		{
			// Find mesh min xyz
			min.x = fmin(min.x, vertices[i].Position.x);
			min.y = fmin(min.y, vertices[i].Position.y);
			min.z = fmin(min.z, vertices[i].Position.z);

			// Find mesh max xyz
			max.x = fmax(max.x, vertices[i].Position.x);
			max.y = fmax(max.y, vertices[i].Position.y);
			max.z = fmax(max.z, vertices[i].Position.z);
		}
		SetCollider(min, max);
	}
}

void GameEntity::CopyCalculateCollider(Collider collider)
{
	std::vector<Vertex> vertices = meshPtr->GetVertices();
	XMFLOAT3 min = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 max = XMFLOAT3(FLT_MIN, FLT_MIN, FLT_MIN);
	for (int i = 0; i < meshPtr->GetVertexCount(); i++)
	{
		// Find mesh min xyz
		collider.min.x = fmin(min.x, vertices[i].Position.x);
		collider.min.y = fmin(min.y, vertices[i].Position.y);
		collider.min.z = fmin(min.z, vertices[i].Position.z);

		// Find mesh max xyz
		collider.max.x = fmax(max.x, vertices[i].Position.x);
		collider.max.y = fmax(max.y, vertices[i].Position.y);
		collider.max.z = fmax(max.z, vertices[i].Position.z);
	}
	SetCollider(min, max);
}

bool GameEntity::CheckCollision(const GameEntity & other)
{
	XMFLOAT3 otherPos = other.GetPositionFloat();

	float resultantX = otherPos.x - this->position.x;

	float aExtentX = coll.max.x;
	float bExtentX = other.GetCollider().max.x;

	if (fabs(resultantX) <= aExtentX + bExtentX)
	{
		float resultantY = otherPos.y - this->position.y;

		float aExtentY = coll.max.y;
		float bExtentY = other.GetCollider().max.y;

		if (fabs(resultantY) <= aExtentY + bExtentY)
		{
			return true;
		}
	}

	return false;
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
	CalculateCollider();
}

void GameEntity::ScaleBy(vec3 toScale)
{
	XMVECTOR newScale = XMVectorAdd(XMLoadFloat3(&scale), XMLoadFloat3(&toScale));
	XMStoreFloat3(&scale, newScale);
}