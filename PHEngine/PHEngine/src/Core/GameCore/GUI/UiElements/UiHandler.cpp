#include "UiHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"

using namespace EngineCore;
using namespace Graphics::Proxy;

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
            ownerSp->RegisterUiCanvasProxy_OnRenderThread(canvasSceneProxy);
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
    }
}