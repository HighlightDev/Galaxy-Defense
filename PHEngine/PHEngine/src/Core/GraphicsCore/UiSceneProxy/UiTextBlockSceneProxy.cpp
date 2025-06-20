#include "UiTextBlockSceneProxy.h"

#include "Core/GameCore/GUI/Common/FontHandler.h"
#include "Core/GameCore/GUI/Common/TextFieldProxy.h"
#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"
#include "Core/GameCore/GUI/UiElements/UiTextBlock.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
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
    , mTextLineWidth(uiTextBlock->GetTextLineWidth())
    , mFontSize(uiTextBlock->GetFontSize())
    , mTextHorizontalAlignment(uiTextBlock->GetTextHorizontalAlignment())
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

            mFontTexture = fontHandlerSp->GetFontBatcher(mFontName)->GetFontTextureAtlas();

            mTextFieldProxy = TextFieldProxy::CreateTextFieldProxyInstance(
                UniqueFontTextIdGenerator::GenerateUniqueFontTextId(),
                eTextFieldProxyType::GUI_TEXT_FIELD,
                false,
                mText,
                mFontName,
                glm::vec2(),
                mTextColor,
                mFontSize,
                mTextHorizontalAlignment,
                mTextLineWidth,
                1,
                false);

            fontHandlerSp->RegisterText(mTextFieldProxy);
        } else {
            LogInfo("UiTextBlockSceneProxy::Initialize => CRIT: FontHandler is null");
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
            const auto& renderDataSp = fontHandlerSp->GetFontBatcher(mFontName);
            mUiLabelShader->ExecuteShader();
            const auto textHeightScreenSpace = mTextFieldProxy->GetCreatedMeshTextHeight();
            mUiLabelShader->SetPosition(
                glm::vec2(
                    mNormalizedTranslation.x + mCenterOffset.x,
                    1.0f - (mNormalizedTranslation.y + mCenterOffset.y + textHeightScreenSpace)));
            mFontTexture->BindTexture(0);
            mUiLabelShader->SetFontAtlasSlot(0);
            mUiLabelShader->SetOpacity(mOpacity * mOverlayOpacity);
            mUiLabelShader->SetColor(mTextColor);
            renderDataSp->GetTextMesh()->GetBuffer()->RenderVAO(
                mTextFieldProxy->GetVertexStart(), mTextFieldProxy->GetVerticesCount(), GL_TRIANGLES);
            mUiLabelShader->StopShader();
        }
    }
}

void UiTextBlockSceneProxy::SetText(const std::string& text)
{
    mText = text;

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            fontHandlerSp->TextChanged(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId(), mText);
        }
    }
}

void UiTextBlockSceneProxy::SetTextLineWidth(const float textLineWidth)
{
    mTextLineWidth = textLineWidth;
    mTextFieldProxy->SetLineMaxWidth(mTextLineWidth);

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            fontHandlerSp->TextChanged(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId(), mText);
        }
    }
}

void UiTextBlockSceneProxy::SetFontSize(const float fontSize)
{
    mFontSize = fontSize;
    mTextFieldProxy->SetFontSize(mFontSize);

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            fontHandlerSp->TextChanged(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId(), mText);
        }
    }
}

void UiTextBlockSceneProxy::SetOpacity(const float opacity)
{
    mOpacity = opacity;
}

void UiTextBlockSceneProxy::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    mTextHorizontalAlignment = textHorizontalAlignment;
    mTextFieldProxy->SetTextHorizontalAlignment(textHorizontalAlignment);

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            fontHandlerSp->TextChanged(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId(), mText);
        }
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
            fontHandlerSp->UnregisterText(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId());
        }
    }
}
} // namespace Proxy
} // namespace Graphics