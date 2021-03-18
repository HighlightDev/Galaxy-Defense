#include "PlanarReflectionComponent.h"
#include "Core/GameCore/ACamera.h"

namespace Game {

   PlanarReflectionComponent::PlanarReflectionComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, std::weak_ptr<ACamera> ownerCamera)
      : SceneComponent(gameObjectName, translation, rotation, scale)
      , mOwnerCamera(ownerCamera)
   {

   }

   PlanarReflectionComponent::~PlanarReflectionComponent()
   {

   }

}