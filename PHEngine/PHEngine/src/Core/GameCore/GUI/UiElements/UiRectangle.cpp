#include "UiRectangle.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiRectangleSceneProxy.h"
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
        UiRectangle::UiRectangle(const std::weak_ptr<UiCanvas> &canvasParent, const std::weak_ptr<IUiTransformable> &parent)
            : UiItemBase(canvasParent, parent),
              mColor(glm::vec4(1))
        {
        }

        UiRectangle::~UiRectangle()
        {
        }

        void UiRectangle::OnRegistered()
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

        void UiRectangle::OnUnregistered()
        {
        }

        void UiRectangle::SetColor(const glm::vec4 &color)
        {
            if (!EngineMath::CheckSimilarityIVec4(color, mColor))
            {
                mColor = color;
                SyncDataOnRenderThread();
            }
        }

        glm::vec4 UiRectangle::GetColor() const
        {
            return mColor;
        }

        std::shared_ptr<UiSceneProxyBase> UiRectangle::CreateUiSceneProxy() const
        {
            return std::make_shared<UiRectangleSceneProxy>(this);
        }

        void UiRectangle::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiRectangle::SyncDataOnRenderThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &canvasSp = GetParentCanvas().lock())
                {
                    if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
                    {
                        sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [=]()
                                                       {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(GetUId(), canvasSp->GetUId());
                            if (uiSceneProxy)
                            {
                                const auto& rectangleSceneProxy = std::static_pointer_cast<UiRectangleSceneProxy>(uiSceneProxy);
                                rectangleSceneProxy->SetColor(mColor);
                            } });
                    }
                }
            }
        }
    }
}