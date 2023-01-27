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
              mColor(glm::vec3(1.0f)),
              mOpacity(1.0f)
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

        void UiRectangle::SetColor(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(color, mColor))
            {
                mColor = color;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        void UiRectangle::SetColor(const uint8_t r, const uint8_t g, const uint8_t b)
        {
            static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
            glm::vec3 color = glm::vec3(static_cast<float>(r) * INV_COLOR_MAX_BYTE_VALUE,
                                        static_cast<float>(g) * INV_COLOR_MAX_BYTE_VALUE,
                                        static_cast<float>(b) * INV_COLOR_MAX_BYTE_VALUE);
            SetColor(color);
        }

        void UiRectangle::OnPropertiesShouldBeUpdatedOnRenderThread()
        {
            UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();

            SyncDataOnRenderThread();
        }

        void UiRectangle::SetColor(const uint32_t hexColor)
        {
            SetColor(EngineMath::FromHexColorToVec3Color(hexColor));
        }

        void UiRectangle::SetOpacity(const float opacity)
        {
            if (!EngineMath::FloatsNearEqual(opacity, mOpacity))
            {
                mOpacity = opacity;
            }
        }

        float UiRectangle::GetOpacity() const
        {
            return mOpacity;
        }

        glm::vec3 UiRectangle::GetColor() const
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
                    if (const auto &sceneRenderer = sceneSp->GetThreadManager().GetSceneRendererWP().lock())
                    {
                        const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(GetUId(), canvasSp->GetUId());
                        if (uiSceneProxy)
                        {
                            sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [=]()
                                                           {
                                const auto& rectangleSceneProxy = std::static_pointer_cast<UiRectangleSceneProxy>(uiSceneProxy);
                                rectangleSceneProxy->SetColor(mColor);
                                rectangleSceneProxy->SetOpacity(mOpacity); });
                        }
                        else
                        {
                            mIsPropertiesShouldBeUpdatedOnRenderThread = true;
                        }
                    }
                }
            }
        }
    }
}