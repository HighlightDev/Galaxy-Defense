#pragma once

#include "Core/GameCore/ShaderImplementation/UiRectangleShader.h"
#include "UiSceneProxyBase.h"

#include <glm/vec4.hpp>

namespace EngineCore {
namespace GUI {
class UiToggleButton;
}
} // namespace EngineCore

using namespace EngineCore::ShaderImpl;

namespace Graphics {
namespace Proxy {
class UiToggleButtonSceneProxy : public UiSceneProxyBase {
    std::shared_ptr<UiRectangleShader> mUiRectangleShader;

    glm::vec3 mToggleOffColor;

    glm::vec3 mToggleOnColor;

    float mOpacity;

    bool mIsStateOn;

public:
    UiToggleButtonSceneProxy(const ::EngineCore::GUI::UiToggleButton* uiRectangle);

    ~UiToggleButtonSceneProxy() override;

    void Render() override;

    void SetToggleOnColor(const glm::vec3& color);

    void SetToggleOffColor(const glm::vec3& color);

    void SetOpacity(const float opacity);

    void SetState(const bool state);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;
};
} // namespace Proxy
} // namespace Graphics