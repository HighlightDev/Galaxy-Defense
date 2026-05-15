#pragma once

#include "Core/GameCore/ShaderImplementation/UiRectangleShader.h"
#include "UiSceneProxyBase.h"

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

public:
    explicit UiScrollListSceneProxy(const ::EngineCore::GUI::UiScrollList* uiScrollList);

    ~UiScrollListSceneProxy() override = default;

    void Render() override;

    void OnSceneProxyRegistered() override;

    void CleanUp() override;
};
} // namespace Proxy
} // namespace Graphics
