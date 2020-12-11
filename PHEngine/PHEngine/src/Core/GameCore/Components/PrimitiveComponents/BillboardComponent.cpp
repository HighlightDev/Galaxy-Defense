#include "BillboardComponent.h"
#include "Core/GraphicsCore/SceneProxy/BillboardSceneProxy.h"

namespace Game
{

   BillboardComponent::BillboardComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const BillboardRenderData& renderData)
      : PrimitiveComponent(gameObjectName, translation, rotation, scale)
      , m_renderData(renderData)
   {
   }

   BillboardComponent::~BillboardComponent()
   {
   }

   ComponentType BillboardComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void BillboardComponent::Tick(float deltaTime)
   {

   }

   void BillboardComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);
   }

   std::shared_ptr<PrimitiveSceneProxy> BillboardComponent::CreateSceneProxy() const
   {
      return std::make_shared<BillboardSceneProxy>(this);
   }
}
