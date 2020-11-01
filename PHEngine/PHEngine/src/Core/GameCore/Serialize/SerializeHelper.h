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
   class StateMachine;
   class Scene;
   class Component;
   class StaticMeshComponent;

   class SerializeHelper
   {
   public:
      
      static std::shared_ptr<SerializeDataMesh> GetSerializedDataStaticMesh(const StaticMeshComponent* component);

      static SerializeDataMaterial GetSerializeDataMaterial(std::shared_ptr<IMaterial> materialInstance);

      static std::shared_ptr<SerializeDataPhysicsComponent> GetSerializeDataPhysicsComponent(PhysicsComponent* component);

      static std::shared_ptr<Actor> CreateActorFromSerializedData(const SerializeDataActor& data);

      static std::shared_ptr<StateMachine> CreateFsmFromSerializedData(std::shared_ptr<SerializeDataStateMachine> data);

      static std::shared_ptr<Component> CreateComponentFromSerializedData(Scene* scene, std::shared_ptr<SerializeDataBase> data);

      static IMaterial* CreateMaterialFromSerializedData(const SerializeDataMaterial& materialData);
   };

}

