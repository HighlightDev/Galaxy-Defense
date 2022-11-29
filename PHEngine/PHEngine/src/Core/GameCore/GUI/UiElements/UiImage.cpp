#include "UiImage.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiImageSceneProxy.h"

using namespace EngineCore;
using namespace Graphics::Proxy;

namespace EngineCore
{
    namespace GUI
    {
        UiImage::UiImage(const std::weak_ptr<UiCanvas>& canvasParent, const std::weak_ptr<IUiTransformable> &parent)
            : UiItemBase(canvasParent, parent),
              mColor(glm::vec4(0.0, 0.0, 0.0, 1.0))
        {
        }

        void UiImage::OnRegistered()
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

        void UiImage::OnDeregistered()
        {
        }

        void UiImage::UpdateHierarchyTransform()
        {
            UiItemBase::UpdateHierarchyTransform();
        }

        void UiImage::SetColor(const glm::vec4 &color)
        {
            mColor = color;
        }

        glm::vec4 UiImage::GetColor() const
        {
            return mColor;
        }

        std::shared_ptr<UiSceneProxyBase> UiImage::CreateUiSceneProxy() const
        {
            return std::make_shared<UiImageSceneProxy>(this);
        }
    }
}