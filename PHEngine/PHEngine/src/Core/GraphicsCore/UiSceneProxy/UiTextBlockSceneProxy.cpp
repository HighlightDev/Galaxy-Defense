#include "UiTextBlockSceneProxy.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontAtlas.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontHandler.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeTextFieldProxy.h"
#include "Core/GameCore/GUI/UiElements/UiTextBlock.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/UtilityCore/EngineMath.h"
#include "UiCanvasSceneProxy.h"

using namespace Resources;
using namespace IO;
using namespace Graphics;
using namespace EngineCore::GUI;
using namespace EngineCore;
using namespace Graphics::Renderer;

namespace Graphics::Proxy {
UiTextBlockSceneProxy::UiTextBlockSceneProxy(const UiTextBlock* uiTextBlock)
    : UiSceneProxyBase(uiTextBlock)
    , mText("")
    , mFontName(uiTextBlock->GetFontName())
    , mOpacity(uiTextBlock->GetOpacity())
    , mTextLineWidthHeight(uiTextBlock->GetTextLineWidthHeight())
    , mFontSize(uiTextBlock->GetFontSize())
    , mTextHorizontalAlignment(uiTextBlock->GetTextHorizontalAlignment())
    , mTextVerticalAlignment(uiTextBlock->GetTextVerticalAlignment())
    , mTextColor(uiTextBlock->GetTextColor())
    , mRectangleColor(uiTextBlock->GetRectangleColor())
    , mRectangleOpacity(uiTextBlock->GetRectangleOpacity())
    , mRectangleRadius(uiTextBlock->GetRectangleRadius())
    , mBorderColor(uiTextBlock->GetBorderColor())
    , mBorderRadius(uiTextBlock->GetBorderRadius())
    , mBorderOpacity(uiTextBlock->GetBorderOpacity())
    , mBorderThickness(uiTextBlock->GetBorderThickness())
    , mBorderAspectRatioFactor(1.0f)
    , mSchrinkScaleToFitText(1.0f)
{
}

UiTextBlockSceneProxy::~UiTextBlockSceneProxy()
{
}

void UiTextBlockSceneProxy::OnSceneProxyRegistered()
{
    Initialize();
}

void UiTextBlockSceneProxy::Initialize()
{
    const auto& folderManager = FolderManager::GetInstance();
    {
        ShaderParams shaderParams(
            "UiRectangle Shader",
            folderManager->GetShadersPath() + "uiVS.glsl",
            folderManager->GetShadersPath() + "uiRectangleFS.glsl",
            "",
            "",
            "",
            "");
        mUiRectangleShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiRectangleShader>(shaderParams);
    }

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            ShaderParams shaderParams(
                "UiLabel Shader",
                folderManager->GetShadersPath() + "fontVS.glsl",
                folderManager->GetShadersPath() + "fontFS.glsl",
                "",
                "",
                "",
                "");
            mUiLabelShader = ShaderPool::GetInstance()->template GetOrAllocateResource<FontRenderingShader>(shaderParams);
            mTextFieldProxy = FreeTypeTextFieldProxy::CreateTextFieldProxyInstance(
                UniqueFontTextIdGenerator::GenerateUniqueFontTextId(),
                eTextFieldProxyType::GUI_TEXT_FIELD,
                false,
                mText,
                mFontName,
                glm::vec2(),
                mTextColor,
                mFontSize,
                0,
                mTextHorizontalAlignment,
                mTextVerticalAlignment,
                mTextLineWidthHeight);
            onTextChanged();

            fontHandlerSp->RegisterText(mTextFieldProxy);
            mFontTexture = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize))->GetFontTextureAtlas();
        } else {
            LogInfo("UiTextBlockSceneProxy::Initialize: CRIT: FreeTypeFontHandler is null");
        }
    } else {
        LogInfo("UiTextBlockSceneProxy::Initialize: CRIT: CanvasProxy is null");
    }
}

void UiTextBlockSceneProxy::Render()
{
    const glm::vec2 boundariesPaddingFactor = glm::vec2(1.0f) - (glm::vec2(mBorderThickness) / glm::vec2(mWidthHeightPixels));

    // Render borders
    RenderRectangle(mBorderAspectRatioFactor, mBorderColor, mBorderOpacity, mBorderRadius);
    // Render background rectangle
    RenderRectangle(glm::vec2(boundariesPaddingFactor), mRectangleColor, mRectangleOpacity, mRectangleRadius);
    // Render text
    RenderText();
}

void UiTextBlockSceneProxy::SetText(const std::string& text)
{
    if (mText != text) {
        mText = text;

        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextChanged(mTextFieldProxy->GetTextFieldId(), mText);
                onTextChanged();
            }
        }
        CalculateTextAlignmentOffset();
        SchrinkToFitText();
    }
}

void UiTextBlockSceneProxy::SetTextLineWidthHeight(const glm::ivec2& textLineWidthHeight)
{
    if (textLineWidthHeight != mTextLineWidthHeight) {
        mTextLineWidthHeight = textLineWidthHeight;
        mTextFieldProxy->SetLineWidthHeight(mTextLineWidthHeight);
        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextChanged(mTextFieldProxy->GetTextFieldId(), mText);
                onTextChanged();
            }
        }
        CalculateTextAlignmentOffset();
        SchrinkToFitText();
    }
}

void UiTextBlockSceneProxy::SetFontSize(const int32_t fontSize)
{
    if (mFontSize != fontSize) {
        mFontSize = fontSize;
        mTextFieldProxy->SetFontSize(mFontSize);

        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->FontSizeChanged(mTextFieldProxy);
                onTextChanged();
                // change texture according to new font
                mFontTexture = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize))->GetFontTextureAtlas();
            }
        }
        CalculateTextAlignmentOffset();
        SchrinkToFitText();
    }
}

void UiTextBlockSceneProxy::SetOpacity(const float opacity)
{
    mOpacity = opacity;
}

void UiTextBlockSceneProxy::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    if (mTextHorizontalAlignment != textHorizontalAlignment) {
        mTextHorizontalAlignment = textHorizontalAlignment;
        mTextFieldProxy->SetTextHorizontalAlignment(textHorizontalAlignment);

        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextChanged(mTextFieldProxy->GetTextFieldId(), mText);
                onTextChanged();
            }
        }
        CalculateTextAlignmentOffset();
        SchrinkToFitText();
    }
}

void UiTextBlockSceneProxy::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment)
{
    if (mTextVerticalAlignment != textVerticalAlignment) {
        mTextVerticalAlignment = textVerticalAlignment;
        mTextFieldProxy->SetTextVerticalAlignment(textVerticalAlignment);
        CalculateTextAlignmentOffset();
        SchrinkToFitText();
    }
}

void UiTextBlockSceneProxy::SetTextColor(const glm::vec3& textColor)
{
    mTextColor = textColor;
}

void UiTextBlockSceneProxy::SetRectangleColor(const glm::vec3& rectangleColor)
{
    mRectangleColor = rectangleColor;
}

void UiTextBlockSceneProxy::SetRectangleOpacity(const float rectangleOpacity)
{
    mRectangleOpacity = rectangleOpacity;
}

void UiTextBlockSceneProxy::SetRectangleRadius(const float rectangleBorderRadius)
{
    mRectangleRadius = rectangleBorderRadius;
}

void UiTextBlockSceneProxy::SetBorderColor(const glm::vec3& borderColor)
{
    mBorderColor = borderColor;
}

void UiTextBlockSceneProxy::SetBorderRadius(const float borderRadius)
{
    mBorderRadius = borderRadius;
}

void UiTextBlockSceneProxy::SetBorderOpacity(const float borderOpacity)
{
    mBorderOpacity = borderOpacity;
}

void UiTextBlockSceneProxy::SetBorderThickness(const int32_t borderThickness)
{
    mBorderThickness = borderThickness;
}

void UiTextBlockSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiLabelShader);
    ShaderPool::GetInstance()->TryToFreeMemory(mUiRectangleShader);

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            fontHandlerSp->UnregisterText(mTextFieldProxy->GetTextFieldId());
        }
    }
}

void UiTextBlockSceneProxy::CalculateTextAlignmentOffset()
{
    if (mTextHorizontalAlignment == eTextHorizontalAlignmentType::LEFT) {
        mTextAlignmentOffset.x = 0.0f;
    } else if (mTextHorizontalAlignment == eTextHorizontalAlignmentType::CENTER) {
        mTextAlignmentOffset.x
            = (GetNormalizedWidthHeight().x * 0.5f) - (mTextFieldProxy->GetCreatedMeshTextWidthHeightNormalized().x * 0.5f);
    } else if (mTextHorizontalAlignment == eTextHorizontalAlignmentType::RIGHT) {
        mTextAlignmentOffset.x = GetNormalizedWidthHeight().x - mTextFieldProxy->GetCreatedMeshTextWidthHeightNormalized().x;
    }

    // Free type text start coordinates are from the bottom left corner
    if (mTextVerticalAlignment == eTextVerticalAlignmentType::BOTTOM) {
        mTextAlignmentOffset.y = 0.0f;
    } else if (mTextVerticalAlignment == eTextVerticalAlignmentType::CENTER) {
        mTextAlignmentOffset.y
            = (GetNormalizedWidthHeight().y * 0.5f) - (mTextFieldProxy->GetCreatedMeshTextWidthHeightNormalized().y * 0.5f);
    } else if (mTextVerticalAlignment == eTextVerticalAlignmentType::TOP) {
        mTextAlignmentOffset.y = GetNormalizedWidthHeight().y - mTextFieldProxy->GetCreatedMeshTextWidthHeightNormalized().y;
    }
}

void UiTextBlockSceneProxy::SchrinkToFitText()
{
    const glm::vec2 boundariesPaddingFactor = glm::vec2(1.0f) - (glm::vec2(mBorderThickness) / glm::vec2(mWidthHeightPixels));

    mSchrinkScaleToFitText
        = (glm::vec2(mTextFieldProxy->GetCreatedMeshTextWidthHeightScreenSpace()) + glm::vec2(mBorderThickness))
        / glm::vec2(mWidthHeightPixels);

    mBorderAspectRatioFactor = glm::vec2(1.0f);
    if (mSchrinkScaleToFitText.x > mSchrinkScaleToFitText.y) {
        float diff = mSchrinkScaleToFitText.x - mSchrinkScaleToFitText.y;
        diff -= diff * boundariesPaddingFactor.x;
        mBorderAspectRatioFactor.x -= (diff / (float)mSchrinkScaleToFitText.x);
    } else if (mSchrinkScaleToFitText.x < mSchrinkScaleToFitText.y) {
        float diff = mSchrinkScaleToFitText.y - mSchrinkScaleToFitText.x;
        diff -= diff * boundariesPaddingFactor.y;
        mBorderAspectRatioFactor.y -= (diff / (float)mSchrinkScaleToFitText.y);
    }
}

void UiTextBlockSceneProxy::RenderText()
{
    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            const auto& renderDataSp = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize));
            mUiLabelShader->ExecuteShader();
            const auto textHeightScreenSpace = mTextFieldProxy->GetCreatedMeshTextWidthHeightNormalized().y;
            mUiLabelShader->SetPosition(glm::vec2(
                mNormalizedTranslation.x + mCenterOffset.x + mTextAlignmentOffset.x,
                1.0f - (mNormalizedTranslation.y + mCenterOffset.y + textHeightScreenSpace + mTextAlignmentOffset.y)));
            mFontTexture->BindTexture(0);
            mUiLabelShader->SetFontAtlasSlot(0);
            mUiLabelShader->SetOpacity(mOpacity * mOverlayOpacity);
            mUiLabelShader->SetColor(mTextColor);
            renderDataSp->GetFreeTypeFontAtlas()->GetBuffer()->RenderVAO(
                mTextFieldProxy->GetVertexStart(), mTextFieldProxy->GetVerticesCount(), GL_TRIANGLES);
            mUiLabelShader->StopShader();
        }
    }
}

void UiTextBlockSceneProxy::RenderRectangle(
    const glm::vec2& scale, const glm::vec3& color, const float opacity, const float borderRadius)
{
    mUiRectangleShader->ExecuteShader();
    const glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * scale * mSchrinkScaleToFitText)) * 0.5f);
    mUiRectangleShader->SetTransform(
        mNormalizedTranslation + scaleOffset + mCenterOffset, mNormalizedScale * scale * mSchrinkScaleToFitText);
    mUiRectangleShader->SetColor(color);
    mUiRectangleShader->SetOpacity(opacity * mOverlayOpacity);
    mUiRectangleShader->SetBorderRadius(borderRadius);
    mUiRectangleShader->SetWidthHeightPixels(
        glm::vec2(static_cast<float>(mWidthHeightPixels.x), static_cast<float>(mWidthHeightPixels.y)));
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
    mUiRectangleShader->StopShader();
}

void UiTextBlockSceneProxy::onTextChanged()
{
    if (const auto& rendererSp = mSceneRendererWp.lock()) {
        rendererSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mUiItemUId,
            Hash64_CT("UiTextBlockSceneProxy::onTextChanged"),
            [rendererSp,
             uiItemUId = mUiItemUId,
             createdMeshTextWidthHeightNormalized = mTextFieldProxy->GetCreatedMeshTextWidthHeightNormalized(),
             createdMeshTextWidthHeightScreenSpace = mTextFieldProxy->GetCreatedMeshTextWidthHeightScreenSpace()](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneSp = sceneWp.lock()) {
                    const auto uiItem = sceneSp->GetUiHandler()->GetUiItemByUId(uiItemUId);
                    assert(uiItem);
                    const auto textBlockSp = std::dynamic_pointer_cast<UiTextBlock>(uiItem);
                    assert(textBlockSp);
                    textBlockSp->SetTextNormalizedSize(createdMeshTextWidthHeightNormalized);
                    textBlockSp->SetTextScreenSpaceSize(createdMeshTextWidthHeightScreenSpace);
                }
            });
    }
}
} // namespace Graphics::Proxy