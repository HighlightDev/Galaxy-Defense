#include "PointLightComponent.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/GameCore/Actor.h"

namespace Game
{

   PointLightComponent::PointLightComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, const PointLightRenderData& renderData)
      : LightComponent(gameObjectName, translation, rotation, glm::vec3(1))
      , m_renderData(renderData)
   {

   }

   PointLightComponent::~PointLightComponent()
   {

   }

   std::shared_ptr<LightSceneProxy> PointLightComponent::CreateSceneProxy() const
   {
      return std::make_shared<PointLightSceneProxy>(this);
   }

   ComponentType PointLightComponent::GetComponentType() const
   {
      return LIGHT_COMPONENT;
   }

   float time = 0.0f;

   void PointLightComponent::Tick(const float deltaTime)
   {
      Base::Tick(deltaTime);
    
   }

}