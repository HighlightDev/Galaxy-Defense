#include "UiOverlay.h"

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiOverlayLuaProxy.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"

using namespace EngineCore;

namespace EngineCore
{
    namespace GUI
    {
        UiOverlay::UiOverlay(const std::string &overlayName,
                             const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                             const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> &luaScriptProcessor)
            : EngineToLuaReplicatorBase(),
              mOverlayName(overlayName),
              mSceneWp(sceneWp),
              mCanvas()
        {
            EngineToLuaReplicatorBase::SetLuaScriptProcessor(luaScriptProcessor);
        }

        void UiOverlay::SetOverlayCanvas(const std::shared_ptr<UiCanvas> &canvas)
        {
            assert(!mCanvas);
            mCanvas = canvas;
            mCanvas->CreateAnimator();
            mCanvas->GetAnimator()->SubscribeOnAnimationFinished([this](const std::string &animationName)
                                                                 { 
                if ("FadeOut" == animationName)
                {
                    mCanvas->SetIsVisible(false);
                } });
        }

        std::string UiOverlay::GetOverlayName() const
        {
            return mOverlayName;
        }

        void UiOverlay::OpenOverlay()
        {
            assert(mCanvas);
            mCanvas->SetIsVisible(true);
            const auto &animator = mCanvas->GetAnimator();
            if (animator->HasAnimation("FadeIn"))
            {
                animator->StartAnimation("FadeIn");
            }
        }

        void UiOverlay::CloseOverlay()
        {
            assert(mCanvas);
            const auto &animator = mCanvas->GetAnimator();
            if (animator->HasAnimation("FadeOut"))
            {
                animator->StartAnimation("FadeOut");
            }
            else
            {
                mCanvas->SetIsVisible(false);
            }
        }

        std::shared_ptr<::EngineCore::Scripts::LuaProxy> UiOverlay::ReplicateLuaProxy()
        {
            return std::make_shared<UiOverlayLuaProxy>(std::static_pointer_cast<UiOverlay>(shared_from_this()));
        }

        void UiOverlay::SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr)
        {
        }

        void UiOverlay::Initialize()
        {
        }

        void UiOverlay::Tick(const float deltaTime)
        {
            if (mCanvas)
            {
                mCanvas->Tick(deltaTime);
            }
        }

        void UiOverlay::UnpausableTick(const float deltaTime)
        {
            if (mCanvas)
            {
                mCanvas->UnpausableTick(deltaTime);
            }
        }

        std::shared_ptr<UiCanvas> UiOverlay::GetCanvas() const
        {
            return mCanvas;
        }

        void UiOverlay::SubscribeOnAnimationFinished(const std::function<void(std::string)> &callback)
        {
            assert(mCanvas);
            const auto &animator = mCanvas->GetAnimator();
            animator->SubscribeOnAnimationFinished(callback);
        }

        bool UiOverlay::HasFadeInAnimation() const
        {
            assert(mCanvas);
            const auto &animator = mCanvas->GetAnimator();
            return animator->HasAnimation("FadeIn");
        }

        bool UiOverlay::HasFadeOutAnimation() const
        {
            assert(mCanvas);
            const auto &animator = mCanvas->GetAnimator();
            return animator->HasAnimation("FadeOut");
        }
    }
}