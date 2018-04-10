#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Collider.h"
#include <vector>

class GameEntity {
protected:
	mat4 worldMatrix;// = MAT4_IDENTITY;
	mat4 worldMatTransposed;

	vec3 position;// = vec3(0.0f, 0.0f, 0.0f);
	vec4 rotation;// = Q_IDENTITY;
	vec3 scale;// = vec3(1.0f, 1.0f, 1.0f);

	Mesh* meshPtr = nullptr;
	Material* matPtr = nullptr;
	Collider coll;

	GameEntity* parent = nullptr;
	std::vector<GameEntity*> children;
	mat4 parentWorldMatrix;

	void Init();
public:
	GameEntity();
	virtual ~GameEntity();

	GameEntity(Mesh * mesh, Material* material, ColliderType colliderType,
		vec3 position = vec3(0,0,0), 
		vec3 rotation = vec3(0,0,0), 
		vec3 scale = vec3(1,1,1));

	GameEntity(Mesh * mesh, Material* material, ColliderType colliderType,
		vec3 position = vec3(0, 0, 0),
		vec3 rotation = vec3(0, 0, 0),
		float scale = 1);

	void Update();

	mat4* GetWorldMat();
	vec3 GetPosition();
	vec4 GetRotation();
	vec3 GetRotEuler();
	vec3 GetScale();

	Mesh* GetMesh();
	void SetMesh(Mesh* mesh);

	Material * GetMat();
	void SetMat(Material * newMat);
	void SetParent(GameEntity* parent);

	Collider* GetCollider();

	void SetPosition(vec3 newPos);
	void SetPosition(float x, float y, float z);
	void SetScale(vec3 newScale);
	void SetScale(float x, float y, float z);
	void SetRotationEuler(vec3 euler);
	void SetRotationEuler(float x, float y, float z);
	void SetRotationQuaternion(vec4 quaternion);
	void SetRotationQuaterniont(float x, float y, float z, float w);

	void TranslateBy(float x, float y, float z);
	void TranslateBy(vec3 translation);
	void RotateByEuler(vec3 euler);
	void RotateOnAxis(float x, float y, float z, float angle);
	void RotateOnAxis(vec3 axis, float angle);
	void RotateByQuaternion(vec4 quaternion);
	void ScaleBy(float x, float y, float z);
	void ScaleBy(vec3 toScale);
};