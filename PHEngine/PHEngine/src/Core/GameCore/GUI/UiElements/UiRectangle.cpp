#include "UiRectangle.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiRectangleSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiRectangleLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;

namespace EngineCore
{
    namespace GUI
    {
        UiRectangle::UiRectangle()
            : UiItemBase(),
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
                if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                {
                    if (const auto &parentCanvasSp = mParentCanvas.lock())
                    {
                        const auto thisSceneProxy = CreateUiSceneProxy();
                        sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(thisSceneProxy, parentCanvasSp->GetUId());
                    }
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
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
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

        void UiRectangle::OnPropertiesShouldBeUpdatedOnLuaThread()
        {
            UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();

            SyncDataOnLuaThread();
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
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
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

        std::shared_ptr<LuaProxy> UiRectangle::ReplicateLuaProxy()
        {
            return std::make_shared<UiRectangleLuaProxy>(std::static_pointer_cast<UiRectangle>(shared_from_this()));
        }

        void UiRectangle::SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr)
        {
            UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

            const auto &jsonObj = nlohmann::json::parse(luaJsonPropsStr);
            if (jsonObj.contains("color"))
            {
                const auto colorProps = jsonObj["color"];
                glm::vec3 color;
                for (auto it = colorProps.cbegin(); it != colorProps.cend(); ++it)
                {
                    const auto key = it.key();
                    if ("r" == key)
                    {
                        color.r = it->get<float>();
                    }
                    else if ("g" == key)
                    {
                        color.g = it->get<float>();
                    }
                    else if ("b" == key)
                    {
                        color.b = it->get<float>();
                    }
                    else
                    {
                        assert(false);
                    }
                }
                if (!EngineMath::CheckSimilarityVec3(color, mColor))
                {
                    mColor = color;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("opacity"))
            {
                const auto opacity = jsonObj["opacity"].get<float>();
            }
        }

        void UiRectangle::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiRectangle::SyncDataOnRenderThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &canvasSp = GetParentCanvas().lock())
                {
                    if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                    {
                        const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(GetUId(), canvasSp->GetUId());
                        if (uiSceneProxy)
                        {
                            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [=]()
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

        void UiRectangle::SyncDataOnLuaThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiRectangle::SyncDataOnLuaThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &luaScriptProcessorSp = GetLuaScriptProcessorWp().lock())
                {
                    if (const auto &rectangleLuaProxy = std::static_pointer_cast<UiRectangleLuaProxy>(luaScriptProcessorSp->GetLuaProxy(GetLuaProxyId())))
                    {
                        SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [rectangleLuaProxy, opacity = mOpacity, color = mColor]()
                                                                                         {
                            rectangleLuaProxy->SetOpacity_FromGameThread(opacity);
                            rectangleLuaProxy->SetColor_FromGameThread(color); });
                    }
                }
            }
        }
    }
}