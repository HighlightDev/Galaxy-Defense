#pragma once

#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
#include "Implementation/GUI/Shaders/UiConnectionLineShader.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>

namespace EngineCore::GUI {
class UiConnectionLine;
} // namespace EngineCore::GUI

namespace Game {

class UiConnectionLineSceneProxy : public ::Graphics::Proxy::UiSceneProxyBase {
    std::shared_ptr<UiConnectionLineShader> mShader;

    glm::vec2 mStartPoint;
    glm::vec2 mEndPoint;

    glm::vec3 mColor;

    float mThicknessPx;
    float mDashLengthPx;
    float mGapLengthPx;
    float mOpacity;

public:
    explicit UiConnectionLineSceneProxy(const ::EngineCore::GUI::UiConnectionLine* uiConnectionLine);

    ~UiConnectionLineSceneProxy() override;

    void Render(
        const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
        const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy) override;

    void SetStartPoint(const glm::vec2& startPoint);

    void SetEndPoint(const glm::vec2& endPoint);

    void SetColor(const glm::vec3& color);

    void SetThicknessPx(const float thicknessPx);

    void SetDashLengthPx(const float dashLengthPx);

    void SetGapLengthPx(const float gapLengthPx);

    void SetOpacity(const float opacity);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;
};

} // namespace Game
