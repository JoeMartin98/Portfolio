#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

namespace Urho3D
{
	class Node;
	class Scene;
	class RigidBody;
	class CollisionShape;
	class ResourceCache;
}

using namespace Urho3D;
const int NumBoids = 300;

class Boid 
{
public:
	Boid() {
		pNode = nullptr;
		pRigidBody = nullptr;
		pCollisionShape = nullptr;
		pObject = nullptr;
	};
	~Boid();

	Node* pNode;
	RigidBody* pRigidBody;
	CollisionShape* pCollisionShape;
	StaticModel* pObject;
	Vector3 force;


	static float Range_FAttract;
	static float Range_FRepel;
	static float Range_FAlign;
	static float FAttract_Factor;
	static float FRepel_Factor;
	static float FAlign_Factor;
	static float FAttract_Vmax;

	void Initialise(ResourceCache *pRes, Scene *pScene);
	void ComputeForce(Boid *pBoid);
	void Update(float lastFrame);


};

class BoidSet
{
public: 
	Boid boidList[NumBoids];
	Boid boidList2[NumBoids];
	Boid boidList3[NumBoids];
	Boid boidList4[NumBoids];
	Boid boidList5[NumBoids];
	Boid boidList6[NumBoids];


	bool update;
	BoidSet() {};
	~BoidSet();
	void Initialise(ResourceCache *pRes, Scene *pScene);
	void Update(float lastFrame);
};