#include "SkeletalMeshComponent.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"

using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace Game
{

   SkeletalMeshComponent::SkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const SkeletalMeshRenderData& renderData)
      : PrimitiveComponent(translation, rotation, scale)
      , m_renderData(renderData)
      , mSrcAnimationTime(0.0f)
      , mDstAnimationTime(0.0f)
      , mSrcAnimationName("")
      , mDstAnimationName("")
      , mTransitionValue(0.0f)
      , bTransitionEnabled(false)
   {

   }

   SkeletalMeshComponent::~SkeletalMeshComponent()
   {
   }

   uint64_t SkeletalMeshComponent::GetComponentType() const
   {
      return SKELETAL_MESH_COMPONENT;
   }

   void SkeletalMeshComponent::Tick(float deltaTime)
   {
      const float animDeltaTime = deltaTime * 1000.0f;

      mSrcAnimationTime += animDeltaTime;

      if (m_tickCounter == 1) // TODO: hot fix for optimization, later should be done much better way
      {
         constexpr uint64_t functionId = Hash("SkeletalMeshComponent: SetAnimationDeltaTime");
         mSrcAnimationTime = fmod(mSrcAnimationTime, 1000000.0f);

         if (const auto& sceneRenderer = m_scene->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

               SkeletalMeshSceneProxy* proxyPtr = static_cast<SkeletalMeshSceneProxy*>(sceneRenderer->SceneProxies[PrimitiveProxyComponentId].get());
               proxyPtr->UpdateAnimationData(bTransitionEnabled, mTransitionValue, mSrcAnimationTime, mDstAnimationTime, 0, 0);
            });
         }

         m_tickCounter = -1;
      }
      
      m_tickCounter++;
   }

   std::shared_ptr<PrimitiveSceneProxy> SkeletalMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<SkeletalMeshSceneProxy>(this);
   }

}