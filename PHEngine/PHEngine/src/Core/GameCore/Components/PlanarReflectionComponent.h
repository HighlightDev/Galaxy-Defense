#pragma once
#include "SceneComponent.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

namespace Graphics{

   class PlanarReflectionProxy;
}

namespace Game {

   class ACamera;

   using Graphics::PlanarReflectionProxy;
   
   class PlanarReflectionComponent :
      public SceneComponent
   {
      ACamera* mOwnerCamera;

      ::Graphics::ViewPortInfo mRenderTargetViewPortInfo;

      size_t mPlanarReflectionSceneProxyId = -1;

   public:

      PlanarReflectionComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation,
         glm::vec3 scale, ACamera* ownerCamera, const ::Graphics::ViewPortInfo& fboViewPortInfo);

      virtual ~PlanarReflectionComponent();

      std::shared_ptr<PlanarReflectionProxy> CreatePlanarReflectionProxy();

      virtual void Tick(const float deltaTime) override;

      ::Graphics::ViewPortInfo GetRenderTargetViewPortInfo() const;

      ACamera* GetOwnerCamera() const;

      size_t GetSceneProxyId() const;

      void SetSceneProxyId(const size_t sceneProxyId);

      virtual ComponentType GetComponentType() const override;
   };
}

