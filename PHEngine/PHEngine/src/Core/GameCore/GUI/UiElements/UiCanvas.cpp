#include "UiCanvas.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/UtilityCore/EngineMath.h"

#include <algorithm>

using namespace EngineCore;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace EngineCore
{
    namespace GUI
    {
        size_t UiCanvas::s_UId = 0;

        UiCanvas::UiCanvas(const ViewPortInfo &canvasScreenProperties)
            : mUId(s_UId++),
              mAbsoluteOrigin(glm::ivec2(canvasScreenProperties.OriginX, canvasScreenProperties.OriginY)),
              mWidthHeight(glm::ivec2(canvasScreenProperties.Width, canvasScreenProperties.Height)),
              mChildren(),
              mRegisteredUiItems(),
              mIsVisible(true)
        {
        }

        void UiCanvas::SetScene(const std::weak_ptr<Scene> &sceneWp)
        {
            mScene = sceneWp;
        }

        size_t UiCanvas::GetUId() const
        {
            return mUId;
        }

        std::weak_ptr<Scene> UiCanvas::GetScene() const
        {
            return mScene;
        }

        const glm::ivec2 &UiCanvas::GetAbsoluteOrigin() const
        {
            return mAbsoluteOrigin;
        }

        size_t UiCanvas::GetZOrder() const
        {
            return 0;
        }

        size_t UiCanvas::GetWidth() const
        {
            return mWidthHeight.x;
        }

        size_t UiCanvas::GetHeight() const
        {
            return mWidthHeight.y;
        }

        glm::vec2 UiCanvas::GetNormalizedTranslation() const
        {
            return glm::vec2();
        }

        glm::vec2 UiCanvas::GetNormalizedScale() const
        {
            return glm::vec2(1.0);
        }

        std::shared_ptr<IUiTransformable> UiCanvas::GetRootParent() const
        {
            return std::shared_ptr<IUiTransformable>();
        }

        bool UiCanvas::IsVisible() const
        {
            return mIsVisible;
        }

        void UiCanvas::SetIsVisible(const bool isVisible)
        {
            if (mIsVisible != isVisible)
            {
                mIsVisible = isVisible;
                SyncDataOnRenderThread();
            }
        }

        void UiCanvas::SetAbsoluteOrigin(const glm::ivec2 &transform)
        {
            if (!EngineMath::CheckSimilarityIVec2(transform, mAbsoluteOrigin))
            {
                mAbsoluteOrigin = transform;
                UpdateHierarchyTransform();
                SyncDataOnRenderThread();
            }
        }

        void UiCanvas::SetZOrder(const size_t z_order)
        {
        }

        void UiCanvas::SetWidth(const size_t width)
        {
            if (mWidthHeight.x != width)
            {
                mWidthHeight.x = width;
                UpdateHierarchyTransform();
                SyncDataOnRenderThread();
            }
        }

        void UiCanvas::SetHeight(const size_t height)
        {
            if (mWidthHeight.y != height)
            {
                mWidthHeight.y = height;
                UpdateHierarchyTransform();
                SyncDataOnRenderThread();
            }
        }

        void UiCanvas::UpdateHierarchyTransform()
        {
            for (const auto &child : mChildren)
            {
                child->UpdateHierarchyTransform();
            }
        }

        void UiCanvas::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            RegisterUiItem(uiItem->GetUId());
            mChildren.emplace_back(uiItem);
            uiItem->OnRegistered();
        }

        void UiCanvas::RegisterUiItem(const size_t uiId)
        {
            assert(!mRegisteredUiItems.count(uiId));
            mRegisteredUiItems.insert(uiId);
        }

        void UiCanvas::UnregisterUiItem(const size_t uiId)
        {
            assert(mRegisteredUiItems.count(uiId));
            mRegisteredUiItems.erase(uiId);
        }

        void UiCanvas::RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            UnregisterUiItem(uiItem->GetUId());
            const auto foundIt = std::find_if(mChildren.begin(), mChildren.end(), [&](const auto& childItem) { return uiItem->GetUId() == childItem->GetUId(); });
            (*foundIt)->OnUnregistered();
            const auto it = std::remove_if(mChildren.begin(), mChildren.end(), [&](const auto &childUi)
                                           { return childUi->GetUId() == uiItem->GetUId(); });
            mChildren.erase(it);
        }

        std::shared_ptr<UiCanvasSceneProxy> UiCanvas::CreateUiCanvasSceneProxy() const
        {
            return std::make_shared<UiCanvasSceneProxy>(this);
        }

        void UiCanvas::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiCanvas::SyncDataOnRenderThread");
            if (const auto &sceneSp = mScene.lock())
            {
                if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
                {
                    sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, 0, functionId, [this, sceneRenderer]()
                    {
                        const auto& canvasProxy = sceneRenderer->GetCanvasSceneProxyByProxyId(GetUId());
                        canvasProxy->SetIsVisible(mIsVisible),
                        canvasProxy->SetAbsoluteOrigin(mAbsoluteOrigin),
                        canvasProxy->SetWidthHeight(mWidthHeight); 
                    });
                }
            }
        }
    }
}