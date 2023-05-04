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
        {
            LogInfo("UiHandler::ctor");
        }

        void UiHandler::SetScene(const std::weak_ptr<::EngineCore::Scene> &owner)
        {
            mOwner = owner;
        }

        std::shared_ptr<UiCanvas> UiHandler::CreateCanvas(const ViewPortInfo &canvasScreenSize)
        {
            const auto &ownerSp = mOwner.lock();
            assert(ownerSp);
            const auto &newCanvas = std::make_shared<UiCanvas>(canvasScreenSize);
            LogInfo("UiHandler::CreateCanvas => uid = ", newCanvas->GetUId());
            const auto &canvasSceneProxy = newCanvas->CreateUiCanvasSceneProxy();
            if (const auto &sceneRendererSp = ownerSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
                sceneRendererSp->RegisterUiCanvasProxy_OnRenderThread(canvasSceneProxy);
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
        }

        void UiHandler::UnpausableTick(const float deltaTime)
        {
            for (const auto &canvas : mUiCanvases)
            {
                canvas->UnpausableTick(deltaTime);
            }
        }

        std::shared_ptr<UiCanvas> UiHandler::GetCanvasByName(const std::string& canvasName) const
        {
            const auto foundResultIt = std::find_if(mUiCanvases.cbegin(), mUiCanvases.cend(), [canvasName](const auto& canvas) {
                return canvasName == canvas->GetName();
            });

            return foundResultIt != mUiCanvases.cend() ? *foundResultIt : nullptr;
        }

        std::shared_ptr<UiCanvas> UiHandler::GetCanvasByUId(const uint32_t canvasId) const
        {
            const auto foundResultIt = std::find_if(mUiCanvases.cbegin(), mUiCanvases.cend(), [canvasId](const auto& canvas) {
                return canvasId == canvas->GetUId();
            });

            return foundResultIt != mUiCanvases.cend() ? *foundResultIt : nullptr;
        }
    }
}