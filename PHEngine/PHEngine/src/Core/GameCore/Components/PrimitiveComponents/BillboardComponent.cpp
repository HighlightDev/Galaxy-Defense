#include "BillboardComponent.h"
#include "Core/GraphicsCore/SceneProxy/BillboardSceneProxy.h"

namespace Game
{

   BillboardComponent::BillboardComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const BillboardRenderData& renderData)
      : PrimitiveComponent(translation, rotation, scale)
      , m_renderData(renderData)
   {
   }

   BillboardComponent::~BillboardComponent()
   {
   }

   uint64_t BillboardComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void BillboardComponent::Tick(float deltaTime)
   {

   }

   std::shared_ptr<PrimitiveSceneProxy> BillboardComponent::CreateSceneProxy() const
   {
      return std::make_shared<BillboardSceneProxy>(this);
   }
}
