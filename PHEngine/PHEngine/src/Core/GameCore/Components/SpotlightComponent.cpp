#include "SpotlightComponent.h"
#include "Core/GraphicsCore/SceneProxy/SpotlightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

namespace Game
{

   SpotlightComponent::SpotlightComponent(const std::string& gameObjectName, const glm::vec3& translation, const glm::vec3& rotation, const SpotlightRenderData& renderData)
      : PointLightComponent(gameObjectName, translation, renderData)
      , m_renderData(renderData)
   {
      mTransform->Rotator = glm::quat(glm::vec3(DEG_TO_RAD(rotation.x), DEG_TO_RAD(rotation.y), DEG_TO_RAD(rotation.z)));
   }

   SpotlightComponent::~SpotlightComponent()
   {
   }

   std::shared_ptr<LightSceneProxy> SpotlightComponent::CreateSceneProxy() const
   {
      return std::make_shared<SpotlightSceneProxy>(this);
   }

   void SpotlightComponent::Tick(const float deltaTime)
   {
      Base::Tick(deltaTime);
   }

   void SpotlightComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);

      auto lightCompData = std::make_shared<SerializeDataSpotlightComponent>();

      lightCompData->ComponentName = GameObjectName;
      lightCompData->AmbientLight = m_renderData.Ambient;
      lightCompData->DiffuseLight = m_renderData.Diffuse;
      lightCompData->SpecularLight = m_renderData.Specular;
      lightCompData->Translation = GetTranslation();
      lightCompData->Rotation = GetRotationEuler();
      lightCompData->Attenuation = m_renderData.Attenuation;
      lightCompData->RadianceSqrRadius = m_renderData.RadianceSqrRadius;
      lightCompData->Cutoff = m_renderData.Cutoff;

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

      actorData.ComponentsData.emplace_back(lightCompData);
   }

}