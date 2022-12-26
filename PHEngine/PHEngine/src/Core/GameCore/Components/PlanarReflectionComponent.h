#pragma once
#include "SceneComponent.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceController.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Resources;
using namespace Graphics::Texture;

namespace Graphics
{
   class PlanarReflectionProxy;
}

namespace EngineCore
{
   class ACamera;
   struct PlanarReflectionComponentData;

   using Graphics::PlanarReflectionProxy;

   class PlanarReflectionComponent
       : public SceneComponent,
         public IDeferredResourceCreator
   {
      size_t mPlanarReflectionSceneProxyId = -1;

      glm::vec4 mReflectionPlane;

      ACamera *mOwnerCamera;

      ::Graphics::ViewPortInfo mRenderTargetViewPortInfo;

      std::shared_ptr<DeferredResourceController<std::shared_ptr<ITexture>, eResourceType::TEXTURE>> mPlanarReflectionDeferredController;

   public:
      PlanarReflectionComponent(const PlanarReflectionComponentData &data);

      ~PlanarReflectionComponent() override;

      std::shared_ptr<PlanarReflectionProxy> CreatePlanarReflectionProxy();

      void Tick(const float deltaTime) override;

      void OnPostInitialized() override;

      void PostLevelInit() override;

      ::Graphics::ViewPortInfo GetRenderTargetViewPortInfo() const;

      ACamera *GetOwnerCamera() const;

      size_t GetSceneProxyId() const;

      glm::vec4 GetReflectionPlane() const;

      void SetSceneProxyId(const size_t sceneProxyId);

      eComponentType GetComponentType() const override;

      void SyncDataWithRenderThread();

      std::shared_ptr<IDeferredResourceBase> GetDeferredResource() override;

   private:
      void UpdateReflectionPlane();
   };
}
