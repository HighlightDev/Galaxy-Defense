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
            // todo: for now
            auto animator = mCanvas->CreateAndGetAnimator();
            animator->AddAnimation("FadeIn", AnimationData(eAnimationInterpolationFunctionType::LINEAR, 0.5f, "Opacity", 0.0f, 1.0f));
            animator->AddAnimation("FadeOut", AnimationData(eAnimationInterpolationFunctionType::LINEAR, 0.5f, "Opacity", 1.0f, 0.0f));
            animator->SubscribeOnAnimationFinished([this](const std::string& animationName) {
                if ("FadeOut" == animationName)
                {
                    mCanvas->SetIsVisible(false);
                }
            });
        }

        std::string UiOverlay::GetOverlayName() const
        {
            return mOverlayName;
        }

        void UiOverlay::OpenOverlay()
        {
            assert(mCanvas);
            mCanvas->SetIsVisible(true);
            mCanvas->CreateAndGetAnimator()->StartAnimation("FadeIn");
        }

        void UiOverlay::CloseOverlay()
        {
            assert(mCanvas);
            mCanvas->CreateAndGetAnimator()->StartAnimation("FadeOut");
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

        std::shared_ptr<UiCanvas> UiOverlay::GetCanvas() const
        {
            return mCanvas;
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
    }
}