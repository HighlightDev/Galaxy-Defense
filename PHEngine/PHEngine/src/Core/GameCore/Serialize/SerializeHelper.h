#pragma once

#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"


namespace Graphics {
   class IMaterial;
}

namespace EnginePhysics {
   class PhysicsComponent;
}

namespace EngineCore {

   using Graphics::IMaterial;
   using EnginePhysics::PhysicsComponent;

   class Actor;
   class ACamera;
   class Tweener;
   class Scene;
   class Component;
   class StaticMeshComponent;
   class SkeletalMeshComponent;
   class PlanarReflectionComponent;

   class SerializeHelper
   {
   public:
      
      static std::shared_ptr<SerializeDataCamera> GetSerializedDataCamera(const ACamera* camera);

      static std::shared_ptr<SerializeDataStaticMesh> GetSerializedDataStaticMesh(const StaticMeshComponent* component);

      static std::shared_ptr<SerializeDataSkeletalMesh> GetSerializedDataSkeletalMesh(const SkeletalMeshComponent* component);

      static std::shared_ptr<SerializeDataPlanarReflectionComponent> GetSerializedDataPlanarReflectionComponent(const PlanarReflectionComponent* component);

      static SerializeDataMaterial GetSerializeDataMaterial(std::shared_ptr<IMaterial> materialInstance);

      static std::shared_ptr<SerializeDataPhysicsComponent> GetSerializeDataPhysicsComponent(const PhysicsComponent* component);

      static std::shared_ptr<SerializeDataPhysicsShape> GetSerializePhysicsShapeData(PhysicsShapeBase* physicsShape);

      static std::shared_ptr<ACamera> CreateCameraFromSerializedData(std::shared_ptr<Scene> scene, std::shared_ptr<SerializeDataCamera> data, bool& outIsMainSceneCamera);

      static std::shared_ptr<Actor> CreateActorFromSerializedData(const SerializeDataActor& data);

      static std::shared_ptr<Tweener> CreateTweenerFromSerializedData(std::shared_ptr<SerializeDataTweener> data);

      static std::shared_ptr<Component> CreateComponentFromSerializedData(std::shared_ptr<Scene> scene, std::shared_ptr<SerializeDataBase> data);

      static PhysicsShapeBase* CreatePhysicsShape(SerializeDataPhysicsShape* serDataShape);

      static std::shared_ptr<IMaterial> CreateMaterialFromSerializedData(const SerializeDataMaterial& materialData);

      static std::vector<std::string> GetSerializedAllocatedResources(const SerializeAllocatedResources& allocatedResources);
   };

}

