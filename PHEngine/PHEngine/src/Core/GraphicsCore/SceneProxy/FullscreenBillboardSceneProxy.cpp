#include "FullscreenBillboardSceneProxy.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

#include <glm/mat4x4.hpp>

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics {
namespace Proxy {
FullscreenBillboardSceneProxy::FullscreenBillboardSceneProxy(const FullscreenBillboardComponent* component)
    : BillboardSceneProxy(component)
{
}

FullscreenBillboardSceneProxy::~FullscreenBillboardSceneProxy()
{
}

void FullscreenBillboardSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    BillboardSceneProxy::Render(cameraSceneProxy, glm::mat4(1), glm::mat4(1));
}

} // namespace Proxy
} // namespace Graphics
