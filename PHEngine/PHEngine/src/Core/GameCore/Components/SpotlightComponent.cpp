#include "SpotlightComponent.h"

#include "Core/GameCore/Components/ComponentData/SpotlightComponentData.h"
#include "Core/GraphicsCore/SceneProxy/SpotlightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {

SpotlightComponent::SpotlightComponent(const std::shared_ptr<LightComponentData>& data)
    : PointLightComponent(data)
{
    mTransform->Rotator
        = glm::quat(glm::vec3(DEG_TO_RAD(data->Rotation.x), DEG_TO_RAD(data->Rotation.y), DEG_TO_RAD(data->Rotation.z)));

    const auto& spotlightComponentData = std::static_pointer_cast<SpotlightComponentData>(data);
    mLightRenderData = std::make_shared<SpotlightRenderData>(
        spotlightComponentData->Attenuation,
        spotlightComponentData->RadianceRadius,
        spotlightComponentData->Cutoff,
        spotlightComponentData->Ambient,
        spotlightComponentData->Diffuse,
        spotlightComponentData->Specular,
        spotlightComponentData->ShadowInfo);
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

void SpotlightComponent::Tick(const float deltaTimeSec)
{
    Base::Tick(deltaTimeSec);
}

} // namespace EngineCore