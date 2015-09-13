#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "Common.h"
#include "btBulletDynamicsCommon.h"
#include "xmath.h"
#define PHYSICS_DEBUG 1
#if PHYSICS_DEBUG
#include "VBO.h"
#endif

class PhysicsManager : public Singleton<PhysicsManager>
{
private:
	btDefaultCollisionConfiguration* collisionConfiguration;

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher;

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache;

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
#if PHYSICS_DEBUG
	VBO* vboSphere;
#endif
public:
	static void Init();
	static void Release();
	PhysicsManager();
	~PhysicsManager();
	btRigidBody* AddBoxShape(const Vector3f &origin, const Vector3f &size, const float &massIn);
	btRigidBody* AddSphereShape(const Vector3f &origin, const float &radius, const float &massIn);
	void RenderDebug();
	void Update(const float &dt);
	btDiscreteDynamicsWorld* GetDynamicWorld(){ return dynamicsWorld; }
};
#endif