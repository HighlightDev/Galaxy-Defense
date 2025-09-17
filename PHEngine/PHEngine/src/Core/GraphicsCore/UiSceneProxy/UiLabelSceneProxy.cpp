#include "UiLabelSceneProxy.h"

#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontAtlas.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontHandler.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeTextFieldProxy.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/UtilityCore/EngineMath.h"
#include "UiCanvasSceneProxy.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace Graphics;
using namespace EngineCore::GUI;
using namespace EngineCore;

namespace Graphics {
namespace Proxy {
UiLabelSceneProxy::UiLabelSceneProxy(const UiLabel* uiLabel)
    : UiSceneProxyBase(uiLabel)
    , mText("")
    , mFontName(uiLabel->GetFontName())
    , mOpacity(uiLabel->GetOpacity())
    , mTextLineWidthHeight(uiLabel->GetTextLineWidthHeight())
    , mFontSize(uiLabel->GetFontSize())
    , mTextHorizontalAlignment(uiLabel->GetTextHorizontalAlignment())
    , mTextColor(uiLabel->GetTextColor())
{
}

UiLabelSceneProxy::~UiLabelSceneProxy()
{
}

void UiLabelSceneProxy::OnSceneProxyRegistered()
{
    Initialize();
}

void UiLabelSceneProxy::Initialize()
{
    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            const auto& folderManager = FolderManager::GetInstance();
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
                mTextLineWidthHeight,
                false);
            fontHandlerSp->RegisterText(mTextFieldProxy);

            mFontTexture = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize))->GetFontTextureAtlas();
        } else {
            LogInfo("UiLabelSceneProxy::Initialize => CRIT: FreeTypeFontHandler is null");
        }
    } else {
        LogInfo("UiLabelSceneProxy::Initialize => CRIT: CanvasProxy is null");
    }
}

void UiLabelSceneProxy::Render()
{
    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            const auto& renderDataSp = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize));
            mUiLabelShader->ExecuteShader();
            const auto textHeightTextureSpace = mTextFieldProxy->GetCreatedMeshTextWidthHeightNormalized().y;
            mUiLabelShader->SetPosition(glm::vec2(
                mNormalizedTranslation.x + mCenterOffset.x + mTextAlignmentOffset.x,
                1.0f - (mNormalizedTranslation.y + mCenterOffset.y + textHeightTextureSpace + mTextAlignmentOffset.y)));
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

void UiLabelSceneProxy::SetText(const std::string& text)
{
    if (mText != text) {
        mText = text;

        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextChanged(mTextFieldProxy->GetTextFieldId(), mText);
            }
        }
        CalculateTextAlignmentOffset();
    }
}

void UiLabelSceneProxy::SetTextLineWidthHeight(const glm::ivec2& textLineWidthHeight)
{
    if (textLineWidthHeight != mTextLineWidthHeight) {
        mTextLineWidthHeight = textLineWidthHeight;
        mTextFieldProxy->SetLineWidthHeight(mTextLineWidthHeight);
        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextChanged(mTextFieldProxy->GetTextFieldId(), mText);
            }
        }
        CalculateTextAlignmentOffset();
    }
}

void UiLabelSceneProxy::SetFontSize(const int32_t fontSize)
{
    if (mFontSize != fontSize) {
        mFontSize = fontSize;
        mTextFieldProxy->SetFontSize(mFontSize);

        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->FontSizeChanged(mTextFieldProxy);
                // change texture according to new font
                mFontTexture = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize))->GetFontTextureAtlas();
            }
        }
        CalculateTextAlignmentOffset();
    }
}

void UiLabelSceneProxy::SetOpacity(const float opacity)
{
    mOpacity = opacity;
}

void UiLabelSceneProxy::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    if (mTextHorizontalAlignment != textHorizontalAlignment) {
        mTextHorizontalAlignment = textHorizontalAlignment;
        mTextFieldProxy->SetTextHorizontalAlignment(textHorizontalAlignment);

        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextChanged(mTextFieldProxy->GetTextFieldId(), mText);
            }
        }
        CalculateTextAlignmentOffset();
    }
}

void UiLabelSceneProxy::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment)
{
    if (mTextVerticalAlignment != textVerticalAlignment) {
        mTextVerticalAlignment = textVerticalAlignment;
        mTextFieldProxy->SetTextVerticalAlignment(textVerticalAlignment);
        CalculateTextAlignmentOffset();
    }
}

void UiLabelSceneProxy::SetTextColor(const glm::vec3& textColor)
{
    mTextColor = textColor;
}

void UiLabelSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiLabelShader);

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            fontHandlerSp->UnregisterText(mTextFieldProxy->GetTextFieldId());
        }
    }
}

void UiLabelSceneProxy::CalculateTextAlignmentOffset()
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
} // namespace Proxy
} // namespace Graphics