#include "LightComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonApi/StringHash.h"

namespace Game
{

   size_t LightComponent::TotalLightSceneProxyId = 0;

   LightComponent::LightComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
      : SceneComponent(translation, rotation, scale)
   {
   }

   LightComponent::~LightComponent()
   {
   }

   uint64_t LightComponent::GetComponentType() const
   {
      return LIGHT_COMPONENT;
   }

   void LightComponent::UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);
      // Update light proxy transform
      constexpr uint64_t functionId = Hash("LightComponent: OnUpdateLightComponentTransform_GameThread");

      m_scene->OnUpdateLightComponentTransform_GameThread(LightSceneProxyId, GetObjectId(), functionId, m_relativeMatrix);
   }

}