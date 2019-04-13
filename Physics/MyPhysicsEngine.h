#pragma once

#include "BasicActors.h"
#include "Extras\Camera.h"
#include "Extras\Renderer.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	//a list of colours: Circus Palette
	static const PxVec3 color_palette[] = { PxVec3(255.0f, 255.0f, 255.0f), PxVec3(.0f, 252.0f, .0f) };

	//pyramid vertices
	static PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = {1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1};

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose=PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs),end(pyramid_trigs)), pose)
		{
		}
	};

	struct FilterGroup
	{
		enum Enum
		{
			ACTOR0 = (1 << 0),
			ACTOR1 = (1 << 1),
			ACTOR2 = (1 << 2)
			//add more if you need
		};
	};

	///A customised collision class, implemneting various callbacks
	class MySimulationEventCallback : public PxSimulationEventCallback
	{
	public:
		//an example variable that will be checked in the main simulation loop
		bool trigger;

		MySimulationEventCallback() : trigger(false) {}

		///Method called when the contact with the trigger object is detected.
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count)
		{
			//you can read the trigger information here
			for (PxU32 i = 0; i < count; i++)
			{
				//filter out contact with the planes
				if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
				{

					//check if eNOTIFY_TOUCH_FOUND trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						string collision = pairs->otherActor->getName();
						string collisionPlace = pairs->triggerActor->getName();
						if (collision == "ball" && collisionPlace == "goal")
						{
							pairs->otherActor->setGlobalPose(PxTransform(PxVec3(2.0f, 5.0f, -2.0f)));
							pairs->otherActor->isRigidBody()->setLinearVelocity(PxVec3(.0f, .0f, .0f));
							cerr << "TRYYYYY" << endl;
							//generateField
						}

						if (collision == "ball" && collisionPlace == "floor")
						{
							pairs->otherActor->setGlobalPose(PxTransform(PxVec3(2.0f, 5.0f, -3.0f)));
							pairs->otherActor->isRigidBody()->setLinearVelocity(PxVec3(.0f, .0f, .0f));
							cerr << "DEADDDDDD" << endl;
						}
					//	cerr << "onTrigger::eNOTIFY_TOUCH_FOUND" << endl;
						cerr << collision << endl;
						trigger = true;
					}
					//check if eNOTIFY_TOUCH_LOST trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						//cerr << "onTrigger::eNOTIFY_TOUCH_LOST" << endl;
						trigger = false;
					}				
				}
			}
		}

		///Method called when the contact by the filter shader is detected.
		virtual void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs)
		{
			//cerr << "Contact found between " << pairHeader.actors[0]->getName() << " " << pairHeader.actors[1]->getName() << endl;

			//check all pairs
			for (PxU32 i = 0; i < nbPairs; i++)
			{
				//check eNOTIFY_TOUCH_FOUND
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					//cerr << "onContact::eNOTIFY_TOUCH_FOUND" << endl;
				}
				//check eNOTIFY_TOUCH_LOST
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
				//	cerr << "onContact::eNOTIFY_TOUCH_LOST" << endl;
				}
			}
		}

		virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {}
		virtual void onWake(PxActor **actors, PxU32 count) {}
		virtual void onSleep(PxActor **actors, PxU32 count) {}
	};

	//A simple filter shader based on PxDefaultSimulationFilterShader - without group filtering
	static PxFilterFlags CustomFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		// let triggers through
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		//enable continous collision detection
//		pairFlags |= PxPairFlag::eCCD_LINEAR;


		//customise collision filtering here
		//e.g.

		// trigger the contact callback for pairs (A,B) where 
		// the filtermask of A contains the ID of B and vice versa.
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			//trigger onContact callback for this pair of objects
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
			//			pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}

		return PxFilterFlags();
	};


	///Custom scene class
	class MyScene : public Scene
	{
		Field* field;
		Plane* plane;
		StaticBox* goal;
		StaticBox* floor;
		vector<Axe*> base;
		StaticBox* box;
		Post* post;
		Capsule* ball;
		vector<Spear*> spear;
		holder* hold;
		vector<BoxCompound*> axe;
		vector<RevoluteJoint*> rJoint;
		int count = 0;
		bool forward = true;
		bool start = true;
		bool numbersGood = false;
		float forwardSpeed;
		int min = 0;
		int max = 8;
		int frame = 0;
		int move[5] = {};
		Cloth* cloth;
		vector<PxVec3> spearInit;
		vector<PxVec3> spearInitRight;
		PxVec3 ballInit;

		//materials
		PxMaterial* rubber;
		PxMaterial* wood;

		/*Spears tramp;
*/
		MySimulationEventCallback* my_callback;
			
	public:
		///A custom scene class
		MyScene() : Scene() {};

		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);

			my_callback = new MySimulationEventCallback();
			px_scene->setSimulationEventCallback(my_callback);

			rubber = CreateMaterial(1.0f, 1.0f, .5f);
			wood = CreateMaterial(1.0f, 1.0f, .01f);


			//floor
			floor = new StaticBox(PxTransform(PxVec3(0.0f, -2.0f, .0f)), PxVec3(140.0f, .6f, 150.0f));
			floor->SetTrigger(true);
			floor->Name("floor");
			Add(floor);
				
			//FIELD
			field = new Field(PxTransform(PxVec3(2.0f, .0f, 25.0f)));
			field->Name("field");
			field->Color(color_palette[1]);	

			Add(field);
			
			//EDGE
			box = new StaticBox(PxTransform(PxVec3(36.9f, .0f, 25.0f)), PxVec3(15.0f, .3f, 35.0f));
			box->Color(PxVec3(.0f/255.0f, .0f / 255.0f, .0f / 255.0f));
			Add(box);

			//EDGE
			box = new StaticBox(PxTransform(PxVec3(-32.9f, .0f, 25.0f)), PxVec3(15.0f, .3f, 35.0f));
			box->Color(PxVec3(.0f / 255.0f, .0f / 255.0f, .0f / 255.0f));
			Add(box);

			//EDGE COLLISION
			box = new StaticBox(PxTransform(PxVec3(36.9f, .0f, 25.0f)), PxVec3(15.0f, .3f, 35.0f));
			box->Color(PxVec3(.0f / 255.0f, .0f / 255.0f, .0f / 255.0f));
			box->SetTrigger(true);
			box->Name("floor");
			Add(box);

			//EDGE COLLISION
			box = new StaticBox(PxTransform(PxVec3(-32.9f, .0f, 25.0f)), PxVec3(15.0f, .3f, 35.0f));
			box->SetTrigger(true);
			box->Color(PxVec3(.0f / 255.0f, .0f / 255.0f, .0f / 255.0f));
			box->Name("floor");
			Add(box);

			//POSTS
			post = new Post(PxTransform(PxVec3(.0f,5.4f,.0f)));
			post->Color(color_palette[0]);
			Add(post);

			post = new Post(PxTransform(PxVec3(.0f, 5.4f, 50.0f)));
			Add(post);

			//BALL
			ballInit = PxVec3(2.0f, 5.0f, -2.0f);
			ball = new Capsule(PxTransform(ballInit), PxVec2(.22f,.22f));
			ball->Name("ball");
			ball->Color(PxVec3(255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f));
			ball->Material(rubber);
			Add(ball);
		
			//SPEARS LEFT
			float place = 8.0f;
			spear.resize(8);
			spearInit.resize(4);
			spearInitRight.resize(4);
			for (int x = 0; x < 4; x++)
			{		
				spearInit[x] = PxVec3(38.0f, .5f, place);
				spear[x] = new Spear(PxTransform(spearInit[x]), PxVec3(10.0f, .2f, .2f));
				spear[x]->Color(PxVec3(139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f));
				spear[x]->Material(wood);
				Add(spear[x]);			
				place += 15.0f;
			}

			//SPEARS RIGHT
			place = 2.0f;
			for (int x = 4; x < 8; x++)
			{
				spearInitRight[x-4] = PxVec3(-35.0f, .5f, place);
				spear[x] = new Spear(PxTransform(spearInitRight[x-4]), PxVec3(10.0f, .2f, .2f));
				spear[x]->Color(PxVec3(139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f));
				spear[x]->Material(wood);
				Add(spear[x]);
				place += 15.0f;
			}	



			//AXE (ROTATING)
			//CENTRE
			int axeInt = 1;
			//int axeInt = 1000;
			float xPos = 2.0f;
			axe.resize(axeInt);
			base.resize(axeInt);
			rJoint.resize(axeInt);
			for (int x = 0; x < axeInt; x++)
			{
				base[x] = new Axe(PxTransform(PxVec3(2.0f + xPos * x * 5, .5f, 25.0f)), PxVec3(.6f, .04f, .6f));
				base[x]->Color(PxVec3(139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f));
				base[x]->SetKinematic(true);
				Add(base[x]);

				axe[x] = new BoxCompound(PxTransform(PxVec3(xPos * x, .1f, 25.0f)), PxVec3(5.0f, 0.2f, 0.2f));
				axe[x]->Color(PxVec3(139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f));
				Add(axe[x]);

				axe[x]->GetShape()->setLocalPose(PxTransform(PxVec3(0, 0, 0), PxQuat(PxPi, PxVec3(0, 1, 0))));
				rJoint[x] = new RevoluteJoint(base[x], PxTransform(PxVec3(.0f, .4f, .0f), PxQuat(PxPiDivTwo * 45, PxVec3(0.0f, .0f, 1.0f))),
					axe[x], PxTransform(PxVec3(-.0f, .2f, -0.0f), PxQuat(PxPiDivTwo * 45, PxVec3(0.0f, .0f, 1.0f))));

				rJoint[x]->DriveVelocity(4.0f);
			}
			
			//GOAL
			goal = new StaticBox(PxTransform(PxVec3(2.0f, 0.5f, 55.0f)), PxVec3(25.0f, 3.0f, .4f));
			goal->SetTrigger(true);
			goal->Name("goal");
			goal->Color(PxVec3(12.0f / 255.0f, 120.0f / 255.0f, 19.0f / 255.0f));
			Add(goal);


			int res = 30;
			//int res = 40;
			//int res = 60;
			//int res = 80;
			//CLOTH
			cloth = new Cloth(PxTransform(PxVec3(-25.0f, 5.5f, 52.f)), PxVec2(50.f, 5.f), res, res);
			cloth->Color(PxVec3(220.0f / 255.0f, 20.0f / 255.0f, 60.0f / 255.0f));
			Add(cloth);

			//CLOTH
			cloth = new Cloth(PxTransform(PxVec3(-20.0f, 5.5f, 52.f), PxQuat(PxPiDivTwo, PxVec3(0.0f, 1.0f, .0f))), PxVec2(50.f, 5.f), res, res);
			cloth->Color(PxVec3(220.0f / 255.0f, 20.0f / 255.0f, 60.0f / 255.0f));
			Add(cloth);

			//CLOTH
			cloth = new Cloth(PxTransform(PxVec3(25.0f, 5.5f, 2.f), PxQuat(PxPiDivTwo, PxVec3(0.0f, -1.0f, .0f))), PxVec2(50.f, 5.f), res, res);
			cloth->Color(PxVec3(220.0f / 255.0f, 20.0f / 255.0f, 60.0f / 255.0f));
			Add(cloth);

		}

		//void generateField()
		//{
		//	const int holes = 4;
		//	int randomHole[holes] = {};
		//	int min = 30;
		//	int max = 150;

		//	for (int p = 0; p < holes; p++)
		//	{
		//		randomHole[p] = min + (rand() * (int)(max - min) / RAND_MAX);
		//	}

		//	for (int x = 0; x < 17; x++)
		//	{
		//		for (int y = 0; y < 10; y++)
		//		{
		//			float xPos = (4.0f * float(y) - 18.0f);
		//			float zPos = (4.0f * float(x) - 32.0f);

		//			for (int m = 0; m < holes; m++)
		//			{
		//				if (randomHole[m] == count)
		//				{

		//					field->GetShape(count)->setLocalPose(PxTransform(PxVec3(xPos, -5.0f, zPos)));

		//					
		//					//GetShape(count)->setLocalPose(PxTransform(PxVec3(xPos, -5.0f, zPos)));
		//				}
		//			}
		//			count++;
		//		}
		//	}
		//}


		//Custom udpate function
		virtual void CustomUpdate() 
		{
			float fps = 0.0f;
			int timeSpent = glutGet(GLUT_ELAPSED_TIME) / 1000;
			frame++;

			if (timeSpent > 0)
				fps = frame / timeSpent;

			cerr << fps << endl;
			
			srand(time(NULL));
			if (start) {
				start = false;

				while(numbersGood == false)
				{
					move[0] = min + (rand() * (int)(max - min) / RAND_MAX);
					move[1] = min + (rand() * (int)(max - min) / RAND_MAX);
					move[2] = min + (rand() * (int)(max - min) / RAND_MAX);
					move[3] = min + (rand() * (int)(max - min) / RAND_MAX);
					move[4] = min + (rand() * (int)(max - min) / RAND_MAX);
					numbersGood = checkNumbers(move);
				}
			}

			count = count + 1;

			if (count < 390)
			{
				if (count == 200)
				{
					forward = !forward;
					for (int x = 0; x < 8; x++)
					{
						if (forward)spear[x]->Get()->isRigidBody()->setLinearVelocity(PxVec3(.0f, 0.1f, 0.0f));
					}
				}
					
				if (forward) forwardSpeed = 25.0f;
				else forwardSpeed = -25.0f;


				for (int l = 0; l < 5; l++)
				{
					if(move[l] < 4 && forward) spear[move[l]]->Get()->isRigidBody()->setLinearVelocity(PxVec3(-25.0f, 0.0f, 0.0f));
					if (move[l] > 4 && forward) spear[move[l]]->Get()->isRigidBody()->setLinearVelocity(PxVec3(25.0f, 0.0f, 0.0f));
					if (move[l] < 4 && !forward) spear[move[l]]->Get()->isRigidBody()->setLinearVelocity(PxVec3(25.0f, 0.0f, 0.0f));
					if (move[l] > 4 && !forward) spear[move[l]]->Get()->isRigidBody()->setLinearVelocity(PxVec3(-25.0f, 0.0f, 0.0f));
				}	
			}
			else {
				spear[move[0]]->Get()->isRigidBody()->setLinearVelocity(PxVec3(.0f, 0.0f, 0.0f));
				spear[move[1]]->Get()->isRigidBody()->setLinearVelocity(PxVec3(.0f, 0.0f, 0.0f));
				spear[move[2]]->Get()->isRigidBody()->setLinearVelocity(PxVec3(.0f, 0.0f, 0.0f));
				spear[move[3]]->Get()->isRigidBody()->setLinearVelocity(PxVec3(.0f, 0.0f, 0.0f));
				spear[move[4]]->Get()->isRigidBody()->setLinearVelocity(PxVec3(.0f, 0.0f, 0.0f));
				forward = !forward;
				count = 0;
				numbersGood = false;
				start = true;
				resetSpears();
				//generateField();
			}
		}

		/// An example use of key release handling
		void ExampleKeyReleaseHandler()
		{
			cerr << "I am realeased!" << endl;
		}

		/// An example use of key presse handling
		void ExampleKeyPressHandler()
		{
			cerr << "I am pressed!" << endl;
		}

		bool checkNumbers(int move[])
		{
			for (unsigned i = 0; i < 5; i++)
				for (unsigned k = i + 1; k < 5; k++)
					if (move[i] == move[k]) {
						return false;
					}
			return true;
		}

		void resetSpears()
		{
			int count = 0;
			for (int x = 0; x < 4; x++)
			{
				spear[x]->Get()->isRigidBody()->setGlobalPose(PxTransform(spearInit[x]));
			}

			for (int y = 4; y < 8; y++)
			{
				spear[y]->Get()->isRigidBody()->setGlobalPose (PxTransform(spearInitRight[count]));
				count++;
			}
		}
	};
}
