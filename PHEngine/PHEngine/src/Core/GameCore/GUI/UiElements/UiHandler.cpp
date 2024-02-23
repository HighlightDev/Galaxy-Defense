#include "UiHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

using namespace EngineCore;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace EngineCore
{
    namespace GUI
    {
        UiHandler::UiHandler()
            : mOwner(),
              mUiCanvases()
#ifdef DEBUG
              ,
              mDebugUiCanvas()
#endif
        {
            LogInfo("UiHandler::ctor");
        }

        void UiHandler::SetScene(const std::weak_ptr<::EngineCore::Scene> &owner)
        {
            mOwner = owner;
        }

#ifdef DEBUG
        std::shared_ptr<UiCanvas> UiHandler::CreateDebugCanvas(const ViewPortInfo &canvasScreenSize)
        {
            if (!mDebugUiCanvas)
            {
                const auto &ownerSp = mOwner.lock();
                assert(ownerSp);
                mDebugUiCanvas = std::make_shared<UiCanvas>(canvasScreenSize, "DebugCanvas");
                LogInfo("UiHandler::CreateDebugCanvas => uid = ", mDebugUiCanvas->GetUId());
                mDebugUiCanvas->Initialize();
                const auto &canvasSceneProxy = mDebugUiCanvas->CreateUiCanvasSceneProxy();
                if (const auto &sceneRendererSp = ownerSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                {
                    sceneRendererSp->RegisterUiCanvasProxy_OnRenderThread(mDebugUiCanvas, canvasSceneProxy);
                    sceneRendererSp->SetDebugUiCanvasId(canvasSceneProxy->GetUiItemUId());
                }
                mDebugUiCanvas->SetScene(mOwner);
            }
            return mDebugUiCanvas;
        }
#endif

        std::shared_ptr<UiCanvas> UiHandler::CreateCanvas(const ViewPortInfo &canvasScreenSize, const std::string &name)
        {
            const auto &ownerSp = mOwner.lock();
            assert(ownerSp);
            const auto &newCanvas = std::make_shared<UiCanvas>(canvasScreenSize, name);
            LogInfo("UiHandler::CreateCanvas => uid = ", newCanvas->GetUId());
            newCanvas->Initialize();
            const auto &canvasSceneProxy = newCanvas->CreateUiCanvasSceneProxy();
            if (const auto &sceneRendererSp = ownerSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
                sceneRendererSp->RegisterUiCanvasProxy_OnRenderThread(newCanvas, canvasSceneProxy);
            }
            mUiCanvases.emplace_back(newCanvas)->SetScene(mOwner);
            return newCanvas;
        }

        void UiHandler::Tick(const float deltaTime)
        {
            for (const auto &canvas : mUiCanvases)
            {
                canvas->Tick(deltaTime);
            }
#ifdef DEBUG
            if (mDebugUiCanvas)
            {
                mDebugUiCanvas->Tick(deltaTime);
            }
#endif
        }

        void UiHandler::UnpausableTick(const float deltaTime)
        {
            for (const auto &canvas : mUiCanvases)
            {
                canvas->UnpausableTick(deltaTime);
            }
#ifdef DEBUG
            if (mDebugUiCanvas)
            {
                mDebugUiCanvas->UnpausableTick(deltaTime);
            }
#endif
        }

        std::shared_ptr<UiCanvas> UiHandler::GetCanvasByName(const std::string &canvasName) const
        {
            const auto foundResultIt = std::find_if(mUiCanvases.cbegin(), mUiCanvases.cend(), [canvasName](const auto &canvas)
                                                    { return canvasName == canvas->GetName(); });

            return foundResultIt != mUiCanvases.cend() ? *foundResultIt : nullptr;
        }

        std::shared_ptr<UiCanvas> UiHandler::GetCanvasByUId(const uint32_t canvasId) const
        {
            const auto foundResultIt = std::find_if(mUiCanvases.cbegin(), mUiCanvases.cend(), [canvasId](const auto &canvas)
                                                    { return canvasId == canvas->GetUId(); });

            return foundResultIt != mUiCanvases.cend() ? *foundResultIt : nullptr;
        }

        bool UiHandler::CheckIfUiInterceptsMouseEvent(const glm::ivec2 &currentMousePosition) const
        {
            for (const auto &canvas : mUiCanvases)
            {
                if (canvas->IsVisible() &&
                    canvas->GetIfCanInterceptMouseInputEvents() &&
                    canvas->CheckIfInterceptsMouseEvent(currentMousePosition))
                {
                    return true;
                }
            }

#ifdef DEBUG
            if (mDebugUiCanvas && mDebugUiCanvas->IsVisible() && mDebugUiCanvas->CheckIfInterceptsMouseEvent(currentMousePosition))
            {
                return true;
            }
#endif
            return false;
        }

        void UiHandler::CleanUp()
        {
            LogInfo("UiHandler::CleanUp => canvases count: ", mUiCanvases.size());

            for (const auto &canvas : mUiCanvases)
            {
                canvas->CleanUp();
            }

            mUiCanvases.clear();
        }
    }
}