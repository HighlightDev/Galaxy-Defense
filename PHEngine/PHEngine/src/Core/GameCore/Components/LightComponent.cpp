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

   void LightComponent::UpdateRelativeMatrix(const glm::mat4& parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);
      // Update light proxy transform
      static const uint64_t functionId = Hash("LightComponent: UpdateLightComponentTransform_GameThread");

      if (const auto& sceneSP = m_sceneWP.lock())
      {
         sceneSP->UpdateLightComponentTransform_OnRenderThread(LightSceneProxyId, GetObjectId(), functionId, m_relativeMatrix);
      }
   }

}