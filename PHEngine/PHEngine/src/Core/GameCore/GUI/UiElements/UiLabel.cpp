#include "UiLabel.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiLabelSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"

#include <functional>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace EngineCore
{
    namespace GUI
    {
        UiLabel::UiLabel(const std::string &fontName)
            : UiItemBase(),
              mText(""),
              mOpacity(1.0f),
              mFontName(fontName),
              mTextLineWidth(1.0f),
              mFontSize(5.0f),
              mTextColor(glm::vec3())
        {
            assert(mFontName.size());
        }

        UiLabel::~UiLabel()
        {
        }

        void UiLabel::OnRegistered()
        {
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &parentCanvasSp = mParentCanvas.lock())
                {
                    const auto thisSceneProxy = CreateUiSceneProxy();
                    sceneSp->RegisterUiSceneProxy_OnRenderThread(thisSceneProxy, parentCanvasSp->GetUId());
                }
            }
        }

        void UiLabel::OnUnregistered()
        {
        }

        void UiLabel::OnPropertiesShouldBeUpdatedOnRenderThread()
        {
            UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
            mTextLineWidth = mNormalizedScale.x;
            SyncDataOnRenderThread();
        }

        void UiLabel::SetText(const std::string &text)
        {
            if (text != mText)
            {
                mText = text;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        std::string UiLabel::GetText() const
        {
            return mText;
        }

        void UiLabel::SetOpacity(const float opacity)
        {
            if (glm::abs(mOpacity - opacity) > EngineMath::ENGINE_FLOAT_EPSILON)
            {
                mOpacity = opacity;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        float UiLabel::GetOpacity() const
        {
            return mOpacity;
        }

        std::string UiLabel::GetFontName() const
        {
            return mFontName;
        }

        float UiLabel::GetTextLineWidth() const
        {
            return mTextLineWidth;
        }

        void UiLabel::SetFontSize(const float fontSize)
        {
            if (glm::abs(mFontSize - fontSize) > EngineMath::ENGINE_FLOAT_EPSILON)
            {
                mFontSize = fontSize;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        float UiLabel::GetFontSize() const
        {
            return mFontSize;
        }

        void UiLabel::SetTextColor(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(color, mTextColor))
            {
                mTextColor = color;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        void UiLabel::SetTextColor(const uint32_t hexColor)
        {
            SetTextColor(EngineMath::FromHexColorToVec3Color(hexColor));
        }

        glm::vec3 UiLabel::GetTextColor() const
        {
            return mTextColor;
        }

        void UiLabel::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
        {
            if (mTextHorizontalAlignment != textHorizontalAlignment)
            {
                mTextHorizontalAlignment = textHorizontalAlignment;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        eTextHorizontalAlignmentType UiLabel::GetTextHorizontalAlignment() const
        {
            return mTextHorizontalAlignment;
        }

        std::shared_ptr<UiSceneProxyBase> UiLabel::CreateUiSceneProxy() const
        {
            return std::make_shared<UiLabelSceneProxy>(this);
        }

        std::shared_ptr<LuaProxy> UiLabel::ReplicateLuaProxy()
        {
            return nullptr;
        }

        void UiLabel::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiLabel::SyncDataOnRenderThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &canvasSp = GetParentCanvas().lock())
                {
                    if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                    {
                        const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(GetUId(), canvasSp->GetUId());
                        if (uiSceneProxy)
                        {
                            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [=]() {
                                const auto& labelSceneProxy = std::static_pointer_cast<UiLabelSceneProxy>(uiSceneProxy);
                                labelSceneProxy->SetOpacity(mOpacity);
                                labelSceneProxy->SetText(mText);
                                labelSceneProxy->SetTextLineWidth(mTextLineWidth);
                                labelSceneProxy->SetFontSize(mFontSize);
                                labelSceneProxy->SetTextColor(mTextColor);
                                labelSceneProxy->SetTextHorizontalAlignment(mTextHorizontalAlignment); 
                            });
                        }
                        else
                        {
                            mIsPropertiesShouldBeUpdatedOnRenderThread = true;
                        }
                    }
                }
            }
        }
    }
}