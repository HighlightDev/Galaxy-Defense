#include "StaticMeshComponent.h"
#include "Core/GraphicsCore/SceneProxy/StaticMeshSceneProxy.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"

#include <memory>
#include <algorithm>

using namespace Graphics;

namespace EngineCore
{

   StaticMeshComponent::StaticMeshComponent(const std::shared_ptr<MeshComponentData> &meshComponentData, const MeshRenderData renderData)
       : PrimitiveComponent(meshComponentData->EngineObjectName,
                            meshComponentData->m_translation,
                            meshComponentData->m_eulerRotationDegrees,
                            meshComponentData->m_scale),
         m_renderData(renderData)
   {
   }

   StaticMeshComponent::~StaticMeshComponent()
   {
   }

   void StaticMeshComponent::Tick(const float deltaTime)
   {
      Base::Tick(deltaTime);
   }

   void StaticMeshComponent::SetIsEnabled(const bool bEnabled)
   {
      PrimitiveComponent::SetIsEnabled(bEnabled);

      const auto &material = GetMaterial();
      if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
      {
         material->SetIsEnabled(bEnabled);
      }
   }

   void StaticMeshComponent::SetIsVisible(bool isVisible)
   {
      PrimitiveComponent::SetIsVisible(isVisible);

      const auto &material = GetMaterial();
      if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType())
      {
         material->SetIsEnabled(isVisible);
      }
   }

   void StaticMeshComponent::PostLevelInit()
   {
      PrimitiveComponent::PostLevelInit();
   }

   std::shared_ptr<IMaterial> StaticMeshComponent::GetMaterial() const
   {
      std::shared_ptr<IMaterial> materialResult = nullptr;
      if (const auto &sceneSP = m_sceneWP.lock())
      {
         materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
      }
      assert(materialResult != nullptr);
      return materialResult;
   }

   void StaticMeshComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);

      auto staticCompData = SerializeHelper::GetSerializedDataStaticMesh(this);
      actorData.ComponentsData.emplace_back(staticCompData);
   }

   std::shared_ptr<PrimitiveSceneProxy> StaticMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<StaticMeshSceneProxy>(this);
   }

   eComponentType StaticMeshComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

}
