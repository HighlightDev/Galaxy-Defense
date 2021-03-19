#pragma once
#include "SceneComponent.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

namespace Game {

   class ACamera;
   
   class PlanarReflectionComponent :
      public SceneComponent
   {
      std::shared_ptr<ACamera> mOwnerCamera;

      ::Graphics::ViewPortInfo mFboViewPortInfo;

   public:

      PlanarReflectionComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation,
         glm::vec3 scale, ACamera* ownerCamera, const ::Graphics::ViewPortInfo& fboViewPortInfo);

      virtual ~PlanarReflectionComponent();

      class PlanarReflectionProxy;
      std::shared_ptr<PlanarReflectionProxy> CreatePlanarReflectionProxy();

      virtual void Tick(const float deltaTime) override;

   };
}

