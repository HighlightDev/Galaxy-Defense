#pragma once

#include "Core/GameCore/GUI/UiElements/UiItemBase.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Graphics {
namespace Proxy {
class UiSceneProxyBase;
}
} // namespace Graphics

namespace EngineCore {
namespace Scripts {
class LuaProxy;
}
} // namespace EngineCore

namespace EngineCore {
namespace GUI {
class UiCanvas;

class UiConnectionLine : public UiItemBase {
    glm::vec2 mStartPoint;
    glm::vec2 mEndPoint;

    // Если имя задано — соответствующий endpoint привязывается к центру виджета с этим именем
    // и пересчитывается при смещении цели (скролл и т.п.). Пусто — endpoint задаётся вручную.
    std::string mStartAnchorTarget;
    std::string mEndAnchorTarget;

    glm::vec3 mColor;

    float mThicknessPx;
    float mDashLengthPx;
    float mGapLengthPx;

    float mOpacity;

public:
    explicit UiConnectionLine(const std::string& name = std::string(""));

    ~UiConnectionLine() override;

    void SetStartPoint(const glm::vec2& startPoint);

    glm::vec2 GetStartPoint() const;

    void SetEndPoint(const glm::vec2& endPoint);

    glm::vec2 GetEndPoint() const;

    void SetStartAnchorTarget(const std::string& targetUiItemName);

    std::string GetStartAnchorTarget() const;

    void SetEndAnchorTarget(const std::string& targetUiItemName);

    std::string GetEndAnchorTarget() const;

    void SetColor(const glm::vec3& color);

    void SetColor(const uint8_t r, const uint8_t g, const uint8_t b);

    glm::vec3 GetColor() const;

    void SetThicknessPx(const float thicknessPx);

    float GetThicknessPx() const;

    void SetDashLengthPx(const float dashLengthPx);

    float GetDashLengthPx() const;

    void SetGapLengthPx(const float gapLengthPx);

    float GetGapLengthPx() const;

    void SetOpacity(const float opacity);

    float GetOpacity() const;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    bool OnPropertiesShouldBeUpdatedOnRenderThread() override;

    bool OnPropertiesShouldBeUpdatedOnLuaThread() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetUiTypeString() const override;

    bool IsTransformDependentToUiItem(const std::string& uiItemName) const override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

    void RecalculateAnchorPositions() override;

private:
    void ResolveAnchoredEndpoints();

    bool SyncDataOnRenderThread();

    bool SyncDataOnLuaThread();
};
} // namespace GUI
} // namespace EngineCore
