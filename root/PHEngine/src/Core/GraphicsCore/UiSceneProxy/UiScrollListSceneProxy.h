#pragma once

#include "Core/GameCore/ShaderImplementation/UiRectangleShader.h"
#include "UiSceneProxyBase.h"

#include <glm/vec3.hpp>

#include <cstdint>

namespace EngineCore {
namespace GUI {
class UiScrollList;
}
} // namespace EngineCore

using namespace EngineCore::ShaderImpl;

namespace Graphics {
namespace Proxy {
class UiScrollListSceneProxy : public UiSceneProxyBase {
    std::shared_ptr<UiRectangleShader> mUiRectangleShader;

    int32_t mScrollOffset{0};
    int32_t mMaxScrollOffset{0};
    uint8_t mScrollbarSide{0};
    glm::vec3 mScrollbarBackgroundColor{0.2f, 0.2f, 0.2f};
    glm::vec3 mScrollbarThumbColor{0.6f, 0.6f, 0.6f};
    uint32_t mScrollbarThicknessPixels{8};

public:
    explicit UiScrollListSceneProxy(const ::EngineCore::GUI::UiScrollList* uiScrollList);

    ~UiScrollListSceneProxy() override = default;

    void Render(
        const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
        const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy) override;

    void OnSceneProxyRegistered() override;

    void CleanUp() override;

    void SetScrollOffset(const int32_t value);

    void SetMaxScrollOffset(const int32_t value);

    void SetScrollbarSide(const uint8_t value);

    void SetScrollbarBackgroundColor(const glm::vec3& value);

    void SetScrollbarThumbColor(const glm::vec3& value);

    void SetScrollbarThicknessPixels(const uint32_t value);
};
} // namespace Proxy
} // namespace Graphics
