#include "CubemapComponent.h"
#include "Core/GraphicsCore/SceneProxy/CubemapSceneProxy.h"
#include "Core/GameCore/Components/ComponentData/CubemapComponentData.h"

namespace EngineCore
{

   CubemapComponent::CubemapComponent(const CubemapComponentData &data, const CubemapRenderData &renderData)
       : PrimitiveComponent(data.EngineObjectName, data.m_translation, data.m_eulerRotationDegrees, data.m_scale, BoundingBox3D()),
         m_renderData(renderData)
   {
   }

   CubemapComponent::~CubemapComponent()
   {
   }

   eComponentType CubemapComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void CubemapComponent::Tick(const float deltaTime)
   {
   }

   void CubemapComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);
   }

   std::shared_ptr<PrimitiveSceneProxy> CubemapComponent::CreateSceneProxy() const
   {
      return std::make_shared<CubemapSceneProxy>(this);
   }

}