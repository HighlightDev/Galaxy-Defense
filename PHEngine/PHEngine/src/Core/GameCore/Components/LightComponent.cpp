#include "LightComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"

namespace Game
{

   LightComponent::LightComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
      : SceneComponent(gameObjectName, translation, rotation, scale)
   {
   }

   LightComponent::~LightComponent()
   {
   }

   ComponentType LightComponent::GetComponentType() const
   {
      return LIGHT_COMPONENT;
   }

   void LightComponent::UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);
      // Update light proxy transform
      constexpr uint64_t functionId = Hash("LightComponent: UpdateLightComponentTransform_GameThread");

      m_scene->UpdateLightComponentTransform_GameThread(LightSceneProxyId, GetObjectId(), functionId, m_relativeMatrix);
   }

}