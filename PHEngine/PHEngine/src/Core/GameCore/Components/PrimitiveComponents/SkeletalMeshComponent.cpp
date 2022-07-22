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
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"

using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace EngineCore
{

   SkeletalMeshComponent::SkeletalMeshComponent(const MeshComponentData &meshComponentData, const SkeletalMeshRenderData &renderData)
       : PrimitiveComponent(meshComponentData.GameObjectName,
                            meshComponentData.m_translation,
                            meshComponentData.m_eulerRotationDegrees,
                            meshComponentData.m_scale,
                            renderData.m_skin->GetBoundingBox()),
         m_renderData(renderData),
         mLuaScriptAbsPath(IO::FolderManager::GetInstance()->GetScriptPath() +
                           meshComponentData.m_luaScriptPath),
         mLuaInstance(std::make_unique<LuaWrapper>()),
         mUpdateDataResetTimeCounter(0.0f),
         update_data_reset_time(0.1f),
         mTimeIncreaseMultiply(1.0f),
         LuaScriptName(meshComponentData.m_luaScriptPath),
         SrcAnimationTime(std::make_shared<EngineGOProperty<float>>(0.0f, "SrcAnimTime")),
         DstAnimationTime(std::make_shared<EngineGOProperty<float>>(0.0f, "DstAnimTime")),
         SrcAnimationName(std::make_shared<EngineGOProperty<std::string>>("", "SrcAnimName")),
         DstAnimationName(std::make_shared<EngineGOProperty<std::string>>("", "DstAnimName")),
         TransitionValue(std::make_shared<EngineGOProperty<float>>(0.0f, "AnimTransitionValue")),
         bTransitionEnabled(std::make_shared<EngineGOProperty<bool>>(false, "bAnimTransitionEnabled"))
   {
      /* Meta table */
      AddEngineProperty(SrcAnimationTime);
      AddEngineProperty(DstAnimationTime);
      AddEngineProperty(SrcAnimationName);
      AddEngineProperty(DstAnimationName);
      AddEngineProperty(TransitionValue);
      AddEngineProperty(bTransitionEnabled);
      /* Meta table */
   }

   SkeletalMeshComponent::~SkeletalMeshComponent()
   {
   }

   void SkeletalMeshComponent::PostLevelInit()
   {
      PrimitiveComponent::PostLevelInit();

      if (mLuaInstance->ExecuteScript(mLuaScriptAbsPath))
      {
         mTimeIncreaseMultiply = LuaGetGlobal<float>::Value(*mLuaInstance.get(), "AnimationTimeMultiply", -1);
      }
   }

   void SkeletalMeshComponent::SetIsEnabled(const bool bEnabled)
   {
      PrimitiveComponent::SetIsEnabled(bEnabled);

      const auto &material = GetMaterial();
      if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
      {
         material->SetIsEnabled(bEnabled);
      }
   }

   void SkeletalMeshComponent::SetIsVisible(bool isVisible)
   {
      PrimitiveComponent::SetIsVisible(isVisible);

      const auto &material = GetMaterial();
      if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
      {
         material->SetIsEnabled(isVisible);
      }
   }

   std::shared_ptr<IMaterial> SkeletalMeshComponent::GetMaterial() const
   {
      // get from scene corresponding to material proxy material instance
      std::shared_ptr<IMaterial> materialResult = nullptr;
      if (const auto &sceneSP = m_sceneWP.lock())
      {
         materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
      }
      assert(materialResult != nullptr);
      return materialResult;
   }

   eComponentType SkeletalMeshComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void SkeletalMeshComponent::Tick(const float deltaTime)
   {
      SrcAnimationTime->SetValue(SrcAnimationTime->GetValue() + (deltaTime * mTimeIncreaseMultiply));
      mUpdateDataResetTimeCounter += deltaTime;

      const bool bUpdateData = mUpdateDataResetTimeCounter >= update_data_reset_time;

      if (bUpdateData)
      {
         SyncDataWithRenderThread();
      }
   }

   void SkeletalMeshComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);

      auto staticCompData = SerializeHelper::GetSerializedDataSkeletalMesh(this);
      actorData.ComponentsData.emplace_back(staticCompData);
   }

   void SkeletalMeshComponent::SyncDataWithRenderThread()
   {
      mUpdateDataResetTimeCounter = fmod(mUpdateDataResetTimeCounter, update_data_reset_time);

      static const uint64_t functionId = Hash("SkeletalMeshComponent: SetAnimationDeltaTime");
      if (const auto &sceneSP = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            SrcAnimationTime->SetValue(fmod(SrcAnimationTime->GetValue(), 100000.0f));
            sceneSP->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]()
                                           { 
                              const auto& primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(SceneProxyId);
                              assert(primitiveProxySp);
                              SkeletalMeshSceneProxy *proxyPtr 
                              = static_cast<SkeletalMeshSceneProxy *>(primitiveProxySp.get()); 
                                    proxyPtr->UpdateAnimationData(bTransitionEnabled->GetValue(),
                                    TransitionValue->GetValue(),
                                    SrcAnimationTime->GetValue(),
                                    DstAnimationTime->GetValue(),
                                    SrcAnimationName->GetValue(),
                                    DstAnimationName->GetValue()); });
         }
      }
   }

   std::shared_ptr<PrimitiveSceneProxy> SkeletalMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<SkeletalMeshSceneProxy>(this);
   }

}