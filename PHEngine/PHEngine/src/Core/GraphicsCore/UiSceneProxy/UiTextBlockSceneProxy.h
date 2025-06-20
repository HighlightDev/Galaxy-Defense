#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/GameCore/ShaderImplementation/FontRenderingShader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/UiSceneProxy/UiRectangleSceneProxy.h"
#include "UiSceneProxyBase.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiTextBlock;
}
} // namespace EngineCore

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Texture;

namespace Graphics {
class TextFieldProxy;

namespace Proxy {
class UiTextBlockSceneProxy : public UiRectangleSceneProxy {

    std::shared_ptr<FontRenderingShader> mUiLabelShader;

    std::string mText;

    const std::string mFontName;

    std::shared_ptr<ITexture> mFontTexture;

    float mOpacity;

    std::shared_ptr<::Graphics::TextFieldProxy> mTextFieldProxy;

    float mTextLineWidth;

    float mFontSize;

    ::EngineCore::eTextHorizontalAlignmentType mTextHorizontalAlignment;

    glm::vec3 mTextColor;

public:
    UiTextBlockSceneProxy(const ::EngineCore::GUI::UiTextBlock* uiTextBlock);

    ~UiTextBlockSceneProxy() override;

    void Render() override;

    void SetText(const std::string& text);

    void SetOpacity(const float opacity);

    void SetTextLineWidth(const float textLineWidth);

    void SetFontSize(const float fontSize);

    void SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment);

    void SetTextColor(const glm::vec3& textColor);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;

private:
    void Initialize();
};
} // namespace Proxy
} // namespace Graphics