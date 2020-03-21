#include "PhysicsWorld.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AssimpMeshLoader.h"


#include <iostream>

namespace Game
{
   std::unique_ptr<PhysicsPool> PhysicsPool::m_instance = nullptr;

   glm::vec3 PhysicsWorld::GetBodyWorldTransform()
   {
      auto variable = mBody->getWorldTransform().getOrigin();
      return glm::vec3(variable.getX(), variable.getY(), variable.getZ());
   }

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
      }

      std::get<1>(physicsSkin) = shape;

      return physicsSkin;
   }

   btCollisionShape* PhysicsWorld::LoadFloor()
   {
      auto vertices = std::vector<float>({
                -1.0f, 0.0, -1.0f, // top-right
                1.0f, 0.0 -1.0f, // top-left
                1.0f, 0.0, 1.0f, // bottom-left
                1.0f, 0.0, 1.0f, // bottom-left
                -1.0f, 0.0, 1.0f, // bottom-right
                -1.0f, 0.0, -1.0f, // top-right
         });


      btConvexHullShape* shape = new btConvexHullShape();

      for (size_t i = 0; i < vertices.size() / 3; i += 3)
      {
         btVector3 vert1(vertices[i] * 20, vertices[i + 1] * 20, vertices[i + 2] * 20);
         shape->addPoint(vert1);
      }

      return shape;
   }

   btRigidBody* PhysicsWorld::CreateBodyWithMass(float mass, btCollisionShape* shape, bool bFall)
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
      if (bFall)
      {
         rigidBody->setLinearFactor(btVector3(1, 1, 0));

         mBody = rigidBody;
      }
      else
      {
         rigidBody->setLinearFactor(btVector3(1, 0, 0));
         mFloor = rigidBody;
      }

      mWorld->addRigidBody(rigidBody);
      return rigidBody;
   }

   void PhysicsWorld::Tick(const float deltaTime)
   {
      mWorld->stepSimulation(deltaTime);
     /* std::cout.clear();
      std::cout << "Delta Time:" << deltaTime << std::endl << "Position Y : " << mFloor->getWorldTransform().getOrigin().getY() << std::endl;*/
   }
}
