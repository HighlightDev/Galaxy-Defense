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

   class SerializeHelper
   {
   public:
      
      static SerializeDataMaterial GetSerializeDataMaterial(std::shared_ptr<IMaterial> materialInstance);

      static std::shared_ptr<SerializeDataPhysicsComponent> GetSerializeDataPhysicsComponent(PhysicsComponent* component);
   };

}

