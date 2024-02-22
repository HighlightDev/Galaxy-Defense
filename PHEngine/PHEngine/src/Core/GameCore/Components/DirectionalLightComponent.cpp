#include "DirectionalLightComponent.h"

#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"

#include <glm/gtx/quaternion.hpp>

using namespace Graphics;

namespace EngineCore
{

   DirectionalLightComponent::DirectionalLightComponent(const std::shared_ptr<LightComponentData> &lightComponentData)
       : LightComponent(lightComponentData)
   {
      const auto &d_directionalLight = std::static_pointer_cast<DirectionalLightComponentData>(lightComponentData);
      assert(nullptr == mLightRenderData);
      mLightRenderData = std::make_shared<DirectionalLightRenderData>(d_directionalLight->Direction, d_directionalLight->Ambient,
                                                                      d_directionalLight->Diffuse, d_directionalLight->Specular,
                                                                      d_directionalLight->ShadowInfo);
   }

   DirectionalLightComponent::~DirectionalLightComponent()
   {
      PlayerMovedGameThreadEvent::GetInstance()->RemoveListener(PlayerMovedGameThreadEvent::GetInstanceId());
      PhysicsComponentUpdatedGameThreadEvent::GetInstance()->RemoveListener(PhysicsComponentUpdatedGameThreadEvent::GetInstanceId());
   }

   void DirectionalLightComponent::Initialize()
   {
      LightComponent::Initialize();

      if (mLightRenderData->ShadowInfo)
      {
         PlayerMovedGameThreadEvent::GetInstance()->AddListener(std::dynamic_pointer_cast<DirectionalLightComponent>(shared_from_this()));
         PhysicsComponentUpdatedGameThreadEvent::GetInstance()->AddListener(std::dynamic_pointer_cast<DirectionalLightComponent>(shared_from_this()));
      }
   }

   std::shared_ptr<DirectionalLightRenderData> DirectionalLightComponent::GetRenderData() const
   {
      return std::static_pointer_cast<DirectionalLightRenderData>(mLightRenderData);
   }

   std::shared_ptr<LightSceneProxy> DirectionalLightComponent::CreateSceneProxy() const
   {
      return std::make_shared<DirectionalLightSceneProxy>(this);
   }

   void DirectionalLightComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {

      auto &actorData = GetSerializeDataActor(dataContainer);

      auto lightCompData = std::make_shared<SerializeDataDirLightComponent>();
      const auto &renderData = GetRenderData();

      lightCompData->ComponentName = EngineObjectName;
      lightCompData->AmbientLight = renderData->Ambient;
      lightCompData->DiffuseLight = renderData->Diffuse;
      lightCompData->SpecularLight = renderData->Specular;
      lightCompData->Direction = renderData->Direction;
      lightCompData->Rotation = GetRotationDegrees();

      const bool bHasShadowMap = renderData->ShadowInfo != nullptr;

      if (bHasShadowMap)
      {
         lightCompData->ShadowMapSize = (float)renderData->ShadowInfo->GetAtlasResource()->GetTextureRezolution().x;
      }
      else
      {
         lightCompData->ShadowMapSize = 0.0f;
      }

      lightCompData->bHasShadowMap = bHasShadowMap;

      actorData.ComponentsData.emplace_back(lightCompData);
   }

   void DirectionalLightComponent::UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);
   }

   void DirectionalLightComponent::Tick(float deltaTime)
   {
      Base::Tick(deltaTime);

      if (bIsRenderDataDirty && bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
      {
         SyncRenderData();
         bIsRenderDataDirty = false;
      }
   }

   eComponentType DirectionalLightComponent::GetComponentType() const
   {
      return eComponentType::LIGHT_COMPONENT;
   }

   void DirectionalLightComponent::ProcessEvent(const PlayerMovedGameThreadEvent::EventData_t &data)
   {
      if (const auto playerTranslationOffset = std::get<0>(data).lock())
      {
         if (!EngineMath::CheckSimilarityVec3(playerTranslationOffset->Translation, mPlayerTranslationOffset))
         {
            mPlayerTranslationOffset = playerTranslationOffset->Translation;
            bIsRenderDataDirty = true;
            if (bIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
               SyncRenderData();
               bIsRenderDataDirty = false;
            }
         }
      }
   }

   void DirectionalLightComponent::ProcessEvent(const PhysicsComponentUpdatedGameThreadEvent::EventData_t &data)
   {
      bIsRenderDataDirty = true;
   }

   void DirectionalLightComponent::SyncRenderData()
   {
      if (const auto &sceneSp = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
         {
            static const uint64_t functionId = Hash("DirectionalLightComponent::SetPlayerPositionOffset");
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetObjectId(), functionId, [sceneRenderer, lightSceneProxyId = mLightSceneProxyId, playerTranslationOffset = mPlayerTranslationOffset]()
                                                                                {
               const auto& lightProxySp = sceneRenderer->GetLightProxyByProxyId(lightSceneProxyId);
               assert(lightProxySp);                                       
               const auto shadowInfo = lightProxySp->GetShadowInfo();
               if (shadowInfo)
               {
                  shadowInfo->SetPlayerPositionOffset(playerTranslationOffset);
                  lightProxySp->SetIsTransformationDirty(true);
                  shadowInfo->SetIsShadowMapDirty(true);
               } });
         }
      }
   }

}
