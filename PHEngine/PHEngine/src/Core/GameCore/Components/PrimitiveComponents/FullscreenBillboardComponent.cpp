#include "FullscreenBillboardComponent.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/FullscreenBillboardSceneProxy.h"

namespace EngineCore {

FullscreenBillboardComponent::FullscreenBillboardComponent(
    const std::shared_ptr<BillboardComponentData>& data, const BillboardRenderData& renderData)
    : BillboardComponent(data, renderData)
{
}

FullscreenBillboardComponent::~FullscreenBillboardComponent()
{
}

std::shared_ptr<PrimitiveSceneProxy> FullscreenBillboardComponent::CreateSceneProxy() const
{
    return std::make_shared<FullscreenBillboardSceneProxy>(this);
}
} // namespace EngineCore
