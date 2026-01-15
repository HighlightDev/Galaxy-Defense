#include "CubemapComponent.h"

#include "Core/GameCore/Components/ComponentData/CubemapComponentData.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/CubemapComponentLuaProxy.h"
#include "Core/GraphicsCore/SceneProxy/CubemapSceneProxy.h"

using namespace EngineCore::Scripts;

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

std::shared_ptr<PrimitiveSceneProxy> CubemapComponent::CreateSceneProxy() const
{
    return std::make_shared<CubemapSceneProxy>(this);
}

std::shared_ptr<Scripts::LuaProxy> CubemapComponent::ReplicateLuaProxy()
{
    return std::make_shared<CubemapComponentLuaProxy>(std::static_pointer_cast<CubemapComponent>(shared_from_this()));
}

} // namespace EngineCore