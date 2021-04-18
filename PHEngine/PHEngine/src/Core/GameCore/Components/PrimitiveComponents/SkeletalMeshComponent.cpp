#include "SkeletalMeshComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/ScriptingCore/LuaWrapper.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"


using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace Game
{

   SkeletalMeshComponent::SkeletalMeshComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const std::string& LuaScriptName, const SkeletalMeshRenderData& renderData)
      : PrimitiveComponent(gameObjectName, translation, rotation, scale, renderData.m_skin->GetBoundingBox())
      , m_renderData(renderData)
      , mLuaScriptAbsPath(EngineUtility::ConvertFromRelativeToAbsolutePath(IO::FolderManager::GetInstance()->GetDirectoryRelativePathByFileName(LuaScriptName)))
      , mLuaInstance(std::make_unique<LuaWrapper>())
      , mUpdateDataResetTimeCounter(0.0f)
      , update_data_reset_time(0.015f)
      , mTimeIncreaseMultiply(1.0f)
      , LuaScriptName(LuaScriptName)
      , SrcAnimationTime(EngineGOProperty<float>(0.0f, "SrcAnimTime"))
      , DstAnimationTime(EngineGOProperty<float>(0.0f, "DstAnimTime"))
      , SrcAnimationName(EngineGOProperty<std::string>("", "SrcAnimName"))
      , DstAnimationName(EngineGOProperty<std::string>("", "DstAnimName"))
      , TransitionValue(EngineGOProperty<float>(0.0f, "AnimTransitionValue"))
      , bTransitionEnabled(EngineGOProperty<bool>(false, "bAnimTransitionEnabled"))
   {
      /* Meta table */
      ENGINE_PROPERTY(&SrcAnimationTime);
      ENGINE_PROPERTY(&DstAnimationTime);
      ENGINE_PROPERTY(&SrcAnimationName);
      ENGINE_PROPERTY(&DstAnimationName);
      ENGINE_PROPERTY(&TransitionValue);
      ENGINE_PROPERTY(&bTransitionEnabled);
      /* Meta table */
   }

   SkeletalMeshComponent::~SkeletalMeshComponent()
   {
   }

   void SkeletalMeshComponent::PostLevelInit() 
   {
      if (mLuaInstance->ExecuteScript(mLuaScriptAbsPath))
      {
         mTimeIncreaseMultiply = LuaGetGlobal<float>::Value(*mLuaInstance.get(), "AnimationTimeMultiply", -1);
      }
   }

   std::shared_ptr<IMaterial> SkeletalMeshComponent::GetMaterial() const
   {
      // get from scene corresponding to material proxy material instance
      std::shared_ptr<IMaterial> materialResult = nullptr;
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
      }
      assert(materialResult != nullptr);
      return materialResult;
   }

   ComponentType SkeletalMeshComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void SkeletalMeshComponent::Tick(const float deltaTime)
   {
      SrcAnimationTime += deltaTime * mTimeIncreaseMultiply;
      mUpdateDataResetTimeCounter += deltaTime;

      const bool bUpdateData = mUpdateDataResetTimeCounter >= update_data_reset_time;

      if (bUpdateData)
      {
         SyncDataWithRenderThread();
      }
   }

   void SkeletalMeshComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);

      auto staticCompData = SerializeHelper::GetSerializedDataSkeletalMesh(this);
      actorData.ComponentsData.emplace_back(staticCompData);
   }

   void SkeletalMeshComponent::SyncDataWithRenderThread()
   {
      mUpdateDataResetTimeCounter = fmod(mUpdateDataResetTimeCounter, update_data_reset_time);

      static constexpr uint64_t functionId = Hash("SkeletalMeshComponent: SetAnimationDeltaTime");
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            SrcAnimationTime.SetValue(fmod(SrcAnimationTime, 100000.0f));
            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

               SkeletalMeshSceneProxy* proxyPtr = static_cast<SkeletalMeshSceneProxy*>(sceneRenderer->SceneProxiesMap[SceneProxyId].get());
               proxyPtr->UpdateAnimationData(bTransitionEnabled, TransitionValue, SrcAnimationTime, DstAnimationTime, SrcAnimationName, DstAnimationName);
            });
         }
      }
   }

   std::shared_ptr<PrimitiveSceneProxy> SkeletalMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<SkeletalMeshSceneProxy>(this);
   }

}