#include "PlanarReflectionComponent.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"

using namespace Graphics;
using namespace EngineMath;

namespace EngineCore {

   PlanarReflectionComponent::PlanarReflectionComponent(const PlanarReflectionComponentData& data)
      : SceneComponent(data.GameObjectName, data.m_translation, data.m_eulerRotationDegrees, data.m_scale)
      , mReflectionPlane()
      , mOwnerCamera(data.m_ownerCamera)
      , mRenderTargetViewPortInfo(data.m_fboViewPortInfo)
      , mPlanarReflectionDeferredController(std::make_shared<DeferredResourceController<std::shared_ptr<ITexture>, eResourceType::TEXTURE>>())
   {
   }

   PlanarReflectionComponent::~PlanarReflectionComponent()
   {
   }

   void PlanarReflectionComponent::UpdateReflectionPlane()
   {
      glm::vec4 positionOnPlane = m_relativeMatrix * glm::vec4(0, 0, 0, 1);
      const glm::vec3 defaultNormal = -AXIS_UP;
      glm::vec4 normal = m_relativeMatrix * glm::vec4(defaultNormal, 0.0f);

      float d = glm::dot(normal, positionOnPlane);
      mReflectionPlane = glm::vec4(glm::vec3(normal), d);
   }

   ::Graphics::ViewPortInfo PlanarReflectionComponent::GetRenderTargetViewPortInfo() const {

      return mRenderTargetViewPortInfo;
   }

   std::shared_ptr<PlanarReflectionProxy> PlanarReflectionComponent::CreatePlanarReflectionProxy()
   {
      return std::make_shared<PlanarReflectionProxy>(this);
   }

   void PlanarReflectionComponent::OnPostInitialized()
   {
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         sceneSP->RegisterDeferredResourceCreator(this, GetGameObjectName());
      }
   }

   void PlanarReflectionComponent::PostLevelInit()
   {
      SceneComponent::PostLevelInit();

      static const uint64_t functionId = Hash("PlanarReflectionComponent: PostLevelInit");
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
            {
               sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

                  PlanarReflectionProxy* proxyPtr = static_cast<PlanarReflectionProxy*>(sceneRenderer->PlanarReflectionProxiesMap[mPlanarReflectionSceneProxyId].get());
                  auto resourceTexture = proxyPtr->GetPlanarReflectionTexture();
                  mPlanarReflectionDeferredController->GetDeferredResource(); // Just in case deferred resource wasn't initialized
                  mPlanarReflectionDeferredController->SetResource(resourceTexture);
               });
            }
         }
      }
   }

   void PlanarReflectionComponent::Tick(const float deltaTime)
   {
      if (mIsEnabled)
      {
         if (bTransformationDirty)
         {
            UpdateRelativeMatrix();
            UpdateReflectionPlane();
            SyncDataWithRenderThread();
         }
      }
   }

   ACamera* PlanarReflectionComponent::GetOwnerCamera() const
   {
      return mOwnerCamera;
   }

   size_t PlanarReflectionComponent::GetSceneProxyId() const
   {
      return mPlanarReflectionSceneProxyId;
   }

   glm::vec4 PlanarReflectionComponent::GetReflectionPlane() const
   {
      return mReflectionPlane;
   }

   void PlanarReflectionComponent::SetSceneProxyId(const size_t sceneProxyId)
   {
      mPlanarReflectionSceneProxyId = sceneProxyId;
   }

   ComponentType PlanarReflectionComponent::GetComponentType() const {
      return PLANAR_REFLECTION_COMPONENT;
   }

   std::shared_ptr<IDeferredResourceBase> PlanarReflectionComponent::GetDeferredResource()
   {
      return mPlanarReflectionDeferredController->GetDeferredResource();
   }

   void PlanarReflectionComponent::SyncDataWithRenderThread()
   {
      static const uint64_t functionId = Hash("PlanarReflectionComponent: SyncDataWithRenderThread");
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

               PlanarReflectionProxy* proxyPtr = static_cast<PlanarReflectionProxy*>(sceneRenderer->PlanarReflectionProxiesMap[mPlanarReflectionSceneProxyId].get());
               proxyPtr->SetReflectionPlane(mReflectionPlane);
            });
         }
      }
   }

}