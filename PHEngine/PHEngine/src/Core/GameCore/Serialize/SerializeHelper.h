#pragma once

#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"


namespace Graphics {
   class IMaterial;
}

namespace EnginePhysics {
   class PhysicsComponent;
}

namespace Game {

   using Graphics::IMaterial;
   using EnginePhysics::PhysicsComponent;

   class Actor;
   class Tweener;
   class Scene;
   class Component;
   class StaticMeshComponent;
   class SkeletalMeshComponent;

   class SerializeHelper
   {
   public:
      
      static std::shared_ptr<SerializeDataStaticMesh> GetSerializedDataStaticMesh(const StaticMeshComponent* component);

      static std::shared_ptr<SerializeDataSkeletalMesh> GetSerializedDataSkeletalMesh(const SkeletalMeshComponent* component);

      static SerializeDataMaterial GetSerializeDataMaterial(std::shared_ptr<IMaterial> materialInstance);

      static std::shared_ptr<SerializeDataPhysicsComponent> GetSerializeDataPhysicsComponent(PhysicsComponent* component);

      static std::shared_ptr<Actor> CreateActorFromSerializedData(const SerializeDataActor& data);

      static std::shared_ptr<Tweener> CreateTweenerFromSerializedData(std::shared_ptr<SerializeDataTweener> data);

      static std::shared_ptr<Component> CreateComponentFromSerializedData(Scene* scene, std::shared_ptr<SerializeDataBase> data);

      static PhysicsShapeBase* CreatePhysicsShape(const SerializeDataPhysicsComponent* serData);

      static IMaterial* CreateMaterialFromSerializedData(const SerializeDataMaterial& materialData);
   };

}

