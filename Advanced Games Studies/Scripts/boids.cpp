#include <Urho3D/Core/CoreEvents.h>
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
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include "boids.h"

#include <Urho3D/DebugNew.h>


float Boid::Range_FAttract = 25.0f;
float Boid::Range_FRepel = 7.0f;
float Boid::Range_FAlign = 1.2f;
float Boid::FAttract_Vmax = 5.0f;
float Boid::FAttract_Factor = 6.0f;
float Boid::FRepel_Factor = 2.0f;
float Boid::FAlign_Factor = 1.1f;

Boid::~Boid()
{
}

void Boid::Initialise(ResourceCache *pRes, Scene *pScene)
{
	pNode = pScene->CreateChild("Boid");
	pNode->SetPosition(Vector3(Random(180.0f) - 90.0f, 0.0f, Random(180.0f) - 90.0f));
	pNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
	pNode->SetScale(1.0f + Random(2.0f));

	pObject = pNode->CreateComponent<StaticModel>();
	pObject->SetModel(pRes->GetResource<Model>("Models/Cone.mdl"));
	pObject->SetMaterial(pRes->GetResource<Material>("Material/Cone.xml"));
	pObject->SetCastShadows(true);

	pRigidBody = pNode->CreateComponent<RigidBody>(); 
	/*pRigidBody->SetCollisionLayer(2);*/
	pRigidBody->SetMass(1.0f);
	pRigidBody->SetUseGravity(false);
	pRigidBody->SetPosition(Vector3(Random(180.0f) - 90.0f, 30.0f, Random(180.0f) - 90.0f));
	pRigidBody->SetLinearVelocity(Vector3(Random(-20.0f, 20.0f), 0, Random(-20.0f, 20.0f)));

	pCollisionShape = pNode->CreateComponent<CollisionShape>();
	pCollisionShape->SetTriangleMesh(pObject->GetModel(), 0);
}

void Boid::ComputeForce(Boid *pBoidList)
{
	Vector3 CoM, CoV; //Centre of mass
	int n = 0;
	int m = 0;//Counting neighbours
	int boidsFound = 0;
	force = Vector3(0, 0, 0);//Set force to 0

	//Search for a neighhbour
	for (int i = 0; i < NumBoids; i++)
	{
		//Current Boid
		if (this == &pBoidList[i]) continue;
		boidsFound++;
		//vector between boids
		Vector3 sep = pRigidBody->GetPosition() - pBoidList[i].pRigidBody->GetPosition();
		float d = sep.Length();
		if (d < Range_FAttract)
		{
			//it is a neighbour		
			CoM += pBoidList[i].pRigidBody->GetPosition();
			n++;		
		}

		if (d < Range_FRepel)
		{
			Vector3 dir2 = (pBoidList[i].pRigidBody->GetPosition() - pRigidBody->GetPosition()).Normalized();
			force = force - (dir2 - pRigidBody->GetLinearVelocity())*FRepel_Factor;
		}

		if (d < Range_FAlign)
		{
			CoV += pBoidList[i].pRigidBody->GetLinearVelocity();
			m++;
		}

		//Limiting method

		/*if (boidsFound == NumBoids/6) break;*/
	}

	//Attractive force
	if (n > 0)
	{
		//Average CoM
		CoM /= n;
		Vector3 dir = (CoM - pRigidBody->GetPosition()).Normalized();
		Vector3 vDesired = dir * FAttract_Vmax;
		force += (vDesired - pRigidBody->GetLinearVelocity())*FAttract_Factor;
	}

	//Allignent force
	if (m > 0)
	{
		CoV /= m;
		force += (CoV - pRigidBody->GetLinearVelocity())*FAlign_Factor;
	}
}

void Boid::Update(float lastFrame)
{
	pRigidBody->ApplyForce(force);

	Vector3 vel = pRigidBody->GetLinearVelocity();
	float d = vel.Length();
	if (d < 10.0f)
	{
		d = 10.0f;
		pRigidBody->SetLinearVelocity(vel.Normalized()*d);
	}
	else if (d > 50.0f)
	{
		d = 50.0f;
		pRigidBody->SetLinearVelocity(vel.Normalized()*d);
	}

	Vector3 vn = vel.Normalized();
	Vector3 cp = -vn.CrossProduct(Vector3(0.0f, 1.0f, 0.0f));
	float dp = cp.DotProduct(vn);
	pRigidBody->SetRotation(Quaternion(Acos(dp), cp));

	Vector3 p = pRigidBody->GetPosition();
	if (p.y_ < 10.0f)
	{
		p.y_ = 10.0f;
		pRigidBody->SetPosition(p);
	}
	else if (p.y_ > 50.0f)
	{
		p.y_ = 50.0f;
		pRigidBody->SetPosition(p);
	}
}

BoidSet::~BoidSet()
{

}

void BoidSet::Initialise(ResourceCache *pRes, Scene *pScene)
{
	for (int x = 0; x < NumBoids; x++)
	{
		boidList[x].Initialise(pRes, pScene);
		/*boidList2[x].Initialise(pRes, pScene);
		boidList3[x].Initialise(pRes, pScene);
		boidList4[x].Initialise(pRes, pScene);
		boidList5[x].Initialise(pRes, pScene);
		boidList6[x].Initialise(pRes, pScene);*/
	}
}

void BoidSet::Update(float lastFrame)
{
	//BOID SPLITTING METHOD

	//if (update)
	//{
	//	for (int x = 0; x < NumBoids / 2; x++)
	//	{
	//		boidList[x].ComputeForce(&boidList[0]);
	//		boidList[x].Update(lastFrame);
	//	
	//		//boidList2[x].ComputeForce(&boidList[0]);
	//		//boidList2[x].Update(lastFrame);
	//		//boidList3[x].ComputeForce(&boidList[0]);
	//		//boidList3[x].Update(lastFrame);
	//		//boidList4[x].ComputeForce(&boidList[0]);
	//		//boidList4[x].Update(lastFrame);
	//		//boidList5[x].ComputeForce(&boidList[0]);
	//		//boidList5[x].Update(lastFrame);
	//		//boidList6[x].ComputeForce(&boidList[0]);
	//		//boidList6[x].Update(lastFrame);
	//	}
	//}
	//else {
	//	for (int x = NumBoids / 2; x < NumBoids; x++)
	//	{
	//		boidList[x].ComputeForce(&boidList[0]);
	//		boidList[x].Update(lastFrame);
	//	}
	//}
	//update = !update;

		for (int x = 0; x < NumBoids; x++)
		{
			boidList[x].ComputeForce(&boidList[0]);
			boidList[x].Update(lastFrame);		
			/*boidList2[x].ComputeForce(&boidList[0]);
			boidList2[x].Update(lastFrame);
			boidList3[x].ComputeForce(&boidList[0]);
			boidList3[x].Update(lastFrame);
			boidList4[x].ComputeForce(&boidList[0]);
			boidList4[x].Update(lastFrame);
			boidList5[x].ComputeForce(&boidList[0]);
			boidList5[x].Update(lastFrame);
			boidList6[x].ComputeForce(&boidList[0]);
			boidList6[x].Update(lastFrame);*/
		}
}