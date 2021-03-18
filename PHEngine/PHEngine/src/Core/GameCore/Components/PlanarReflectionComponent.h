#pragma once
#include "SceneComponent.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

namespace Game {

   class ACamera;
   
   class PlanarReflectionComponent :
      public SceneComponent
   {
      std::weak_ptr<ACamera> mOwnerCamera;

      ::Graphics::ViewPortInfo mFboViewPortInfo;

   public:

      PlanarReflectionComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation,
         glm::vec3 scale, std::weak_ptr<ACamera> ownerCamera, const ::Graphics::ViewPortInfo& fboViewPortInfo);

      virtual ~PlanarReflectionComponent();

      class PlanarReflectionProxy;
      std::shared_ptr<PlanarReflectionProxy> CreatePlanarReflectionProxy();

   };
}

