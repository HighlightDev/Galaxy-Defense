#include "UiToggleButton.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiToggleButtonSceneProxy.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiToggleButtonLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;

namespace EngineCore
{
    namespace GUI
    {
        UiToggleButton::UiToggleButton(const bool isInitialStateOn, const std::string& name)
            : UiItemBase(name),
              mToggleOffColor(),
              mToggleOnColor(),
              mOpacity(1.0f),
              mIsStateOn(isInitialStateOn)
        {
        }

        UiToggleButton::~UiToggleButton()
        {
        }

        void UiToggleButton::OnRegistered()
        {
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                {
                    if (const auto &parentCanvasSp = mParentCanvas.lock())
                    {
                        sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(std::static_pointer_cast<UiToggleButton>(shared_from_this()), CreateUiSceneProxy(), parentCanvasSp->GetUId());
                    }
                }
            }
        }

        void UiToggleButton::OnUnregistered()
        {
        }

        bool UiToggleButton::IsButtonStateOn() const
        {
            return mIsStateOn;
        }

        void UiToggleButton::ToggleButton()
        {
            mIsStateOn = !mIsStateOn;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
            SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        }

        void UiToggleButton::SetToggleOnColor(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(color, mToggleOnColor))
            {
                mToggleOnColor = color;
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
            }
        }

        void UiToggleButton::SetToggleOffColor(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(color, mToggleOffColor))
            {
                mToggleOffColor = color;
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
            }
        }

        void UiToggleButton::SetToggleOnColor(const uint32_t hexColor)
        {
            SetToggleOnColor(EngineMath::FromHexColorToVec3Color(hexColor));
        }

        void UiToggleButton::SetToggleOffColor(const uint32_t hexColor)
        {
            SetToggleOffColor(EngineMath::FromHexColorToVec3Color(hexColor));
        }

        glm::vec3 UiToggleButton::GetToggleOnColor() const
        {
            return mToggleOnColor;
        }

        float UiToggleButton::GetOpacity() const
        {
            return mOpacity;
        }

        glm::vec3 UiToggleButton::GetToggleOffColor() const
        {
            return mToggleOffColor;
        }

        void UiToggleButton::SetOpacity(const float opacity)
        {
            if (!EngineMath::FloatsNearEqual(mOpacity, opacity))
            {
                mOpacity = opacity;
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
            }
        }

        std::string UiToggleButton::GetUiTypeString() const
        {
            return "UiToggleButton";
        }

        void UiToggleButton::OnPropertiesShouldBeUpdatedOnRenderThread()
        {
            UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();

            SyncDataOnRenderThread();
        }

        void UiToggleButton::OnPropertiesShouldBeUpdatedOnLuaThread()
        {
            UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();

            SyncDataOnLuaThread();
        }

        void UiToggleButton::SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr)
        {
            UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

            const auto &jsonObj = nlohmann::json::parse(luaJsonPropsStr);
            if (jsonObj.contains("toggle_on_color"))
            {
                const glm::vec3 toggleOnColor = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["toggle_on_color"]);
                if (!EngineMath::CheckSimilarityVec3(toggleOnColor, mToggleOnColor))
                {
                    mToggleOnColor = toggleOnColor;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("toggle_off_color"))
            {
             const glm::vec3 toggleOffColor = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["toggle_off_color"]);
                if (!EngineMath::CheckSimilarityVec3(toggleOffColor, mToggleOffColor))
                {
                    mToggleOffColor = toggleOffColor;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("opacity"))
            {
                const auto opacity = jsonObj["opacity"].get<float>();
                if (!EngineMath::FloatsNearEqual(mOpacity, opacity))
                {
                    mOpacity = opacity;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("is_state_on"))
            {
                const auto is_state_on = jsonObj["is_state_on"].get<bool>();
                if (!EngineMath::FloatsNearEqual(mIsStateOn, is_state_on))
                {
                    mIsStateOn = is_state_on;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
        }

        std::shared_ptr<UiSceneProxyBase> UiToggleButton::CreateUiSceneProxy() const
        {
            return std::make_shared<UiToggleButtonSceneProxy>(this);
        }

        std::shared_ptr<LuaProxy> UiToggleButton::ReplicateLuaProxy()
        {
            return std::make_shared<UiToggleButtonLuaProxy>(std::static_pointer_cast<UiToggleButton>(shared_from_this()));
        }

        void UiToggleButton::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiToggleButton::SyncDataOnRenderThread");
            if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &canvasSp = GetParentCanvas().lock())
                    {
                        if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                        {

                            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [sceneRenderer, myUId = GetUId(), canvasUId = canvasSp->GetUId(), toggleOnColor = mToggleOnColor, toggleOffColor = mToggleOffColor, isStateOn = mIsStateOn, opacity = mOpacity]()
                                                                                                {
                                const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                                if (uiSceneProxy)
                                {
                                    const auto& toggleButtonSceneProxy = std::static_pointer_cast<UiToggleButtonSceneProxy>(uiSceneProxy);
                                    toggleButtonSceneProxy->SetToggleOnColor(toggleOnColor);
                                    toggleButtonSceneProxy->SetToggleOffColor(toggleOffColor);
                                    toggleButtonSceneProxy->SetOpacity(opacity);
                                    toggleButtonSceneProxy->SetState(isStateOn); 
                                } });
                        }
                    }
                }
            }
            else
            {
                mIsPropertiesShouldBeUpdatedOnRenderThread = true;
            }
        }

        void UiToggleButton::SyncDataOnLuaThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiToggleButton::SyncDataOnLuaThread");
            if (mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &luaScriptProcessorSp = GetLuaScriptProcessorWp().lock())
                    {
                        SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [luaScriptProcessorSp, luaProxyId = GetLuaProxyId(), opacity = mOpacity, toggleOnColor = mToggleOnColor, toggleOffColor = mToggleOffColor, isStateOn = mIsStateOn]() {
                            if (const auto &toggleButtonLuaProxy = std::static_pointer_cast<UiToggleButtonLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                            {
                                toggleButtonLuaProxy->SetOpacity_FromGameThread(opacity);
                                toggleButtonLuaProxy->SetToggleOnColor_FromGameThread(toggleOnColor);
                                toggleButtonLuaProxy->SetToggleOffColor_FromGameThread(toggleOffColor);
                                toggleButtonLuaProxy->SetIsStateOn_FromGameThread(isStateOn); 
                            } 
                        });
                    }
                }
            }
        }
    }
}