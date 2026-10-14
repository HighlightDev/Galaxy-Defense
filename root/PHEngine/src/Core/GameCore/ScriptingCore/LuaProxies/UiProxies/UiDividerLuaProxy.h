#pragma once

#include "Core/GameCore/GUI/Common/DividerOrientation.h"
#include "UiItemBaseLuaProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiDivider;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiDividerLuaProxy : public UiItemBaseLuaProxy {
protected:
    glm::vec3 mColor;

    float mOpacity;

    float mLineWidthPx;

    float mEdgeFade;

    ::EngineCore::GUI::eDividerOrientation mOrientation;

public:
    explicit UiDividerLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiDivider>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetColor_FromGameThread(const glm::vec3& color);

    void SetOpacity_FromGameThread(const float opacity);

    void SetLineWidthPx_FromGameThread(const float widthPx);

    void SetEdgeFade_FromGameThread(const float edgeFade);

    void SetDividerOrientation_FromGameThread(const ::EngineCore::GUI::eDividerOrientation orientation);
};
} // namespace Scripts
} // namespace EngineCore
