#pragma once

#include "Core/GameCore/GUI/Common/DividerOrientation.h"
#include "Core/GameCore/ShaderImplementation/UiDividerShader.h"
#include "UiSceneProxyBase.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiDivider;
}
} // namespace EngineCore

using namespace EngineCore::ShaderImpl;

namespace Graphics {
namespace Proxy {
class UiDividerSceneProxy : public UiSceneProxyBase {
protected:
    std::shared_ptr<UiDividerShader> mUiDividerShader;

    glm::vec3 mColor;

    float mOpacity;

    float mLineWidthPx;

    float mEdgeFade;

    ::EngineCore::GUI::eDividerOrientation mOrientation;

public:
    UiDividerSceneProxy(const ::EngineCore::GUI::UiDivider* uiDivider);

    ~UiDividerSceneProxy() override;

    void Render(
        const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
        const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy) override;

    void SetColor(const glm::vec3& color);

    void SetOpacity(const float opacity);

    void SetLineWidthPx(const float widthPx);

    void SetEdgeFade(const float edgeFade);

    void SetDividerOrientation(const ::EngineCore::GUI::eDividerOrientation orientation);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;
};
} // namespace Proxy
} // namespace Graphics