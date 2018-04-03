#include "Collision.h"
#include <stdio.h>

bool Collision::DetectCollision(Collider* collider1, Collider* collider2)
{
	return false;
}

// AABB vs. AABB
bool Collision::DetectCollisionBoxBox(Collider* box1, Collider* box2)
{
	// Divide all incoming dimensions by 2
	box1->dimensions.x /= 2;
	box1->dimensions.y /= 2;
	box1->dimensions.z /= 2;
	box2->dimensions.x /= 2;
	box2->dimensions.y /= 2;
	box2->dimensions.z /= 2;

	//Determine if there is a collision with booleans rather than if statements
	bool right1Left2Check = box1->center.x + box1->dimensions.x > box2->center.x - box2->dimensions.x; // If the right side of box 1 is colliding with the left side of box 2
	bool left1Right2Check = box1->center.x - box1->dimensions.x < box2->center.x + box2->dimensions.x; // If the left side of box 1 is colliding with the right side of box 2
	bool top1Bottom2Check = box1->center.y + box1->dimensions.y > box2->center.y - box2->dimensions.y; // If the top side of box 1 is colliding with the bottom side of box 2
	bool bottom1Top2Check = box1->center.y - box1->dimensions.y < box2->center.y + box2->dimensions.y; // If the bottom side of box 1 is colliding with the top side of box 2
	bool front1Back2Check = box1->center.z + box1->dimensions.z > box2->center.z - box2->dimensions.z; // If the front side of box 1 is colliding with the back side of box 2
	bool back2Front1Check = box1->center.z - box1->dimensions.z < box2->center.z + box2->dimensions.z; // If the back side of box 1 is colliding with the front side of box 2

	if (right1Left2Check && left1Right2Check && top1Bottom2Check && bottom1Top2Check && front1Back2Check && back2Front1Check)
	{
		printf("Colliding: \n");
		return true;
	}

	printf("Not Colliding: \n");
	return false;
}

// Sphere vs. sphere 
bool Collision::DetectCollisionSphereSphere(Collider* sphere1, Collider* sphere2)
{
	// Divide all incoming dimensions by 2
	sphere1->dimensions.x /= 2;
	sphere2->dimensions.x /= 2;

	XMFLOAT3 pow2 = XMFLOAT3(2, 2, 2); //Vector for squaring
	XMVECTOR distanceBetweenCenter = XMLoadFloat3(&sphere1->center) - XMLoadFloat3(&sphere2->center); //Get the distance between the center of the two colliders

	float magnitude;
	XMStoreFloat(&magnitude, XMVector3Length(XMVectorSqrt(XMVectorPow(distanceBetweenCenter, XMLoadFloat3(&pow2))))); //Calculate the magnitude of the vector between the two objects

	float combinedRadius = sphere1->dimensions.x + sphere2->dimensions.x;

	if (magnitude > combinedRadius)  // Spheres are not colliding
	{
		printf("Not Colliding: %f\n", magnitude);
		return false;
	}

	printf("Colliding: %f\n", magnitude);
	return true;
}

// Sphere vs. AABB
bool Collision::DetectCollisionSphereBox(Collider * sphere, Collider * box)
{
	// Divide all incoming dimensions by 2
	sphere->dimensions.x /= 2;
	box->dimensions.x /= 2;
	box->dimensions.y /= 2;
	box->dimensions.z /= 2;

	//https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection#Sphere_versus_AABB
	// Get closest point from the box to the sphere by clamping
	float x = fmaxf(box->center.x - box->dimensions.x, fminf(sphere->center.x, box->center.x + box->dimensions.x));
	float y = fmaxf(box->center.y - box->dimensions.y, fminf(sphere->center.y, box->center.y + box->dimensions.y));
	float z = fmaxf(box->center.z - box->dimensions.z, fminf(sphere->center.z, box->center.z + box->dimensions.z));

	float distance = pow(x - sphere->center.x, 2) + pow(y - sphere->center.y, 2) + pow(z - sphere->center.z, 2);

	// If the distance squared > sphere's radius squared, there is no collision
	if (distance > pow(sphere->dimensions.x, 2))
	{
		printf("Not Colliding: \n");
		return false;
	}

	printf("Colliding: \n");
	return true;
}