#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/GameCore/ShaderImplementation/FontRenderingShader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/UiSceneProxy/UiRectangleSceneProxy.h"
#include "UiSceneProxyBase.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EngineCore::GUI {
class UiTextBlock;
class FreeTypeTextFieldProxy;
} // namespace EngineCore::GUI

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Texture;

namespace Graphics::Proxy {
class UiTextBlockSceneProxy : public UiRectangleSceneProxy {

    std::shared_ptr<FontRenderingShader> mUiLabelShader;

    std::string mText;

    const std::string mFontName;

    std::shared_ptr<ITexture> mFontTexture;

    float mOpacity;

    std::shared_ptr<::GUI::FreeTypeTextFieldProxy> mTextFieldProxy;

    glm::ivec2 mTextLineWidthHeight;

    int32_t mFontSize;

    ::EngineCore::eTextHorizontalAlignmentType mTextHorizontalAlignment;

    ::EngineCore::eTextVerticalAlignmentType mTextVerticalAlignment;

    glm::vec3 mTextColor;

    glm::vec2 mTextAlignmentOffset;

public:
    UiTextBlockSceneProxy(const ::EngineCore::GUI::UiTextBlock* uiTextBlock);

    ~UiTextBlockSceneProxy() override;

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
} // namespace Graphics::Proxy