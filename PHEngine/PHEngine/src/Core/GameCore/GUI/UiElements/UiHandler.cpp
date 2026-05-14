#include "UiHandler.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/UiInputComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"

using namespace EngineCore;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace EngineCore {
namespace GUI {
UiHandler::UiHandler()
    : mOwner()
    , mUiCanvases()
#ifdef DEBUG
    , mDebugUiCanvas()
    , mInputComponent(std::make_unique<UiInputComponent>(std::make_shared<ComponentData>("UiHandlerDebugInputComponent")))
#endif
{
    LogInfo("UiHandler::ctor");

#ifdef DEBUG
    mInputComponent->SetIsReceivingMouseEvents(false);
#endif
}

UiHandler::~UiHandler() = default;

void UiHandler::SetScene(const std::weak_ptr<::EngineCore::Scene>& owner)
{
    mOwner = owner;
}

#ifdef DEBUG
std::shared_ptr<UiCanvas> UiHandler::CreateDebugCanvas(const ViewPortInfo& canvasScreenSize)
{
    if (!mDebugUiCanvas) {
        const auto& ownerSp = mOwner.lock();
        ext_assert(ownerSp, "UiHandler::CreateDebugCanvas: ownerSp is null");
        mDebugUiCanvas = std::make_shared<UiCanvas>(canvasScreenSize, "DebugCanvas");
        LogInfo("UiHandler::CreateDebugCanvas: uid = ", mDebugUiCanvas->GetUId());
        mDebugUiCanvas->Initialize();
        const auto& canvasSceneProxy = mDebugUiCanvas->CreateUiCanvasSceneProxy();
        if (const auto& sceneRendererSp = ownerSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneRendererSp->RegisterUiCanvasProxy_OnRenderThread(mDebugUiCanvas, canvasSceneProxy);
        }
        mDebugUiCanvas->SetScene(mOwner);
    }
    return mDebugUiCanvas;
}
#endif

std::shared_ptr<UiCanvas> UiHandler::CreateCanvas(const ViewPortInfo& canvasScreenSize, const std::string& name)
{
    const auto& ownerSp = mOwner.lock();
    ext_assert(ownerSp, "UiHandler::CreateCanvas: ownerSp is null");
    const auto& newCanvas = std::make_shared<UiCanvas>(canvasScreenSize, name);
    LogInfo("UiHandler::CreateCanvas: uid = ", newCanvas->GetUId());
    newCanvas->Initialize();
    const auto& canvasSceneProxy = newCanvas->CreateUiCanvasSceneProxy();
    if (const auto& sceneRendererSp = ownerSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
        sceneRendererSp->RegisterUiCanvasProxy_OnRenderThread(newCanvas, canvasSceneProxy);
    }
    mUiCanvases.emplace_back(newCanvas)->SetScene(mOwner);
    return newCanvas;
}

std::shared_ptr<UiCanvas> UiHandler::CreateHudCanvas(const ViewPortInfo& canvasScreenSize)
{
    if (!mHudCanvas) {
        const auto& ownerSp = mOwner.lock();
        ext_assert(ownerSp, "UiHandler::CreateHudCanvas: ownerSp is null");
        mHudCanvas = std::make_shared<UiCanvas>(canvasScreenSize, "HudCanvas");
        LogInfo("UiHandler::CreateHudCanvas: uid = ", mHudCanvas->GetUId());
        mHudCanvas->Initialize();
        const auto& canvasSceneProxy = mHudCanvas->CreateUiCanvasSceneProxy();
        if (const auto& sceneRendererSp = ownerSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneRendererSp->RegisterUiCanvasProxy_OnRenderThread(mHudCanvas, canvasSceneProxy);
        }
        mHudCanvas->SetScene(mOwner);
    }
    return mHudCanvas;
}

void UiHandler::Tick(const float deltaTimeSec)
{
    for (const auto& canvas : mUiCanvases) {
        canvas->Tick(deltaTimeSec);
    }

    if (mHudCanvas) {
        mHudCanvas->Tick(deltaTimeSec);
    }

#ifdef DEBUG
    if (mDebugUiCanvas) {
        mDebugUiCanvas->Tick(deltaTimeSec);
    }

    static float mPressButtonCooldown = 0.0f;

    const auto& keyboardBindings = mInputComponent->GetKeyboardBindings();
    static constexpr float buttonCooldown = 0.5f;

    if (keyboardBindings->HasPressedKeys()) {
        std::vector<std::shared_ptr<UiCanvas>> mCanvases;
        mCanvases.reserve(mUiCanvases.size() + 2);
        mCanvases.insert(mCanvases.end(), mUiCanvases.begin(), mUiCanvases.end());
        if (mHudCanvas) {
            mCanvases.push_back(mHudCanvas);
        }
        if (mDebugUiCanvas) {
            mCanvases.push_back(mDebugUiCanvas);
        }

        if (KeyState::PRESSED == keyboardBindings->GetStateByKey(eKeyboardKeys::Shift)
            && KeyState::PRESSED == keyboardBindings->GetStateByKey(eKeyboardKeys::P)) {
            for (const auto& canvas : mCanvases) {
                canvas->SetIsHiddenForDebugging(true);
            }
            mPressButtonCooldown = 0.0f;
        } else if (KeyState::PRESSED == keyboardBindings->GetStateByKey(eKeyboardKeys::P)) {
            if (mPressButtonCooldown >= buttonCooldown) {
                mPressButtonCooldown = 0.0f;
                for (const auto& canvas : mCanvases) {
                    canvas->SetIsHiddenForDebugging(false);
                }
            }
        }
    }

    mPressButtonCooldown += deltaTimeSec;
#endif
}

void UiHandler::UnpausableTick(const float deltaTimeSec)
{
    for (const auto& canvas : mUiCanvases) {
        canvas->UnpausableTick(deltaTimeSec);
    }

    if (mHudCanvas) {
        mHudCanvas->UnpausableTick(deltaTimeSec);
    }

#ifdef DEBUG
    if (mDebugUiCanvas) {
        mDebugUiCanvas->UnpausableTick(deltaTimeSec);
    }
#endif
}

std::shared_ptr<IUiTransformable> UiHandler::TryFindUiItemInAllCanvases(const std::string& name) const
{
    for (const auto& canvas : mUiCanvases) {
        if (const auto found = canvas->TryFindHierarchyChildByName(name)) {
            return found;
        }
    }
    if (mHudCanvas) {
        if (const auto found = mHudCanvas->TryFindHierarchyChildByName(name)) {
            return found;
        }
    }
    return nullptr;
}

std::shared_ptr<UiCanvas> UiHandler::GetCanvasByName(const std::string& canvasName) const
{
    const auto foundResultIt = std::find_if(
        mUiCanvases.cbegin(), mUiCanvases.cend(), [canvasName](const auto& canvas) { return canvasName == canvas->GetName(); });

    return foundResultIt != mUiCanvases.cend() ? *foundResultIt : nullptr;
}

std::shared_ptr<UiCanvas> UiHandler::GetCanvasByUId(const uint32_t canvasId) const
{
#ifdef DEBUG
    if (mDebugUiCanvas && canvasId == mDebugUiCanvas->GetUId()) {
        return mDebugUiCanvas;
    }
#endif

    if (mHudCanvas && canvasId == mHudCanvas->GetUId()) {
        return mHudCanvas;
    }

    const auto foundResultIt = std::find_if(
        mUiCanvases.cbegin(), mUiCanvases.cend(), [canvasId](const auto& canvas) { return canvasId == canvas->GetUId(); });

    return foundResultIt != mUiCanvases.cend() ? *foundResultIt : nullptr;
}

bool UiHandler::CheckIfUiInterceptsMouseEvent(const glm::ivec2& currentMousePosition) const
{
    for (const auto& canvas : mUiCanvases) {
        if (canvas->IsVisible() && canvas->GetIfCanInterceptMouseInputEvents()
            && canvas->CheckIfInterceptsMouseEvent(currentMousePosition)) {
            return true;
        }
    }

#ifdef DEBUG
    if (mDebugUiCanvas && mDebugUiCanvas->IsVisible() && mDebugUiCanvas->CheckIfInterceptsMouseEvent(currentMousePosition)) {
        return true;
    }
#endif
    return false;
}

void UiHandler::CleanUp()
{
    LogInfo("UiHandler::CleanUp: canvases count: ", mUiCanvases.size());

    for (const auto& canvas : mUiCanvases) {
        canvas->CleanUp();
    }

    mUiCanvases.clear();

    if (mHudCanvas) {
        mHudCanvas->CleanUp();
        mHudCanvas = nullptr;
    }
}

std::shared_ptr<UiCanvas> UiHandler::GetHudCanvas() const
{
    return mHudCanvas;
}

std::shared_ptr<UiItemBase> UiHandler::GetUiItemByUId(const size_t uiItemUId) const
{
    for (const auto& canvas : mUiCanvases) {
        if (const auto foundItem = canvas->TryFindHierarchyChildByUId(uiItemUId)) {
            return std::dynamic_pointer_cast<UiItemBase>(foundItem);
        }
    }

    if (mHudCanvas) {
        if (const auto foundItem = mHudCanvas->TryFindHierarchyChildByUId(uiItemUId)) {
            return std::dynamic_pointer_cast<UiItemBase>(foundItem);
        }
    }

#ifdef DEBUG
    if (mDebugUiCanvas) {
        if (const auto foundItem = mDebugUiCanvas->TryFindHierarchyChildByUId(uiItemUId)) {
            return std::dynamic_pointer_cast<UiItemBase>(foundItem);
        }
    }
#endif
    return nullptr;
}
} // namespace GUI
} // namespace EngineCore