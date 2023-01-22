#include "SpotlightComponent.h"
#include "Core/GraphicsCore/SceneProxy/SpotlightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Components/ComponentData/SpotlightComponentData.h"

namespace EngineCore
{

   SpotlightComponent::SpotlightComponent(const LightComponentData &data)
       : PointLightComponent(data)
   {
      mTransform->Rotator = glm::quat(glm::vec3(DEG_TO_RAD(data.Rotation.x), DEG_TO_RAD(data.Rotation.y), DEG_TO_RAD(data.Rotation.z)));

      const auto &spotlightComponentData = static_cast<const SpotlightComponentData &>(data);
      assert(nullptr == mLightRenderData);
      mLightRenderData = std::make_shared<SpotlightRenderData>(spotlightComponentData.Attenuation, spotlightComponentData.RadianceRadius,
                                                           spotlightComponentData.Cutoff, spotlightComponentData.Ambient, spotlightComponentData.Diffuse,
                                                           spotlightComponentData.Specular, spotlightComponentData.ShadowInfo);
   }

   SpotlightComponent::~SpotlightComponent()
   {
   }

   std::shared_ptr<SpotlightRenderData> SpotlightComponent::GetRenderData() const
   {
      return std::static_pointer_cast<SpotlightRenderData>(mLightRenderData);
   }

   std::shared_ptr<LightSceneProxy> SpotlightComponent::CreateSceneProxy() const
   {
      return std::make_shared<SpotlightSceneProxy>(this);
   }

   void SpotlightComponent::Tick(const float deltaTime)
   {
      Base::Tick(deltaTime);
   }

   void SpotlightComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);

      auto lightCompData = std::make_shared<SerializeDataSpotlightComponent>();
      const auto& renderData = GetRenderData();

      lightCompData->ComponentName = EngineObjectName;
      lightCompData->AmbientLight = renderData->Ambient;
      lightCompData->DiffuseLight = renderData->Diffuse;
      lightCompData->SpecularLight = renderData->Specular;
      lightCompData->Translation = GetTranslation();
      lightCompData->Rotation = GetRotationDegrees();
      lightCompData->Attenuation = renderData->Attenuation;
      lightCompData->RadianceRadius = renderData->RadianceRadius;
      lightCompData->Cutoff = renderData->Cutoff;

      const bool bHasShadowMap = renderData->ShadowInfo != nullptr;

      if (bHasShadowMap)
      {
         lightCompData->ShadowMapSize = static_cast<float>(renderData->ShadowInfo->GetAtlasResource()->GetTextureRezolution().x);
      }
      else
      {
         lightCompData->ShadowMapSize = 0.0f;
      }

      lightCompData->bHasShadowMap = bHasShadowMap;

      actorData.ComponentsData.emplace_back(lightCompData);
   }

}