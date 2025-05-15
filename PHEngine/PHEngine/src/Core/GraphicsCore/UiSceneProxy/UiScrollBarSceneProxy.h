#pragma once

#include "Core/GameCore/ShaderImplementation/UiRectangleShader.h"
#include "UiSceneProxyBase.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiScrollBar;
}
} // namespace EngineCore

using namespace EngineCore::ShaderImpl;

namespace Graphics {
namespace Proxy {
class UiScrollBarSceneProxy : public UiSceneProxyBase {
    std::shared_ptr<UiRectangleShader> mUiRectangleShader;

    float mMaxScrollValue{0.0f};
    float mMinScrollValue{0.0f};
    float mScrollValue{0.0f};
    float mScrollStep{0.0f};

public:
    UiScrollBarSceneProxy(const ::EngineCore::GUI::UiScrollBar* uiScrollBar);

    ~UiScrollBarSceneProxy() override;

    void Render() override;

    void SetMaxScrollValue(const float maxScrollValue);

    void SetMinScrollValue(const float minScrollValue);

    void SetScrollValue(const float scrollValue);

    void SetScrollStep(const float scrollStep);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;
};
} // namespace Proxy
} // namespace Graphics