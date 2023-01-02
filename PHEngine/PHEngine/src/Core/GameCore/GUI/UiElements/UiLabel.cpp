#include "UiLabel.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiLabelSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"

#include <functional>

using namespace EngineCore;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace EngineCore
{
    namespace GUI
    {
        UiLabel::UiLabel(const std::weak_ptr<UiCanvas> &canvasParent, const std::weak_ptr<IUiTransformable> &parent, const std::string& fontName)
            : UiItemBase(canvasParent, parent),
              mText(""),
              mOpacity(1.0f),
              mFontName(fontName)
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
                    const auto &thisSceneProxy = CreateUiSceneProxy();
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
            SyncDataOnRenderThread();
        }

        void UiLabel::SetText(const std::string &text)
        {
            if (text != mText)
            {
                mText = text;
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

        std::shared_ptr<UiSceneProxyBase> UiLabel::CreateUiSceneProxy() const
        {
            return std::make_shared<UiLabelSceneProxy>(this);
        }

        void UiLabel::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiLabel::SyncDataOnRenderThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &canvasSp = GetParentCanvas().lock())
                {
                    if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
                    {
                        sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [=]()
                                                       {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(GetUId(), canvasSp->GetUId());
                            if (uiSceneProxy)
                            {
                                const auto& labelSceneProxy = std::static_pointer_cast<UiLabelSceneProxy>(uiSceneProxy);
                                labelSceneProxy->SetOpacity(mOpacity);
                                labelSceneProxy->SetText(mText);
                            } });
                    }
                }
            }
        }
    }
}