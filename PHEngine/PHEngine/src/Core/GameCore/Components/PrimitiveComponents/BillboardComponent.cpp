#include "BillboardComponent.h"
#include "Core/GraphicsCore/SceneProxy/BillboardSceneProxy.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"

namespace EngineCore
{

   BillboardComponent::BillboardComponent(const BillboardComponentData &data, const BillboardRenderData &renderData)
       : PrimitiveComponent(data.GameObjectName,
                            data.m_translation,
                            data.m_eulerRotationDegrees,
                            data.m_scale,
                            renderData.m_skin->GetBoundingBox()),
         m_renderData(renderData)
   {
   }

   BillboardComponent::~BillboardComponent()
   {
   }

   eComponentType BillboardComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void BillboardComponent::Tick(float deltaTime)
   {
   }

   void BillboardComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);
   }

   std::shared_ptr<PrimitiveSceneProxy> BillboardComponent::CreateSceneProxy() const
   {
      return std::make_shared<BillboardSceneProxy>(this);
   }
}
