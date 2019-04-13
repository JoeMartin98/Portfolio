#pragma once

#include "PhysicsEngine.h"
#include "MyPhysicsEngine.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	///Plane class
	class Plane : public StaticActor
	{
	public:
		//A plane with default paramters: XZ plane centred at (0,0,0)
		Plane(PxVec3 normal=PxVec3(0.f, 1.f, 0.f), PxReal distance=0.f) 
			: StaticActor(PxTransformFromPlaneEquation(PxPlane(normal, distance)))
		{
			CreateShape(PxPlaneGeometry());
		}
	};

	///Sphere class
	class Sphere : public DynamicActor
	{
	public:
		//a sphere with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m
		// - denisty: 1kg/m^3
		Sphere(const PxTransform& pose=PxTransform(PxIdentity), PxReal radius=1.f, PxReal density=1.f) 
			: DynamicActor(pose)
		{ 
			CreateShape(PxSphereGeometry(radius), density);
		}
	};

	///Box class
	class Box : public DynamicActor
	{
	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		Box(const PxTransform& pose=PxTransform(PxIdentity), PxVec3 dimensions=PxVec3(.5f,.5f,.5f), PxReal density=1.f) 
			: DynamicActor(pose)
		{ 
			CreateShape(PxBoxGeometry(dimensions), density);
		}
	};

	class BoxCompound : public DynamicActor
	{
	public:
		//a Box with default parameters:
		// - pose in 0,0,0
		// - dimensions: 1m x 1m x 1m
		// - denisty: 1kg/m^3
		BoxCompound(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.5f, .5f, .5f), PxReal density = 1.f)
			: DynamicActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);
		}
	};

	///Box class
	class Axe : public DynamicActor
	{
	public:
		Axe(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(PxIdentity), PxReal density = 1.f)
			: DynamicActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);
		}
	};

	//Box class
	class StaticBox : public StaticActor
	{
	public:
		StaticBox(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(PxIdentity), PxReal density = 1.f)
			: StaticActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);
		}
	};

	class holder : public StaticActor
	{
	public:
		holder(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.5f, .2f, 35.0f), PxReal density = 1.f)
			: StaticActor(pose)
		{
			//CreateShape(PxBoxGeometry(.1f, .1f, .6f), density);			
			//CreateShape(PxBoxGeometry(.1f, .2f, .1f), density);
			//CreateShape(PxBoxGeometry(.1f, .2f, .1f), density);
			//GetShape(0)->setLocalPose(PxTransform(PxVec3(-1.2f, .2f, -0.2f)));
			//GetShape(1)->setLocalPose(PxTransform(PxVec3(-1.2f, .5f,0.3f)));
			//GetShape(2)->setLocalPose(PxTransform(PxVec3(-1.2f, .5f,-0.7f)));
			//CreateShape(PxBoxGeometry(.1f, .1f, .6f), density);			
			//CreateShape(PxBoxGeometry(.1f, .2f, .1f), density);
			//CreateShape(PxBoxGeometry(.1f, .2f, .1f), density);
			//GetShape(3)->setLocalPose(PxTransform(PxVec3(-1.2f, 1.1f, -0.2f)));
			//GetShape(4)->setLocalPose(PxTransform(PxVec3(-1.2f, .8f, 0.3f)));
			//GetShape(5)->setLocalPose(PxTransform(PxVec3(-1.2f, .8f, -0.7f)));

			CreateShape(PxBoxGeometry(dimensions), density);
			GetShape(0)->setLocalPose(PxTransform(pose));
		}
	};

	class Field : public StaticActor
	{
	public:

		Field(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(2.0f, .3f, 2.0f), PxReal density = 1.f)
			: StaticActor(pose)
		{
			//Creates 170 tiles for the playing field
			srand(time(NULL));
			int count = 0;
			const int holes = 16;
			int randomHole [holes] = {};
			int min = 30;
			int max = 150;
			for (int p = 0; p < holes; p++)
			{
				randomHole[p] = min + (rand() * (int)(max - min) / RAND_MAX);
			}
			for (int x = 0; x < 17; x++)
			{
				for (int y = 0; y < 10; y++)
				{
					CreateShape(PxBoxGeometry(PxVec3(dimensions)), density);
					float xPos = (4.0f * float(y) - 18.0f);
					float zPos = (4.0f * float(x) - 32.0f);
					GetShape(count)->setLocalPose(PxTransform(PxVec3(xPos, .0f, zPos)));
					for (int m = 0; m < holes; m++)
					{
						if (randomHole[m] == count)
						{
							GetShape(count)->setLocalPose(PxTransform(PxVec3(xPos, -5.0f, zPos)));
						}
					}
					count++;
				}			
			}		
		}
	};

	class Spear : public DynamicActor
	{
	public:
		Spear(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(PxIdentity), PxReal density = 1.f)
			: DynamicActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);
			CreateShape(PxBoxGeometry(dimensions.x/20, dimensions.y, dimensions.z + 1.0f), density);
			GetShape(1)->setLocalPose(PxTransform(PxVec3(-10.0f, .0f, .0f)));
			CreateShape(PxBoxGeometry(dimensions.x/20, dimensions.y, dimensions.z + 1.0f), density);
			GetShape(2)->setLocalPose(PxTransform(PxVec3(10.0f, .0f, .0f)));
		}
	};

	///Post class
	class Post : public StaticActor
	{
	public:
		Post(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.2f, 5.5f, .2f), PxReal density = 1.f)
			: StaticActor(pose)
		{
			CreateShape(PxBoxGeometry(dimensions), density);
			CreateShape(PxBoxGeometry(dimensions), density);
			GetShape(1)->setLocalPose(PxTransform(PxVec3(4.0f, .0f, .0f)));
			CreateShape(PxBoxGeometry(PxVec3(2.0f, .2f, .2f)), density);
			GetShape(2)->setLocalPose(PxTransform(PxVec3(2.0f, -1.6f, .0f)));
		}
	};



	class Capsule : public DynamicActor
	{
	public:
		Capsule(const PxTransform& pose=PxTransform(PxIdentity), PxVec2 dimensions=PxVec2(PxIdentity), PxReal density=1.f)
			: DynamicActor(pose)
		{
			CreateShape(PxCapsuleGeometry(dimensions.x, dimensions.y), density);
		}
	};

	///The ConvexMesh class
	class ConvexMesh : public DynamicActor
	{
	public:
		//constructor
		ConvexMesh(const std::vector<PxVec3>& verts, const PxTransform& pose=PxTransform(PxIdentity), PxReal density=1.f)
			: DynamicActor(pose)
		{
			PxConvexMeshDesc mesh_desc;
			mesh_desc.points.count = (PxU32)verts.size();
			mesh_desc.points.stride = sizeof(PxVec3);
			mesh_desc.points.data = &verts.front();
			mesh_desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
			mesh_desc.vertexLimit = 256;

			CreateShape(PxConvexMeshGeometry(CookMesh(mesh_desc)), density);
		}

		//mesh cooking (preparation)
		PxConvexMesh* CookMesh(const PxConvexMeshDesc& mesh_desc)
		{
			PxDefaultMemoryOutputStream stream;

			if(!GetCooking()->cookConvexMesh(mesh_desc, stream))
				throw new Exception("ConvexMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createConvexMesh(input);
		}
	};

	
	///The TriangleMesh class
	class TriangleMesh : public StaticActor
	{
	public:
		//constructor
		TriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& trigs, const PxTransform& pose=PxTransform(PxIdentity))
			: StaticActor(pose)
		{
			PxTriangleMeshDesc mesh_desc;
			mesh_desc.points.count = (PxU32)verts.size();
			mesh_desc.points.stride = sizeof(PxVec3);
			mesh_desc.points.data = &verts.front();
			mesh_desc.triangles.count = (PxU32)trigs.size();
			mesh_desc.triangles.stride = 3*sizeof(PxU32);
			mesh_desc.triangles.data = &trigs.front();

			CreateShape(PxTriangleMeshGeometry(CookMesh(mesh_desc)));
		}

		//mesh cooking (preparation)
		PxTriangleMesh* CookMesh(const PxTriangleMeshDesc& mesh_desc)
		{
			PxDefaultMemoryOutputStream stream;

			if(!GetCooking()->cookTriangleMesh(mesh_desc, stream))
				throw new Exception("TriangleMesh::CookMesh, cooking failed.");

			PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

			return GetPhysics()->createTriangleMesh(input);
		}
	};

	//Distance joint with the springs switched on
	class DistanceJoint : public Joint
	{
	public:
		DistanceJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
		{
			PxRigidActor* px_actor0 = 0;
			if (actor0)
				px_actor0 = (PxRigidActor*)actor0->Get();

			joint = (PxJoint*)PxDistanceJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);
			joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
			((PxDistanceJoint*)joint)->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);
			Damping(1.f);
			Stiffness(1.f);
		}

		void Stiffness(PxReal value)
		{
			((PxDistanceJoint*)joint)->setStiffness(value);
		}

		PxReal Stiffness()
		{
			return ((PxDistanceJoint*)joint)->getStiffness();
		}

		void Damping(PxReal value)
		{
			((PxDistanceJoint*)joint)->setDamping(value);
		}

		PxReal Damping()
		{
			return ((PxDistanceJoint*)joint)->getDamping();
		}
	};

	///Revolute Joint
	class RevoluteJoint : public Joint
	{
	public:
		RevoluteJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
		{
			PxRigidActor* px_actor0 = 0;
			if (actor0)
				px_actor0 = (PxRigidActor*)actor0->Get();

			joint = PxRevoluteJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);
			joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
		}

		void DriveVelocity(PxReal value)
		{
			//wake up the attached actors
			PxRigidDynamic *actor_0, *actor_1;
			((PxRevoluteJoint*)joint)->getActors((PxRigidActor*&)actor_0, (PxRigidActor*&)actor_1);
			if (actor_0)
			{
				if (actor_0->isSleeping())
					actor_0->wakeUp();
			}
			if (actor_1)
			{
				if (actor_1->isSleeping())
					actor_1->wakeUp();
			}
			((PxRevoluteJoint*)joint)->setDriveVelocity(value);
			((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		}

		PxReal DriveVelocity()
		{
			return ((PxRevoluteJoint*)joint)->getDriveVelocity();
		}

		void SetLimits(PxReal lower, PxReal upper)
		{
			((PxRevoluteJoint*)joint)->setLimit(PxJointAngularLimitPair(lower, upper));
			((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		}
	};

//	class PrismaticJoint : public Joint
//	{
//	public:
//		PrismaticJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
//		{
//			PxRigidActor* px_actor0 = 0;
//			if (actor0)
//				px_actor0 = (PxRigidActor*)actor0->Get();
//
////			joint->->setLimit(PxJointLimitPair(-10.0f, 20.0f, 0.01f);
////			joint->setPrismaticJointFlag(PxPrismaticJointFlag::eLIMIT_ENABLED, true);
//			
//		}
//		
//		void SetLimits(PxReal lower, PxReal upper)
//		{
//			((PxPrismaticJoint*)joint)->setLimit(PxJointLinearLimitPair(-10.0f, 20.0f, 0.01f));
//			((PxPrismaticJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
//		}
//	};

	class Cloth : public Actor
	{
		PxClothMeshDesc mesh_desc;

	public:
		//constructor
		Cloth(PxTransform pose = PxTransform(PxIdentity), const PxVec2& size = PxVec2(1.f, 1.f), PxU32 width = 1, PxU32 height = 1, bool fix_top = true)
		{
			//prepare vertices
			PxReal w_step = size.x / width;
			PxReal h_step = size.y / height;

			PxClothParticle* vertices = new PxClothParticle[(width + 1)*(height + 1) * 4];
			PxU32* quads = new PxU32[width*height * 4];

			for (PxU32 j = 0; j < (height + 1); j++)
			{
				for (PxU32 i = 0; i < (width + 1); i++)
				{
					PxU32 offset = i + j * (width + 1);
					vertices[offset].pos = PxVec3(w_step*i, 0.f, h_step*j);
					if (fix_top && (j == 0)) //fix the top row of vertices
						vertices[offset].invWeight = 0.f;
					else
						vertices[offset].invWeight = 1.f;
				}

				for (PxU32 j = 0; j < height; j++)
				{
					for (PxU32 i = 0; i < width; i++)
					{
						PxU32 offset = (i + j * width) * 4;
						quads[offset + 0] = (i + 0) + (j + 0)*(width + 1);
						quads[offset + 1] = (i + 1) + (j + 0)*(width + 1);
						quads[offset + 2] = (i + 1) + (j + 1)*(width + 1);
						quads[offset + 3] = (i + 0) + (j + 1)*(width + 1);
					}
				}
			}

			//init cloth mesh description
			mesh_desc.points.data = vertices;
			mesh_desc.points.count = (width + 1)*(height + 1);
			mesh_desc.points.stride = sizeof(PxClothParticle);

			mesh_desc.invMasses.data = &vertices->invWeight;
			mesh_desc.invMasses.count = (width + 1)*(height + 1);
			mesh_desc.invMasses.stride = sizeof(PxClothParticle);

			mesh_desc.quads.data = quads;
			mesh_desc.quads.count = width * height;
			mesh_desc.quads.stride = sizeof(PxU32) * 4;

			//create cloth fabric (cooking)
			PxClothFabric* fabric = PxClothFabricCreate(*GetPhysics(), mesh_desc, PxVec3(0, -1, 0));

			//create cloth
			actor = (PxActor*)GetPhysics()->createCloth(pose, *fabric, vertices, PxClothFlags());
			//collisions with the scene objects
			((PxCloth*)actor)->setClothFlag(PxClothFlag::eSCENE_COLLISION, true);

			colors.push_back(default_color);
			actor->userData = new UserData(&colors.back(), &mesh_desc);
		}

		~Cloth()
		{
			delete (UserData*)actor->userData;
		}
	};
}