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

   SkeletalMeshComponent::SkeletalMeshComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const std::string& LuaScriptAbsPath, const SkeletalMeshRenderData& renderData)
      : PrimitiveComponent(gameObjectName, translation, rotation, scale)
      , m_renderData(renderData)
      , mLuaScriptAbsPath(EngineUtility::ConvertFromRelativeToAbsolutePath(LuaScriptAbsPath))
      , mLuaInstance(std::make_unique<LuaWrapper>())
      , mUpdateDataResetTimeCounter(0.0f)
      , update_data_reset_time(0.015f)
      , SrcAnimationTime(GenericObjectProperty<float>(0.0f, "SrcAnimTime"))
      , DstAnimationTime(GenericObjectProperty<float>(0.0f, "DstAnimTime"))
      , SrcAnimationName(GenericObjectProperty<std::string>("", "SrcAnimName"))
      , DstAnimationName(GenericObjectProperty<std::string>("", "DstAnimName"))
      , TransitionValue(GenericObjectProperty<float>(0.0f, "AnimTransitionValue"))
      , bTransitionEnabled(GenericObjectProperty<bool>(false, "bAnimTransitionEnabled"))
   {
      /* Meta table */
      mEngineProperties["SrcAnimTime"] = &SrcAnimationTime;
      mEngineProperties["DstAnimTime"] = &DstAnimationTime;
      mEngineProperties["SrcAnimName"] = &SrcAnimationName;
      mEngineProperties["DstAnimName"] = &DstAnimationName;
      mEngineProperties["AnimTransitionValue"] = &TransitionValue;
      mEngineProperties["bAnimTransitionEnabled"] = &bTransitionEnabled;
      /* Meta table */
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
      if (mLuaInstance->ExecuteScript(mLuaScriptAbsPath))
      {
         const float updatedTime = LuaFunction<float(float)>::Call(*mLuaInstance.get(), "UpdateAnimationTime", deltaTime);
         SrcAnimationTime += updatedTime;
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
         SrcAnimationTime = fmod(SrcAnimationTime, 100000.0f);
         m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

            SkeletalMeshSceneProxy* proxyPtr = static_cast<SkeletalMeshSceneProxy*>(sceneRenderer->SceneProxies[PrimitiveProxyComponentId].get());
            proxyPtr->UpdateAnimationData(bTransitionEnabled, TransitionValue, SrcAnimationTime, DstAnimationTime, SrcAnimationName, DstAnimationName);
         });
      }
   }

   std::shared_ptr<PrimitiveSceneProxy> SkeletalMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<SkeletalMeshSceneProxy>(this);
   }

}