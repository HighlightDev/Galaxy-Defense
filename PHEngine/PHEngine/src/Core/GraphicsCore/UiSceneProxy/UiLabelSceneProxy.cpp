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
    , mTextLineWidth(uiLabel->GetTextLineWidth())
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
                "13_5Atom_Sans_Regular",
                glm::vec2(),
                mTextColor,
                mFontSize,
                0,
                mTextHorizontalAlignment,
                800,
                600,
                false);
            fontHandlerSp->RegisterText(mTextFieldProxy);

            mFontTexture
                = fontHandlerSp->GetFontBatcher(FreeTypeFontParams("13_5Atom_Sans_Regular", mFontSize))->GetFontTextureAtlas();
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
            const auto& renderDataSp = fontHandlerSp->GetFontBatcher(FreeTypeFontParams("13_5Atom_Sans_Regular", mFontSize));
            mUiLabelShader->ExecuteShader();
            const auto textHeightScreenSpace = mTextFieldProxy->GetCreatedMeshTextHeight();
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

void UiLabelSceneProxy::SetText(const std::string& text)
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

void UiLabelSceneProxy::SetTextLineWidth(const float textLineWidth)
{
    if (!EngineMath::FloatsNearEqual(textLineWidth, mTextLineWidth)) {
        mTextLineWidth = textLineWidth;
        mTextFieldProxy->SetLineWidth(mTextLineWidth);
        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextChanged(mTextFieldProxy->GetTextFieldId(), mText);
            }
        }
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
                mFontTexture = fontHandlerSp->GetFontBatcher(FreeTypeFontParams("13_5Atom_Sans_Regular", mFontSize))
                                   ->GetFontTextureAtlas();
            }
        }
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
} // namespace Proxy
} // namespace Graphics