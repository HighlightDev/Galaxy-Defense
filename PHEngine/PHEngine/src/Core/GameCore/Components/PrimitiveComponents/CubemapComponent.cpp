#include "CubemapComponent.h"

#include "Core/GameCore/Components/ComponentData/CubemapComponentData.h"
#include "Core/GraphicsCore/SceneProxy/CubemapSceneProxy.h"

namespace EngineCore {

CubemapComponent::CubemapComponent(const std::shared_ptr<CubemapComponentData>& data, const CubemapRenderData& renderData)
    : PrimitiveComponent(data->EngineObjectName, data->m_translation, data->m_eulerRotationDegrees, data->m_scale)
    , m_renderData(renderData)
{
}

CubemapComponent::~CubemapComponent()
{
}

eComponentType CubemapComponent::GetComponentType() const
{
    return PRIMITIVE_COMPONENT;
}

void CubemapComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
{
    auto& actorData = GetSerializeDataActor(dataContainer);
}

std::shared_ptr<PrimitiveSceneProxy> CubemapComponent::CreateSceneProxy() const
{
    return std::make_shared<CubemapSceneProxy>(this);
}

} // namespace EngineCore