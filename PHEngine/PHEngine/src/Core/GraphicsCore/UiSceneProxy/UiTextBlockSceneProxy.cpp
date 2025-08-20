#include "UiTextBlockSceneProxy.h"

#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontAtlas.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontHandler.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeTextFieldProxy.h"
#include "Core/GameCore/GUI/UiElements/UiTextBlock.h"
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
UiTextBlockSceneProxy::UiTextBlockSceneProxy(const UiTextBlock* uiTextBlock)
    : UiRectangleSceneProxy(uiTextBlock)
    , mText("")
    , mFontName(uiTextBlock->GetFontName())
    , mOpacity(uiTextBlock->GetOpacity())
    , mTextLineWidthHeight(uiTextBlock->GetTextLineWidthHeight())
    , mFontSize(uiTextBlock->GetFontSize())
    , mTextHorizontalAlignment(uiTextBlock->GetTextHorizontalAlignment())
    , mTextVerticalAlignment(uiTextBlock->GetTextVerticalAlignment())
    , mTextColor(uiTextBlock->GetTextColor())
{
}

UiTextBlockSceneProxy::~UiTextBlockSceneProxy()
{
}

void UiTextBlockSceneProxy::OnSceneProxyRegistered()
{
    UiRectangleSceneProxy::OnSceneProxyRegistered();

    Initialize();
}

void UiTextBlockSceneProxy::Initialize()
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
            LogInfo("UiTextBlockSceneProxy::Initialize => CRIT: FreeTypeFontHandler is null");
        }
    } else {
        LogInfo("UiTextBlockSceneProxy::Initialize => CRIT: CanvasProxy is null");
    }
}

void UiTextBlockSceneProxy::Render()
{
    UiRectangleSceneProxy::Render();

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            const auto& renderDataSp = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize));
            mUiLabelShader->ExecuteShader();
            const auto textHeightScreenSpace = mTextFieldProxy->GetCreatedMeshTextHeightTextureSpace();
            mUiLabelShader->SetPosition(glm::vec2(
                mNormalizedTranslation.x + mCenterOffset.x,
                1.0f - (mNormalizedTranslation.y + mCenterOffset.y + textHeightScreenSpace)));
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

void UiTextBlockSceneProxy::SetText(const std::string& text)
{
    if (mText != text) {
        mText = text;

        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextChanged(mTextFieldProxy->GetTextFieldId(), mText);
            }
        }
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
            }
        }
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
                // change texture according to new font
                mFontTexture = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize))->GetFontTextureAtlas();
            }
        }
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
            }
        }
    }
}

void UiTextBlockSceneProxy::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment)
{
    if (mTextVerticalAlignment != textVerticalAlignment) {
        mTextVerticalAlignment = textVerticalAlignment;
        mTextFieldProxy->SetTextVerticalAlignment(textVerticalAlignment);
    }
}

void UiTextBlockSceneProxy::SetTextColor(const glm::vec3& textColor)
{
    mTextColor = textColor;
}

void UiTextBlockSceneProxy::CleanUp()
{
    UiRectangleSceneProxy::CleanUp();

    ShaderPool::GetInstance()->TryToFreeMemory(mUiLabelShader);

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            fontHandlerSp->UnregisterText(mTextFieldProxy->GetTextFieldId());
        }
    }
}
} // namespace Proxy
} // namespace Graphics