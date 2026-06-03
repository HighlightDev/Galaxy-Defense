#include "UiLabelSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontAtlas.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontHandler.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeTextFieldProxy.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
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
using namespace Graphics::Renderer;

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
    , mTextGradientColorType(uiLabel->GetTextGradientColorType())
    , mGradientTextColorStart(uiLabel->GetGradientTextColorStart())
    , mGradientTextColorEnd(uiLabel->GetGradientTextColorEnd())
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
            ShaderParams shaderParams("UiLabel Shader");
            shaderParams.SetMainShaders(
                folderManager->GetAbsolutePath("fontVS.glsl"), folderManager->GetAbsolutePath("fontFS.glsl"));
            mUiLabelShader = ShaderPool::GetInstance()->template GetOrAllocateResource<FontRenderingShader>(shaderParams);
            mTextFieldProxy = FreeTypeTextFieldProxy::CreateTextFieldProxyInstance(
                UniqueFontTextIdGenerator::GenerateUniqueFontTextId(),
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
            fontHandlerSp->RegisterText(mTextFieldProxy);
            onTextChanged();

            mFontTexture = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize))->GetFontTextureAtlas();
        } else {
            LogInfo("UiLabelSceneProxy::Initialize: CRIT: FreeTypeFontHandler is null");
        }
    } else {
        LogInfo("UiLabelSceneProxy::Initialize: CRIT: CanvasProxy is null");
    }
}

void UiLabelSceneProxy::Render(
    const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
    const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy)
{
    if (mTextFieldProxy->GetVerticesCount() == 0) {
        return;
    }

    if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
        if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
            const auto& renderDataSp = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize));
            mUiLabelShader->ExecuteShader();

            const auto normalizedWidthHeight = GetNormalizedWidthHeight();
            const auto textNormSize = mTextFieldProxy->GetCreatedMeshTextWidthHeightNormalized();
            const auto textAlignmentOffset = CalculateTextAlignmentOffset(normalizedWidthHeight, textNormSize);

            mUiLabelShader->SetPosition(
                glm::vec2(
                    mNormalizedTranslation.x + mCenterOffset.x + textAlignmentOffset.x,
                    1.0f - (mNormalizedTranslation.y + mCenterOffset.y + textNormSize.y + textAlignmentOffset.y)));
            mFontTexture->BindTexture(0);
            mUiLabelShader->SetFontAtlasSlot(0);
            mUiLabelShader->SetOpacity(mOpacity * mOverlayOpacity);
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
                onTextChanged();
            }
        }
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
                onTextChanged();
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
                onTextChanged();
                // change texture according to new font
                mFontTexture = fontHandlerSp->GetFontBatcher(FreeTypeFontParams(mFontName, mFontSize))->GetFontTextureAtlas();
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
                onTextChanged();
            }
        }
    }
}

void UiLabelSceneProxy::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment)
{
    if (mTextVerticalAlignment != textVerticalAlignment) {
        mTextVerticalAlignment = textVerticalAlignment;
        mTextFieldProxy->SetTextVerticalAlignment(textVerticalAlignment);
    }
}

void UiLabelSceneProxy::SetTextColor(const glm::vec3& textColor)
{
    if (not EngineMath::CheckSimilarityVec3(mTextColor, textColor)) {
        mTextColor = textColor;
        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextColorChanged(mTextFieldProxy->GetTextFieldId(), textColor);
            }
        }
    }
}

void UiLabelSceneProxy::SetGradientColor(
    const eTextGradientColorType textGradientColorType, const glm::vec3& gradientColorStart, const glm::vec3& gradientColorEnd)
{
    if (mTextGradientColorType != textGradientColorType
        || not EngineMath::CheckSimilarityVec3(mGradientTextColorStart, gradientColorStart)
        || not EngineMath::CheckSimilarityVec3(mGradientTextColorEnd, gradientColorEnd)) {
        mTextGradientColorType = textGradientColorType;
        mGradientTextColorStart = gradientColorStart;
        mGradientTextColorEnd = gradientColorEnd;
        if (const auto& canvasProxySp = mParentCanvasProxy.lock()) {
            if (const auto& fontHandlerSp = canvasProxySp->GetFontHandler().lock()) {
                fontHandlerSp->TextColorGradientChanged(
                    mTextFieldProxy->GetTextFieldId(), textGradientColorType, gradientColorStart, gradientColorEnd);
            }
        }
    }
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

glm::vec2
UiLabelSceneProxy::CalculateTextAlignmentOffset(const glm::vec2& normalizedWidthHeight, const glm::vec2& textNormalizedSize) const
{
    glm::vec2 offset(0.0f);

    if (mTextHorizontalAlignment == eTextHorizontalAlignmentType::LEFT) {
        offset.x = 0.0f;
    } else if (mTextHorizontalAlignment == eTextHorizontalAlignmentType::CENTER) {
        offset.x = (normalizedWidthHeight.x * 0.5f) - (textNormalizedSize.x * 0.5f);
    } else if (mTextHorizontalAlignment == eTextHorizontalAlignmentType::RIGHT) {
        offset.x = normalizedWidthHeight.x - textNormalizedSize.x;
    }

    // Free type text start coordinates are from the bottom left corner
    if (mTextVerticalAlignment == eTextVerticalAlignmentType::BOTTOM) {
        offset.y = 0.0f;
    } else if (mTextVerticalAlignment == eTextVerticalAlignmentType::CENTER) {
        offset.y = (normalizedWidthHeight.y * 0.5f) - (textNormalizedSize.y * 0.5f);
    } else if (mTextVerticalAlignment == eTextVerticalAlignmentType::TOP) {
        offset.y = normalizedWidthHeight.y - textNormalizedSize.y;
    }

    return offset;
}

void UiLabelSceneProxy::onTextChanged()
{
    if (const auto& rendererSp = mSceneRendererWp.lock()) {
        rendererSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mUiItemUId,
            Hash64_CT("UiLabelSceneProxy::onTextChanged"),
            [rendererSp,
             uiItemUId = mUiItemUId,
             createdMeshTextWidthHeightNormalized = mTextFieldProxy->GetCreatedMeshTextWidthHeightNormalized(),
             createdMeshTextWidthHeightScreenSpace = mTextFieldProxy->GetCreatedMeshTextWidthHeightScreenSpace()](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneSp = sceneWp.lock()) {
                    const auto uiItem = sceneSp->GetUiHandler()->GetUiItemByUId(uiItemUId);
                    ext_assert(
                        uiItem, "UiLabelSceneProxy::onTextChanged: UiItem not found for UId: " + std::to_string(uiItemUId));
                    const auto labelSp = std::dynamic_pointer_cast<UiLabel>(uiItem);
                    ext_assert(
                        labelSp,
                        "UiLabelSceneProxy::onTextChanged: Failed to cast UiItem to UiLabel for UId: "
                            + std::to_string(uiItemUId));
                    labelSp->SetTextNormalizedSize(createdMeshTextWidthHeightNormalized);
                    labelSp->SetTextScreenSpaceSize(createdMeshTextWidthHeightScreenSpace);
                }
            });
    }
}
} // namespace Proxy
} // namespace Graphics