#include "LightComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ComponentData/LightComponentData.h"

namespace EngineCore
{

   LightComponent::LightComponent(const LightComponentData& data)
      : SceneComponent(data.EngineObjectName, data.Translation, data.Rotation, data.Scale)
      , mLightRenderData()
   {
   }

   LightComponent::~LightComponent()
   {
   }

   eComponentType LightComponent::GetComponentType() const
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
         const auto updateSuccessfull = sceneSP->UpdateLightComponentTransform_OnRenderThread(LightSceneProxyId, GetObjectId(), functionId, m_relativeMatrix);
         SetIsTransformationDirty(!updateSuccessfull);
      }
   }

}