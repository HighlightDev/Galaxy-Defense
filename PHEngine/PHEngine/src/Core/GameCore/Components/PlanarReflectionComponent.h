#pragma once
#include "SceneComponent.h"

namespace Game {

   class ACamera;
   class PlanarReflectionProxy;

   class PlanarReflectionComponent :
      public SceneComponent
   {
      std::weak_ptr<ACamera> mOwnerCamera;

   public:

      PlanarReflectionComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, std::weak_ptr<ACamera> ownerCamera);

      virtual ~PlanarReflectionComponent();

      std::shared_ptr<PlanarReflectionProxy> CreatePlanarReflectionProxy();

   };
}

