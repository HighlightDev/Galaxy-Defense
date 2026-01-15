#include "UiOverlay.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiProxies/UiOverlayLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

using namespace EngineCore;

namespace EngineCore {
namespace GUI {
UiOverlay::UiOverlay(
    const std::string& overlayName,
    const std::weak_ptr<::EngineCore::Scene>& sceneWp,
    const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor>& luaScriptProcessor)
    : EngineToLuaReplicatorBase()
    , mOverlayName(overlayName)
    , mSceneWp(sceneWp)
    , mCanvas()
{
    EngineToLuaReplicatorBase::SetLuaScriptProcessor(luaScriptProcessor);
}

void UiOverlay::SetOverlayCanvas(const std::shared_ptr<UiCanvas>& canvas)
{
    ext_assert(!mCanvas, "UiOverlay::SetOverlayCanvas: mCanvas is already set for overlay: " + mOverlayName);
    mCanvas = canvas;
    mCanvas->CreateAnimator();
    mCanvas->GetAnimator()->SubscribeOnAnimationFinished([weak = weak_from_this()](const std::string& animationName) {
        if (const auto& uiOverlayPtr = weak.lock()) {
            const auto uiOverlayStrong = std::static_pointer_cast<UiOverlay>(uiOverlayPtr);
            if ("FadeOut" == animationName) {
                uiOverlayStrong->GetCanvas()->SetIsVisible(false);
            }
        }
    });
}

std::string UiOverlay::GetOverlayName() const
{
    return mOverlayName;
}

void UiOverlay::OpenOverlay()
{
    LogInfo("UiOverlay::OpenOverlay: overlay: ", mOverlayName);
    ext_assert(mCanvas, "UiOverlay::OpenOverlay: mCanvas is not set for overlay: " + mOverlayName);
    mCanvas->SetIsVisible(true);
    const auto& animator = mCanvas->GetAnimator();
    if (animator->HasAnimation("FadeIn")) {
        animator->StartAnimation("FadeIn");
    }
}

void UiOverlay::CloseOverlay()
{
    LogInfo("UiOverlay::OpenOverlay: overlay: ", mOverlayName);
    ext_assert(mCanvas, "UiOverlay::CloseOverlay: mCanvas is not set for overlay: " + mOverlayName);
    const auto& animator = mCanvas->GetAnimator();
    if (animator->HasAnimation("FadeOut")) {
        animator->StartAnimation("FadeOut");
    } else {
        mCanvas->SetIsVisible(false);
    }
}

std::shared_ptr<::EngineCore::Scripts::LuaProxy> UiOverlay::ReplicateLuaProxy()
{
    return std::make_shared<UiOverlayLuaProxy>(std::static_pointer_cast<UiOverlay>(shared_from_this()));
}

void UiOverlay::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
}

void UiOverlay::Initialize()
{
}

void UiOverlay::Tick(const float deltaTimeSec)
{
    if (mCanvas) {
        mCanvas->Tick(deltaTimeSec);
    }
}

void UiOverlay::UnpausableTick(const float deltaTimeSec)
{
    if (mCanvas) {
        mCanvas->UnpausableTick(deltaTimeSec);
    }
}

std::shared_ptr<UiCanvas> UiOverlay::GetCanvas() const
{
    return mCanvas;
}

void UiOverlay::SubscribeOnAnimationFinished(const std::function<void(std::string)>& callback)
{
    ext_assert(mCanvas, "UiOverlay::SubscribeOnAnimationFinished: mCanvas is not set for overlay: " + mOverlayName);
    const auto& animator = mCanvas->GetAnimator();
    animator->SubscribeOnAnimationFinished(callback);
}

bool UiOverlay::HasFadeInAnimation() const
{
    ext_assert(mCanvas, "UiOverlay::HasFadeInAnimation: mCanvas is not set for overlay: " + mOverlayName);
    const auto& animator = mCanvas->GetAnimator();
    return animator->HasAnimation("FadeIn");
}

bool UiOverlay::HasFadeOutAnimation() const
{
    ext_assert(mCanvas, "UiOverlay::HasFadeOutAnimation: mCanvas is not set for overlay: " + mOverlayName);
    const auto& animator = mCanvas->GetAnimator();
    return animator->HasAnimation("FadeOut");
}

void UiOverlay::CleanUp()
{
    mCanvas->CleanUp();
}
} // namespace GUI
} // namespace EngineCore