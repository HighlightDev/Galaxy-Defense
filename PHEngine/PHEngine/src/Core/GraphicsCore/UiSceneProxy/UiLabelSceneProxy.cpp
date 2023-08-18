#include "UiLabelSceneProxy.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/GUI/Common/FontHandler.h"
#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"
#include "Core/GameCore/GUI/Common/TextFieldProxy.h"
#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/LoggerExtension.h"
#include "UiCanvasSceneProxy.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace Graphics;
using namespace EngineCore::GUI;
using namespace EngineCore;

namespace Graphics
{
    namespace Proxy
    {
        UiLabelSceneProxy::UiLabelSceneProxy(const UiLabel *uiLabel)
            : UiSceneProxyBase(uiLabel),
              mText(""),
              mFontName(uiLabel->GetFontName()),
              mOpacity(uiLabel->GetOpacity()),
              mTextLineWidth(uiLabel->GetTextLineWidth()),
              mFontSize(uiLabel->GetFontSize()),
              mTextHorizontalAlignment(uiLabel->GetTextHorizontalAlignment()),
              mTextColor(uiLabel->GetTextColor())
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
            if (const auto &canvasProxySp = mParentCanvasProxy.lock())
            {
                if (const auto &fontHandlerSp = canvasProxySp->GetFontHandler().lock())
                {
                    const auto &folderManager = FolderManager::GetInstance();
                    ShaderParams shaderParams("UiLabel Shader", folderManager->GetShadersPath() + "fontVS.glsl", folderManager->GetShadersPath() + "fontFS.glsl", "", "", "", "");
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
                }
                else
                {
                    LogInfo("UiLabelSceneProxy::Initialize => CRIT: FontHandler is null");
                }
            }
            else
            {
                LogInfo("UiLabelSceneProxy::Initialize => CRIT: CanvasProxy is null");
            }
        }

        void UiLabelSceneProxy::Render()
        {
            if (const auto &canvasProxySp = mParentCanvasProxy.lock())
            {
                if (const auto &fontHandlerSp = canvasProxySp->GetFontHandler().lock())
                {
                    const auto &renderDataSp = fontHandlerSp->GetFontBatcher(mFontName);
                    mUiLabelShader->ExecuteShader();
                    const auto textHeightScreenSpace = mTextFieldProxy->GetCreatedMeshTextHeight();
                    mUiLabelShader->SetPosition(glm::vec2(mNormalizedTranslation.x + mCenterOffset.x, 1.0f - (mNormalizedTranslation.y + mCenterOffset.y + textHeightScreenSpace)));
                    mFontTexture->BindTexture(0);
                    mUiLabelShader->SetFontAtlasSlot(0);
                    mUiLabelShader->SetOpacity(mOpacity * mOverlayOpacity);
                    mUiLabelShader->SetColor(mTextColor);
                    renderDataSp->GetTextMesh()->GetBuffer()->RenderVAO(mTextFieldProxy->GetVertexStart(), mTextFieldProxy->GetVerticesCount(), GL_TRIANGLES);
                    mUiLabelShader->StopShader();
                }
            }
        }

        void UiLabelSceneProxy::SetText(const std::string &text)
        {
            mText = text;

            if (const auto &canvasProxySp = mParentCanvasProxy.lock())
            {
                if (const auto &fontHandlerSp = canvasProxySp->GetFontHandler().lock())
                {
                    fontHandlerSp->TextChanged(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId(), mText);
                }
            }
        }

        void UiLabelSceneProxy::SetTextLineWidth(const float textLineWidth)
        {
            mTextLineWidth = textLineWidth;
            mTextFieldProxy->SetLineMaxWidth(mTextLineWidth);

            if (const auto &canvasProxySp = mParentCanvasProxy.lock())
            {
                if (const auto &fontHandlerSp = canvasProxySp->GetFontHandler().lock())
                {
                    fontHandlerSp->TextChanged(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId(), mText);
                }
            }
        }

        void UiLabelSceneProxy::SetFontSize(const float fontSize)
        {
            mFontSize = fontSize;
            mTextFieldProxy->SetFontSize(mFontSize);

            if (const auto &canvasProxySp = mParentCanvasProxy.lock())
            {
                if (const auto &fontHandlerSp = canvasProxySp->GetFontHandler().lock())
                {
                    fontHandlerSp->TextChanged(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId(), mText);
                }
            }
        }

        void UiLabelSceneProxy::SetOpacity(const float opacity)
        {
            mOpacity = opacity;
        }

        void UiLabelSceneProxy::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
        {
            mTextHorizontalAlignment = textHorizontalAlignment;
            mTextFieldProxy->SetTextHorizontalAlignment(textHorizontalAlignment);

            if (const auto &canvasProxySp = mParentCanvasProxy.lock())
            {
                if (const auto &fontHandlerSp = canvasProxySp->GetFontHandler().lock())
                {
                    fontHandlerSp->TextChanged(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId(), mText);
                }
            }
        }

        void UiLabelSceneProxy::SetTextColor(const glm::vec3 &textColor)
        {
            mTextColor = textColor;
        }

        void UiLabelSceneProxy::CleanUp()
        {
            ShaderPool::GetInstance()->TryToFreeMemory(mUiLabelShader);

            if (const auto &canvasProxySp = mParentCanvasProxy.lock())
            {
                if (const auto &fontHandlerSp = canvasProxySp->GetFontHandler().lock())
                {
                    fontHandlerSp->UnregisterText(mTextFieldProxy->GetFontName(), mTextFieldProxy->GetTextFieldId());
                }
            }
        }
    }
}