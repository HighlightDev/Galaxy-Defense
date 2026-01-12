#pragma once

#include "Core/GameCore/ShaderImplementation/UiRectangleShader.h"
#include "UiSceneProxyBase.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiRectangle;
}
} // namespace EngineCore

using namespace EngineCore::ShaderImpl;

namespace Graphics {
namespace Proxy {
class UiRectangleSceneProxy : public UiSceneProxyBase {
protected:
    std::shared_ptr<UiRectangleShader> mUiRectangleShader;

    glm::vec3 mColor;

    float mOpacity;

    float mBorderRadius;

    bool mIsRoundTop;

    bool mIsRoundBottom;

public:
    UiRectangleSceneProxy(const ::EngineCore::GUI::UiRectangle* uiRectangle);

    ~UiRectangleSceneProxy() override;

    void Render() override;

    void SetColor(const glm::vec3& color);

    void SetOpacity(const float opacity);

    void SetBorderRadius(const float borderRadiusPx);

    void SetIsRoundTop(const bool bIsRoundTop);

    void SetIsRoundBottom(const bool bIsRoundBottom);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;
};
} // namespace Proxy
} // namespace Graphics