#include "CubemapComponent.h"
#include "Core/GraphicsCore/SceneProxy/CubemapSceneProxy.h"

namespace Game
{

   CubemapComponent::CubemapComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const CubemapRenderData& renderData)
      : PrimitiveComponent(gameObjectName, translation, rotation, scale)
      , m_renderData(renderData)
   {
   }

   CubemapComponent::~CubemapComponent()
   {
   }

   ComponentType CubemapComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void CubemapComponent::Tick(const float deltaTime)
   {

   }

   void CubemapComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);
   }

   std::shared_ptr<PrimitiveSceneProxy> CubemapComponent::CreateSceneProxy() const
   {
      return std::make_shared<CubemapSceneProxy>(this);
   }

}