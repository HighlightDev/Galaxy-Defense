#include "PointLightComponent.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/GameCore/Actor.h"

namespace Game
{

   PointLightComponent::PointLightComponent(const std::string& gameObjectName, glm::vec3 translation, const PointLightRenderData& renderData)
      : LightComponent(gameObjectName, translation, glm::vec3(0), glm::vec3(1))
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

   void PointLightComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto it = std::find_if(dataContainer.Actors.begin(), dataContainer.Actors.end(), [=](const SerializeDataActor& actorData) { return actorData.ActorName == GetOwner()->GetName(); });

      assert(it != dataContainer.Actors.end());

      auto lightCompData = std::make_shared<SerializeDataPointLightComponent>();

      lightCompData->ComponentName = GameObjectName;
      lightCompData->AmbientLight = m_renderData.Ambient;
      lightCompData->DiffuseLight = m_renderData.Diffuse;
      lightCompData->SpecularLight = m_renderData.Specular;
      lightCompData->Translation = GetTranslation();
      lightCompData->Rotation = GetRotationEuler();
      lightCompData->Attenuation = m_renderData.Attenuation;
      lightCompData->RadianceSqrRadius = m_renderData.RadianceSqrRadius;

      const bool bHasShadowMap = !!m_renderData.ShadowInfo;

      if (bHasShadowMap)
      {
         lightCompData->ShadowMapSize = static_cast<float>(m_renderData.ShadowInfo->GetAtlasResource()->GetTextureRezolution().x);
      }
      else
      {
         lightCompData->ShadowMapSize = 0.0f;
      }

      lightCompData->bHasShadowMap = bHasShadowMap;

      it->ComponentsData.emplace_back(lightCompData);
   }

}