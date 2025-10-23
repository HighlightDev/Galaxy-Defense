#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/GameCore/ShaderImplementation/FontRenderingShader.h"
#include "Core/GameCore/ShaderImplementation/UiRectangleShader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
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
class UiTextBlockSceneProxy : public UiSceneProxyBase {

    std::shared_ptr<FontRenderingShader> mUiLabelShader;

    std::shared_ptr<UiRectangleShader> mUiRectangleShader;

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

    glm::vec3 mRectangleColor;

    float mRectangleOpacity;

    float mRectangleRadius;

    glm::vec3 mBorderColor;

    float mBorderRadius;

    float mBorderOpacity;

    glm::vec2 mSchrinkScaleToFitText;

    glm::vec2 mBorderAspectRatioFactor;

    int32_t mBorderThickness;

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

    void SetRectangleColor(const glm::vec3& rectangleColor);

    void SetRectangleOpacity(const float rectangleOpacity);

    void SetRectangleRadius(const float rectangleBorderRadius);

    void SetBorderColor(const glm::vec3& borderColor);

    void SetBorderRadius(const float borderRadius);

    void SetBorderOpacity(const float borderOpacity);

    void SetBorderThickness(const int32_t borderThickness);

    void CleanUp() override;

    void OnSceneProxyRegistered() override;

private:
    void Initialize();

    void CalculateTextAlignmentOffset();

    /**
     * @brief Calculates and updates the scale of the text block boundaries to fit the rendered text.
     *
     * This method adjusts the `mScale` member variable so that the text block's boundaries
     * are scaled to precisely fit the size of the text as rendered on the screen.
     *
     * The resulting scale ensures that the text block visually matches the size of the text,
     * preventing overflow or excessive padding.
     */
    void SchrinkToFitText();

    void RenderText();

    void RenderRectangle(const glm::vec2& scale, const glm::vec3& color, const float opacity, const float borderRadius);

    void onTextChanged();
};
} // namespace Graphics::Proxy