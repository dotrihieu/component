#include "PhysicsManager.h"
#include "Device_Base.h"

void PhysicsManager::Init()
{
	SafeDelete(instance);
	instance = new PhysicsManager();

	instance->collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	instance->dispatcher = new	btCollisionDispatcher(instance->collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	instance->overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	instance->solver = new btSequentialImpulseConstraintSolver;

	instance->dynamicsWorld = new btDiscreteDynamicsWorld(instance->dispatcher, instance->overlappingPairCache, instance->solver, instance->collisionConfiguration);
	//instance->dynamicsWorld->setGravity(btVector3(0, -1, 0));
	instance->dynamicsWorld->setGravity(btVector3(0, 0, -1));
#if PHYSICS_DEBUG
	int detail = 10; //min 2
	int vertexCount = detail*detail * 2 * 6;
	float* data = new float[vertexCount * 3];
	int index = 0;
	for (int i = 0; i < detail; i++)
	{
		float y = float(i) * 2.0f /float(detail) - 1.0f;
		float yUp = float(i + 1)* 2.0f / float(detail) - 1.0f;
		float r = sqrt(1.0f - pow(y, 2));
		float rUp = sqrt(1.0f - pow(yUp, 2));
		for (int j = 0; j < detail; j++)
		{
			/*
			float x = float(j) * r * 2.0f / float(detail) - r;
			float xNext = float(j + 1) * r * 2.0f / float(detail) - r;
			float xUp = float(j) * rUp * 2.0f / float(detail) - rUp;
			float xUpNext = float(j + 1) * rUp * 2.0f / float(detail) - rUp;
			*/
			float angle = float(j) * 3.1416 * 0.5f / float(detail);
			float angleNext = float(j + 1) * 3.1416 * 0.5f / float(detail);

			float ck = cos(angle)*r*2.0f;
			float x = cos(angle) * ck - r;

			float ckNext = cos(angleNext)*r*2.0f;
			float xNext = cos(angleNext) * ckNext - r;

			float ckUp = cos(angle)*rUp*2.0f;
			float xUp = cos(angle) * ckUp - rUp;

			float ckUpNext = cos(angleNext)*rUp*2.0f;
			float xUpNext = cos(angleNext) * ckUpNext - rUp;

			float z = sqrt(pow(r, 2) - pow(x, 2));
			float zNext = sqrt(pow(r, 2) - pow(xNext, 2));
			float zUp = sqrt(pow(rUp, 2) - pow(xUp, 2));
			float zUpNext = sqrt(pow(rUp, 2) - pow(xUpNext, 2));
			//---------------------------------------
			data[index++] = x; data[index++] = y; data[index++] = z;
			data[index++] = xNext; data[index++] = y; data[index++] = zNext;
			data[index++] = xUpNext; data[index++] = yUp; data[index++] = zUpNext;
			data[index++] = x; data[index++] = y; data[index++] = z;
			data[index++] = xUpNext; data[index++] = yUp; data[index++] = zUpNext;
			data[index++] = xUp; data[index++] = yUp; data[index++] = zUp;
			//---------------------------------------
			data[index++] = -x; data[index++] = y; data[index++] = -z;
			data[index++] = -xNext; data[index++] = y; data[index++] = -zNext;
			data[index++] = -xUpNext; data[index++] = yUp; data[index++] = -zUpNext;
			data[index++] = -x; data[index++] = y; data[index++] = -z;
			data[index++] = -xUpNext; data[index++] = yUp; data[index++] = -zUpNext;
			data[index++] = -xUp; data[index++] = yUp; data[index++] = -zUp;
		}
	}
	instance->vboSphere = new VBO(VBO::VBO_TARGET_ARRAY_BUFFER);
	//instance->vboSphere->vertexCount = vertexCount;
	instance->vboSphere->usage = VBO::VBO_USAGE_STATIC_DRAW;
	instance->vboSphere->SetAttributeOffset(ATTRIBUTE_FLOAT3_POSITION, 0);
	Device_Base::GetInstance()->CreateVBO(instance->vboSphere, (char*)data, vertexCount * 3 * sizeof(float));
	SafeDeleteArray(data);
#endif
}

void PhysicsManager::Release()
{
	SafeDelete(instance);
}

PhysicsManager::PhysicsManager()
{

}

PhysicsManager::~PhysicsManager()
{
	//cleanup in the reverse order of creation/initialization

	///-----cleanup_start-----

	//remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j<collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();

	///-----cleanup_end-----
}

void PhysicsManager::RenderDebug()
{
	//glLineWidth(2.5);
	Device_Base::GetInstance()->DisableDepthTest();
	
	Program* physicsProgram = ShaderManager::GetInstance()->GetProgramFromFile("Physics_vs.txt", "Physics_fs.txt");
	Device_Base::GetInstance()->UseProgram(physicsProgram);
	for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
	{
		Device_Base::GetInstance()->PushMatrix();
		Device_Base::GetInstance()->SetIdentityMatrix();
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);
			//printf("world pos = %f,%f,%f\n", float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			Matrix matrixTransform;
			matrixTransform.SetIdentity();
			trans.getOpenGLMatrix((btScalar*)&matrixTransform);
			//Device_Base::GetInstance()->Translate(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			Device_Base::GetInstance()->Transform(matrixTransform);
			switch (obj->getCollisionShape()->getShapeType())
			{
			case BOX_SHAPE_PROXYTYPE:
				{
					Vector3f vertices[24];
					for (int i = 0; i < 12; i++)
					{
						btVector3 A, B;
						((btBoxShape*)obj->getCollisionShape())->getEdge(i, A, B);
						vertices[2 * i].x = A.x();
						vertices[2 * i].y = A.y();
						vertices[2 * i].z = A.z();
						vertices[2 * i + 1].x = B.x();
						vertices[2 * i + 1].y = B.y();
						vertices[2 * i + 1].z = B.z();
					}
					//-----------------------------------------------------
					Device_Base::GetInstance()->UnbindVBO(VBO::VBO_TARGET_ARRAY_BUFFER);
					//-------------------------------------------
					Device_Base::GetInstance()->SetAttributePointer(ATTRIBUTE_FLOAT3_POSITION, vertices, sizeof(float) * 3);
					Device_Base::GetInstance()->UpdateVertexAttribPointer();

					Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_MODEL, Device_Base::GetInstance()->GetMatrixWorld());
					Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_VIEW, Device_Base::GetInstance()->GetMatrixView());
					Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_PROJECTION, Device_Base::GetInstance()->GetMatrixProjection());
					Vector4f color(1.0f,0.0f,1.0f,1.0f);
					Device_Base::GetInstance()->SetUniformVec4f(UNIFORM_FLOAT4_COLOR_AMBIENT, color);

					Device_Base::GetInstance()->DrawArray(Device_Base::PRIMITIVE_LINE, 0, 24);
					//----------------------------------------------------------
				}
				break;
			case SPHERE_SHAPE_PROXYTYPE:
				float scale = ((btSphereShape*)obj->getCollisionShape())->getRadius();
				Device_Base::GetInstance()->Scale(scale, scale, scale);
				Device_Base::GetInstance()->BindVBO(vboSphere);
				Device_Base::GetInstance()->UpdateVertexAttribPointer();
				Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_MODEL, Device_Base::GetInstance()->GetMatrixWorld());
				Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_VIEW, Device_Base::GetInstance()->GetMatrixView());
				Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_PROJECTION, Device_Base::GetInstance()->GetMatrixProjection());
				Device_Base::GetInstance()->SetPolygonMode(Device_Base::POLYGON_LINE);
				Device_Base::GetInstance()->DrawArray(Device_Base::PRIMITIVE_TRIANGLE_LIST, 0, vboSphere->vertexCount);
				Device_Base::GetInstance()->SetPolygonMode(Device_Base::POLYGON_FILL);
				break;
			}
		}
		Device_Base::GetInstance()->PopMatrix();
	}
}
btRigidBody* PhysicsManager::AddBoxShape(const Vector3f &origin, const Vector3f &size, const float &massIn)
{
	btCollisionShape* boxShape = new btBoxShape(btVector3(btScalar(size.x), btScalar(size.y), btScalar(size.z)));

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!

	collisionShapes.push_back(boxShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(origin.x, origin.y, origin.z));

	btScalar mass(massIn);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		boxShape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, boxShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);

	return body;
}
btRigidBody* PhysicsManager::AddSphereShape(const Vector3f &origin, const float &radius, const float &massIn)
{
	btCollisionShape* colShape = new btSphereShape(btScalar(radius));
	collisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar	mass(massIn);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(origin.x, origin.y, origin.z));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	dynamicsWorld->addRigidBody(body);

	return body;
}
void PhysicsManager::Update(const float &dt)
{
	dynamicsWorld->stepSimulation(dt);
}