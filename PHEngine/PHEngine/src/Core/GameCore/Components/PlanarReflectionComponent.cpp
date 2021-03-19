#include "PlanarReflectionComponent.h"
#include "Core/GameCore/ACamera.h"

namespace Game {

   PlanarReflectionComponent::PlanarReflectionComponent(const std::string& gameObjectName, glm::vec3 translation,
      glm::vec3 rotation, glm::vec3 scale, ACamera* ownerCamera, const ::Graphics::ViewPortInfo& fboViewPortInfo)
      : SceneComponent(gameObjectName, translation, rotation, scale)
      , mOwnerCamera(ownerCamera)
      , mFboViewPortInfo(fboViewPortInfo)
   {
   }

   PlanarReflectionComponent::~PlanarReflectionComponent()
   {
   }

   void PlanarReflectionComponent::Tick(const float deltaTime)
   {
      if (mIsEnabled)
      {
         if (bTransformationDirty)
         {
            UpdateRelativeMatrix(glm::mat4(1));
         }
      }
   }

}