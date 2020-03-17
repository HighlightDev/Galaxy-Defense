#include "PhysicsWorld.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AssimpMeshLoader.h"

#include <iostream>

namespace Game
{
   std::unique_ptr<PhysicsPool> PhysicsPool::m_instance = nullptr;

   template <typename Model>
   std::shared_ptr<Skin> PhysicsPoolAllocationPolicy<Model>::AllocateMemory(std::string arg)
   {
      const int32_t countOfBonesInfluencingOnVertex = 3;

      std::shared_ptr<Skin> resultSkin;

      {
         std::string absolutePath = std::move(EngineUtility::ConvertFromRelativeToAbsolutePath(arg));
         Io::MeshLoader::Assimp::AssimpMeshLoader<countOfBonesInfluencingOnVertex> loader(absolutePath);

         VertexArrayObject vao;

         Io::MeshLoader::Assimp::MeshVertexData<countOfBonesInfluencingOnVertex>& meshData = loader.GetMeshData();

         const std::vector<float>& vertices = meshData.Verts;
         const std::vector<uint32_t>& indices = meshData.Indices;

         IndexBufferObject* ibo = nullptr;

         VertexBufferObjectBase* vertexVBO = nullptr;

         if (meshData.bHasIndices)
            ibo = new IndexBufferObject(indices);

         vertexVBO = new VertexBufferObject<float, 3, GL_FLOAT>(vertices, GL_ARRAY_BUFFER, 0, DataCarryFlag::Store);

         vao.AddVBO(vertexVBO);

         vao.AddIndexBuffer(ibo);
         vao.BindBuffersToVao();

         resultSkin = std::make_shared<Skin>(vao);

      }

      return resultSkin;
   }

   template <typename Model>
   void PhysicsPoolAllocationPolicy<Model>::DeallocateMemory(std::shared_ptr<Skin> arg)
   {
      arg->CleanUp();
   }


   PhysicsWorld::PhysicsWorld()
   {
   }


   PhysicsWorld::~PhysicsWorld()
   {
      delete mBroadphase;
      delete mCollisionConfiguration;
      delete mDispatcher;
      delete mSolver;
      delete mWorld;
   }

   void PhysicsWorld::InitPhysics()
   {
      //1
      mBroadphase = new btDbvtBroadphase();

      //2
      mCollisionConfiguration = new btDefaultCollisionConfiguration();
      mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

      //3
      mSolver = new btSequentialImpulseConstraintSolver();

      //4
      mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

      //5
      mWorld->setGravity(btVector3(0, -9.8, 0));
   }

   std::tuple<std::shared_ptr<Skin>, btCollisionShape*> PhysicsWorld::LoadSimpleSkinWithPhysics(const std::string& pathToObject)
   {
      using skin_t = typename PhysicsPool::sharedValue_t;
      std::tuple<skin_t, btCollisionShape*> physicsSkin = std::make_tuple(PhysicsPool::GetInstance()->GetOrAllocateResource(pathToObject), nullptr);

      VertexBufferObjectBase* vertices = std::get<0>(physicsSkin)->GetBuffer()->GetVboByIndex(0);

      VertexBufferObject<float, 3, GL_FLOAT>* verticesVBO = static_cast<VertexBufferObject<float, 3, GL_FLOAT>*>(vertices);
      const std::vector<float>& vert = verticesVBO->GetCastedDataRef();

      btTriangleMesh* mesh = new btTriangleMesh();

      btConvexHullShape* shape = new btConvexHullShape();

      for (size_t i = 0; i < vert.size() / 3; i += 3)
      {
         btVector3 vert1(vert[i], vert[i + 1], vert[i + 2]);
         shape->addPoint(vert1);


         /*btVector3 vert2(vert[i + 3], vert[i + 4], vert[i + 5]);
         btVector3 vert3(vert[i + 6], vert[i + 7], vert[i + 8]);*/

         // mesh->addTriangle(vert1, vert2, vert3);
      }

      std::get<1>(physicsSkin) = shape;

      return physicsSkin;
   }

   btRigidBody* PhysicsWorld::CreateBodyWithMass(float mass, btCollisionShape* shape)
   {
      //1
      btQuaternion rotation;
      rotation.setEulerZYX(0, 0, 0);

      //2
      btVector3 position = btVector3(0, 0, 0);

      //3
      btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation, position));

      //4
      btScalar bodyMass = mass;
      btVector3 bodyInertia;
      shape->calculateLocalInertia(bodyMass, bodyInertia);

      //5
      btRigidBody::btRigidBodyConstructionInfo bodyCI = btRigidBody::btRigidBodyConstructionInfo(bodyMass, motionState, shape, bodyInertia);

      //6
      bodyCI.m_restitution = 1.0f;
      bodyCI.m_friction = 0.5f;

      //7
      auto rigidBody = new btRigidBody(bodyCI);

      //8
      rigidBody->setUserPointer(this);

      //9
      rigidBody->setLinearFactor(btVector3(1, 1, 0));

      mWorld->addRigidBody(rigidBody);

      mBody = rigidBody;

      return rigidBody;
   }

   void PhysicsWorld::Tick(const float deltaTime)
   {
      mWorld->stepSimulation(deltaTime);
      //std::cout.clear();
      //std::cout << "Position Y : " <<  mBody->getWorldTransform().getOrigin().getY() << std::endl;
   }
}
