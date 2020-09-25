#include "SkeletalMeshComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/ScriptingCore/LuaWrapper.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace Game
{

   SkeletalMeshComponent::SkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const std::string& mLuaScriptRelPath, const SkeletalMeshRenderData& renderData)
      : PrimitiveComponent(translation, rotation, scale)
      , m_renderData(renderData)
      , mLuaScriptRelPath(mLuaScriptRelPath)
      , mLuaInstance(std::make_unique<LuaWrapper>())
      , mUpdateDataResetTimeCounter(0.0f)
      , update_data_reset_time(0.015f)
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

   void SkeletalMeshComponent::Tick(const float deltaTime)
   {
      if (mLuaInstance->ExecuteScript(mLuaScriptRelPath))
      {
         const float updatedTime = LuaFunction<float(float)>::Call(*mLuaInstance.get(), "UpdateAnimationTime", deltaTime);
         mSrcAnimationTime += updatedTime;
      }

      mUpdateDataResetTimeCounter += deltaTime;
      const bool bUpdateData = mUpdateDataResetTimeCounter >= update_data_reset_time;

      if (bUpdateData)
      {
         SyncDataWithRenderThread();
      }
   }

   void SkeletalMeshComponent::SyncDataWithRenderThread()
   {
      mUpdateDataResetTimeCounter = fmod(mUpdateDataResetTimeCounter, update_data_reset_time);

      static constexpr uint64_t functionId = Hash("SkeletalMeshComponent: SetAnimationDeltaTime");
      if (const auto& sceneRenderer = m_scene->GetThreadManager().TryGetSceneRendererWP().lock())
      {
         mSrcAnimationTime = fmod(mSrcAnimationTime, 100000.0f);
         m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

            SkeletalMeshSceneProxy* proxyPtr = static_cast<SkeletalMeshSceneProxy*>(sceneRenderer->SceneProxies[PrimitiveProxyComponentId].get());
            proxyPtr->UpdateAnimationData(bTransitionEnabled, mTransitionValue, mSrcAnimationTime, mDstAnimationTime, mSrcAnimationName, mDstAnimationName);
         });
      }
   }

   std::shared_ptr<PrimitiveSceneProxy> SkeletalMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<SkeletalMeshSceneProxy>(this);
   }

}