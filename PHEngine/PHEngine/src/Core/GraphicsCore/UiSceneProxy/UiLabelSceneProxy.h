#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/GameCore/ShaderImplementation/FontRenderingShader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "UiSceneProxyBase.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiLabel;
class FreeTypeTextFieldProxy;
} // namespace GUI
} // namespace EngineCore

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Texture;

namespace Graphics {

namespace Proxy {
class UiLabelSceneProxy : public UiSceneProxyBase {
    std::shared_ptr<FontRenderingShader> mUiLabelShader;

    std::string mText;

    const std::string mFontName;

    std::shared_ptr<ITexture> mFontTexture;

    float mOpacity;

    std::shared_ptr<::EngineCore::GUI::FreeTypeTextFieldProxy> mTextFieldProxy;

    glm::ivec2 mTextLineWidthHeight;

    int32_t mFontSize;

    ::EngineCore::eTextHorizontalAlignmentType mTextHorizontalAlignment;

    ::EngineCore::eTextVerticalAlignmentType mTextVerticalAlignment;

    glm::vec3 mTextColor;

    glm::vec2 mTextAlignmentOffset;

public:
    UiLabelSceneProxy(const ::EngineCore::GUI::UiLabel* uiLabel);

    ~UiLabelSceneProxy() override;

    void Render() override;

    void SetText(const std::string& text);

    void SetOpacity(const float opacity);

    void SetTextLineWidthHeight(const glm::ivec2& textLineWidthHeight);

    void SetFontSize(const int32_t fontSize);

    void SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment);

    void SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment);

    void SetTextColor(const glm::vec3& textColor);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;

private:
    void Initialize();

    void CalculateTextAlignmentOffset();
};
} // namespace Proxy
} // namespace Graphics