#pragma once
#include "SceneComponent.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceController.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Resources;
using namespace Graphics::Texture;

namespace Graphics{

   class PlanarReflectionProxy;
}

namespace EngineCore {

   class ACamera;
   struct PlanarReflectionComponentData;

   using Graphics::PlanarReflectionProxy;
   
   class PlanarReflectionComponent 
      : public SceneComponent
      , public IDeferredResourceCreator
   {
      size_t mPlanarReflectionSceneProxyId = -1;

      glm::vec4 mReflectionPlane;

      ACamera* mOwnerCamera;

      ::Graphics::ViewPortInfo mRenderTargetViewPortInfo;

      std::shared_ptr<DeferredResourceController<std::shared_ptr<ITexture>, eResourceType::TEXTURE>> mPlanarReflectionDeferredController;

   public:

      PlanarReflectionComponent(const PlanarReflectionComponentData& data);

      virtual ~PlanarReflectionComponent();

      std::shared_ptr<PlanarReflectionProxy> CreatePlanarReflectionProxy();

      virtual void Tick(const float deltaTime) override;

      virtual void OnPostInitialized() override;

      virtual void PostLevelInit() override;

      ::Graphics::ViewPortInfo GetRenderTargetViewPortInfo() const;

      ACamera* GetOwnerCamera() const;

      size_t GetSceneProxyId() const;

      glm::vec4 GetReflectionPlane() const;

      void SetSceneProxyId(const size_t sceneProxyId);

      virtual ComponentType GetComponentType() const override;

      void SyncDataWithRenderThread();

      virtual std::shared_ptr<IDeferredResourceBase> GetDeferredResource() override;

   private:

      void UpdateReflectionPlane();
   };
}

