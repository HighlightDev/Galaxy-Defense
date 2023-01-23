#include "UiToggleButton.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiToggleButtonSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineCore;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;

namespace EngineCore
{
    namespace GUI
    {
        UiToggleButton::UiToggleButton(const bool isInitialStateOn,
                                       const std::weak_ptr<UiCanvas> &canvasParent,
                                       const std::weak_ptr<IUiTransformable> &parent)
            : UiItemBase(canvasParent, parent),
              mToggleOffColor(),
              mToggleOnColor(),
              mOpacity(1.0f),
              mIsStateOn(isInitialStateOn)
        {
        }

        UiToggleButton::~UiToggleButton()
        {
        }

        void UiToggleButton::OnRegistered()
        {
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &parentCanvasSp = mParentCanvas.lock())
                {
                    sceneSp->RegisterUiSceneProxy_OnRenderThread(CreateUiSceneProxy(), parentCanvasSp->GetUId());
                }
            }
        }

        void UiToggleButton::OnUnregistered()
        {
        }

        bool UiToggleButton::IsButtonStateOn() const
        {
            return mIsStateOn;
        }

        void UiToggleButton::ToggleButton()
        {
            mIsStateOn = !mIsStateOn;
            SetIsPropertiesShouldBeUpdated(true);
        }

        void UiToggleButton::SetToggleOnColor(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(color, mToggleOnColor))
            {
                mToggleOnColor = color;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        void UiToggleButton::SetToggleOffColor(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(color, mToggleOffColor))
            {
                mToggleOffColor = color;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        void UiToggleButton::SetToggleOnColor(const uint32_t hexColor)
        {
            SetToggleOnColor(EngineMath::FromHexColorToVec3Color(hexColor));
        }

        void UiToggleButton::SetToggleOffColor(const uint32_t hexColor)
        {
            SetToggleOffColor(EngineMath::FromHexColorToVec3Color(hexColor));
        }

        glm::vec3 UiToggleButton::GetToggleOnColor() const
        {
            return mToggleOnColor;
        }

        float UiToggleButton::GetOpacity() const
        {
            return mOpacity;
        }

        glm::vec3 UiToggleButton::GetToggleOffColor() const
        {
            return mToggleOffColor;
        }

        void UiToggleButton::SetOpacity(const float opacity)
        {
            if (!EngineMath::FloatsNearEqual(mOpacity, opacity))
            {
                mOpacity = opacity;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        void UiToggleButton::OnPropertiesShouldBeUpdatedOnRenderThread()
        {
            UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();

            SyncDataOnRenderThread();
        }

        std::shared_ptr<UiSceneProxyBase> UiToggleButton::CreateUiSceneProxy() const
        {
            return std::make_shared<UiToggleButtonSceneProxy>(this);
        }

        void UiToggleButton::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiToggleButton::SyncDataOnRenderThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &canvasSp = GetParentCanvas().lock())
                {
                    if (const auto &sceneRenderer = sceneSp->GetThreadManager().GetSceneRendererWP().lock())
                    {
                        sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [=]()
                                                       {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(GetUId(), canvasSp->GetUId());
                            if (uiSceneProxy)
                            {
                                const auto& toggleButtonSceneProxy = std::static_pointer_cast<UiToggleButtonSceneProxy>(uiSceneProxy);
                                toggleButtonSceneProxy->SetToggleOnColor(mToggleOnColor);
                                toggleButtonSceneProxy->SetToggleOffColor(mToggleOffColor);
                                toggleButtonSceneProxy->SetOpacity(mOpacity);
                                toggleButtonSceneProxy->SetState(mIsStateOn);
                            } });
                    }
                }
            }
        }
    }
}