#include "SkyboxComponent.h"

#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneProxy/SkyboxSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

namespace EngineCore {
using namespace EngineUtility;

SkyboxComponent::SkyboxComponent(const std::shared_ptr<SkyboxComponentData>& data, const SkyboxRenderData& renderData)
    : PrimitiveComponent(data->EngineObjectName, glm::vec3(), glm::vec3(), data->m_scale)
    , m_rotateSpeed(2.0f)
    , m_renderData(renderData)
{
    // skybox must be first rendered primitive
    mSortOrderValue = std::numeric_limits<int32_t>::min();
}

SkyboxComponent::~SkyboxComponent()
{
}

void SkyboxComponent::SetIsEnabled(const bool bEnabled)
{
    PrimitiveComponent::SetIsEnabled(bEnabled);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(bEnabled);
    }
}

void SkyboxComponent::SetIsVisible(bool isVisible)
{
    PrimitiveComponent::SetIsVisible(isVisible);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(isVisible);
    }
}

void SkyboxComponent::Tick(const float deltaTimeSec)
{
    Base::Tick(deltaTimeSec);

    SetRotator(mTransform->Rotator * glm::angleAxis(DEG_TO_RAD(deltaTimeSec * m_rotateSpeed), AXIS_UP));
}

std::shared_ptr<PrimitiveSceneProxy> SkyboxComponent::CreateSceneProxy() const
{
    return std::make_shared<SkyboxSceneProxy>(this);
}

eComponentType SkyboxComponent::GetComponentType() const
{
    return PRIMITIVE_COMPONENT;
}

void SkyboxComponent::SetRotateSpeed(float rotateSpeed)
{
    m_rotateSpeed = rotateSpeed;
}

float SkyboxComponent::GetRotateSpeed() const
{
    return m_rotateSpeed;
}

const SkyboxRenderData& SkyboxComponent::GetRenderData() const
{

    return m_renderData;
}

std::shared_ptr<IMaterial> SkyboxComponent::GetMaterial() const
{
    std::shared_ptr<IMaterial> materialResult = nullptr;
    if (const auto& sceneSP = m_sceneWP.lock()) {
        materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
    }
    ext_assert(materialResult != nullptr, "SkyboxComponent::GetMaterial: materialResult is null");
    return materialResult;
}
} // namespace EngineCore