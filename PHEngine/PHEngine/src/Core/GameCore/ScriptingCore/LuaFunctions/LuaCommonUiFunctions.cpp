#include "LuaCommonUiFunctions.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/EnginePropertyType.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationInterpolationFunctionType.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiCanvasLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiItemBaseLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiToggleButtonLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GameCore/ScriptingCore/ReplicatorFactories/CommonUiWidgetFactoryCreator.h"
#include "Core/GameCore/ScriptingCore/ReplicatorFactories/CommonUiWidgetType.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace IO;

namespace EngineCore {
namespace Scripts {
LuaCommonUiFunctions::LuaCommonUiFunctions(LuaScriptExecutorBase* ownerPtr)
    : mOwnerPtr(ownerPtr)
{
}

void LuaCommonUiFunctions::Initialize()
{
}

void LuaCommonUiFunctions::SetScene(const std::weak_ptr<Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

void LuaCommonUiFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
{
    mLuaScriptProcessor = scriptProcessor;
}

void LuaCommonUiFunctions::OnScriptStarted(const LuaWrapper& luaWrapper)
{
}

void LuaCommonUiFunctions::OnScriptStopped(const LuaWrapper& luaWrapper)
{
}

void LuaCommonUiFunctions::RegisterCallbacks(const LuaWrapper& luaWrapper)
{
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::GetCurrentOverlayName"), std::string(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::GetCurrentOverlayName, this, std::placeholders::_1),
        "_GetCurrentOverlayName");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::OpenOverlay"), void(std::string)>::Bind(
        luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::OpenOverlay, this, std::placeholders::_1), "_OpenOverlay");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::OpenBackgroundOverlay"), void(std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::OpenBackgroundOverlay, this, std::placeholders::_1),
        "_OpenBackgroundOverlay");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::CloseCurrentOverlay"), void()>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::CloseCurrentOverlay, this, std::placeholders::_1),
        "_CloseCurrentOverlay");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::CloseOverlayAndClearHistory"), void()>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::CloseOverlayAndClearHistory, this, std::placeholders::_1),
        "_CloseOverlayAndClearHistory");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::CloseBackgroundOverlay"), void(std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::CloseBackgroundOverlay, this, std::placeholders::_1),
        "_CloseBackgroundOverlay");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::CreateCommonUiWidget"), int32_t(int32_t, std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::CreateCommonUiWidget, this, std::placeholders::_1),
        "_CreateCommonUiWidget");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::DestroyCommonUiWidget"), void(int32_t)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::DestroyCommonUiWidget, this, std::placeholders::_1),
        "_DestroyCommonUiWidget");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::IsLuaProxyReady"), bool(int32_t)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::IsLuaProxyReady, this, std::placeholders::_1),
        "_IsLuaProxyReady");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::OnCommonUiWidgetDataUpdated"), void(int32_t, std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::OnCommonUiWidgetDataUpdated, this, std::placeholders::_1),
        "_OnCommonUiWidgetDataUpdated");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::GetGameThreadData"), std::string(int32_t)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::GetGameThreadData, this, std::placeholders::_1),
        "_GetGameThreadData");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::InitializeCanvasInputSystem"), void(int32_t)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::InitializeCanvasInputSystem, this, std::placeholders::_1),
        "_InitializeCanvasInputSystem");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::AddCanvasFadeAnimation"), void(int32_t, int32_t, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaCommonUiFunctions::AddCanvasFadeAnimation, this, std::placeholders::_1),
            "_AddCanvasFadeAnimation");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::AddUiItemAnimation"), void(int32_t, std::string, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaCommonUiFunctions::AddUiItemAnimation, this, std::placeholders::_1),
            "_AddUiItemAnimation");
    LuaCallbackBindingHelper<
        Hash64_CT("LuaCommonUiFunctions::AddUiItemSequenceAnimation"),
        void(int32_t, std::string, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaCommonUiFunctions::AddUiItemSequenceAnimation, this, std::placeholders::_1),
            "_AddUiItemSequenceAnimation");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::StartUiItemAnimation"), void(int32_t, std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::StartUiItemAnimation, this, std::placeholders::_1),
        "_StartUiItemAnimation");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::StartUiItemSequenceAnimation"), void(int32_t, std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::StartUiItemSequenceAnimation, this, std::placeholders::_1),
        "_StartUiItemSequenceAnimation");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::GetUiWidgetName"), std::string(int32_t)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::GetUiWidgetName, this, std::placeholders::_1),
        "_GetUiWidgetName");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::SetUiWidgetParent"), void(int32_t, std::string, std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::SetUiWidgetParent, this, std::placeholders::_1),
        "_SetUiWidgetParent");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::EnableToggleButtonMouseInputReceiver"), void(int32_t)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::EnableToggleButtonMouseInputReceiver, this, std::placeholders::_1),
        "_EnableToggleButtonMouseInputReceiver");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::EnableMouseInputReceiverBase"), void(int32_t)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::EnableMouseInputReceiverBase, this, std::placeholders::_1),
        "_EnableMouseInputReceiverBase");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::GetMouseInputData"), std::string(int32_t)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonUiFunctions::GetMouseInputData, this, std::placeholders::_1),
        "_GetMouseInputData");
}

std::string LuaCommonUiFunctions::GetCurrentOverlayName(const std::tuple<>& data)
{
    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        const auto overlayName = luaProcessorSp->GetOverlayManagerLuaProxy()->GetCurrentOverlayName();
        LogInfo("LuaCommonUiFunctions::GetCurrentOverlayName => overlayName:", overlayName);
        return overlayName;
    }
    LogInfo("LuaCommonUiFunctions::GetCurrentOverlayName => FAILED!");

    return "";
}

void LuaCommonUiFunctions::OpenOverlay(const std::tuple<std::string>& overlayName)
{
    const auto& overlayToOpen = std::get<0>(overlayName);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        luaProcessorSp->GetOverlayManagerLuaProxy()->OpenOverlay(overlayToOpen);
    }
}

void LuaCommonUiFunctions::OpenBackgroundOverlay(const std::tuple<std::string>& overlayName)
{
    const auto& overlayToOpen = std::get<0>(overlayName);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        luaProcessorSp->GetOverlayManagerLuaProxy()->OpenBackgroundOverlay(overlayToOpen);
    }
}

void LuaCommonUiFunctions::CloseCurrentOverlay(const std::tuple<>& emptyData)
{
    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        luaProcessorSp->GetOverlayManagerLuaProxy()->CloseCurrentOverlay();
    }
}

void LuaCommonUiFunctions::CloseOverlayAndClearHistory(const std::tuple<>& emptyData)
{
    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        luaProcessorSp->GetOverlayManagerLuaProxy()->CloseOverlayAndClearHistory();
    }
}

void LuaCommonUiFunctions::CloseBackgroundOverlay(const std::tuple<std::string>& overlayName)
{
    const auto& overlayToClose = std::get<0>(overlayName);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        luaProcessorSp->GetOverlayManagerLuaProxy()->CloseBackgroundOverlay(overlayToClose);
    }
}

int32_t LuaCommonUiFunctions::CreateCommonUiWidget(const std::tuple<int32_t, std::string>& data)
{
    const auto commonUiWidgetType = static_cast<eCommonUiWidgetType>(std::get<0>(data));
    const auto& jsonParametersStr = std::get<1>(data);
    CommonUiWidgetFactoryCreator factoryCreator;
    const auto& replicatorFactory = factoryCreator.GetReplicatorFactory(commonUiWidgetType);
    const int32_t luaProxyId = replicatorFactory->CreateReplicator(mSceneWp, mLuaScriptProcessor, jsonParametersStr);
    LogInfo(
        "LuaCommonUiFunctions::CreateCommonUiWidget => widgetType: ",
        static_cast<uint8_t>(commonUiWidgetType),
        ", luaProxyId: ",
        luaProxyId);
    return luaProxyId;
}

void LuaCommonUiFunctions::DestroyCommonUiWidget(const std::tuple<int32_t>& data)
{
    const auto luaProxyId = std::get<0>(data);
    if (const auto& sceneSp = mSceneWp.lock()) {
        const auto& replicatorSp = sceneSp->GetEngineToLuaReplicatorByLuaProxyId(luaProxyId);
        sceneSp->UnregisterEngineToLuaReplicator(replicatorSp->GetReplicatorId());
    }
}

bool LuaCommonUiFunctions::IsLuaProxyReady(const std::tuple<int32_t>& data)
{
    const auto luaProxyId = std::get<0>(data);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        return nullptr != luaProcessorSp->GetLuaProxy(luaProxyId);
    }

    return false;
}

std::string LuaCommonUiFunctions::GetUiWidgetName(const std::tuple<int32_t>& data)
{
    const auto luaProxyId = std::get<0>(data);
    std::string widgetName;

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        if (const auto& luaProxy = luaProcessorSp->GetLuaProxy(luaProxyId)) {
            if (std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProxy)) {
                const auto& uiItemWidgetProxy = std::static_pointer_cast<UiItemBaseLuaProxy>(luaProxy);
                widgetName = uiItemWidgetProxy->GetUiItemName();
            } else if (std::dynamic_pointer_cast<UiCanvasLuaProxy>(luaProxy)) {
                const auto& uiCanvasProxy = std::static_pointer_cast<UiCanvasLuaProxy>(luaProxy);
                widgetName = uiCanvasProxy->GetCanvasName();
            } else {
                assert(false);
            }
        }
    }

    return widgetName;
}

void LuaCommonUiFunctions::SetUiWidgetParent(
    const std::tuple<int32_t /*lua proxy id*/, std::string /*canvas name*/, std::string /*parent name*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    const auto canvasName = std::get<1>(data);
    const auto parentName = std::get<2>(data);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        if (const auto& luaProxy = luaProcessorSp->GetLuaProxy(luaProxyId)) {
            const auto& uiItemWidgetProxy = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProxy);
            assert(uiItemWidgetProxy);
            uiItemWidgetProxy->SetParent(canvasName, parentName);
        }
    }
}

void LuaCommonUiFunctions::OnCommonUiWidgetDataUpdated(
    const std::tuple<int32_t /*lua proxy id*/, std::string /*json data*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    const auto& jsonDataStr = std::get<1>(data);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        if (const auto& luaProxySp = luaProcessorSp->GetLuaProxy(luaProxyId)) {
            luaProxySp->OnLuaThreadDataUpdated(jsonDataStr);
        }
    }
}

std::string LuaCommonUiFunctions::GetGameThreadData(const std::tuple<int32_t /*lua proxy id*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        if (const auto& luaProxySp = luaProcessorSp->GetLuaProxy(luaProxyId)) {
            if (luaProxySp->IsLuaDataDirty()) {
                return luaProxySp->GetGameThreadData();
            }
        }
    }
    return "";
}

std::string LuaCommonUiFunctions::GetMouseInputData(const std::tuple<int32_t /*lua proxy id*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        if (const auto& luaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId))) {
            if (luaProxySp->IsMouseInputDataDirty()) {
                return luaProxySp->GetMouseInputData();
            }
        }
    }
    return "";
}

void LuaCommonUiFunctions::InitializeCanvasInputSystem(const std::tuple<int32_t /*lua proxy id*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        const auto& canvasSp = std::dynamic_pointer_cast<UiCanvasLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId));
        assert(canvasSp);
        canvasSp->InitializeInputSystem();
    }
}

void LuaCommonUiFunctions::AddCanvasFadeAnimation(
    const std::tuple<int32_t /*lua proxy id*/, int32_t /*0 - fadeIn, 1 - fadeOut*/, std::string /*animation json data*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    const std::string animationName = std::get<1>(data) == 0 ? "FadeIn" : "FadeOut";
    const auto animationJsonData = std::get<2>(data);
    const auto animationDataJsonObj = nlohmann::json::parse(animationJsonData);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        const auto& canvasSp = std::dynamic_pointer_cast<UiCanvasLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId));
        assert(canvasSp);
        canvasSp->AddAnimation(animationName, ParseAnimationDataFromJson(animationDataJsonObj));
    }
}

AnimationData LuaCommonUiFunctions::ParseAnimationDataFromJson(const nlohmann::json& jsonObj)
{
    assert(jsonObj.contains("animatedPropertyType"));
    assert(jsonObj.contains("animationFunctionType"));
    assert(jsonObj.contains("animationDuration"));
    assert(jsonObj.contains("animatedPropertyName"));
    assert(jsonObj.contains("propertySrcValue"));
    assert(jsonObj.contains("propertyDstValue"));
    const auto functionType = static_cast<eAnimationInterpolationFunctionType>(jsonObj["animationFunctionType"].get<int32_t>());
    const auto propType = static_cast<eEnginePropertyType>(jsonObj["animatedPropertyType"].get<int32_t>());
    const auto animationDuration = jsonObj["animationDuration"].get<float>();
    const auto animatedPropertyName = jsonObj["animatedPropertyName"].get<std::string>();
    std::any srcData, dstData;
    if (eEnginePropertyType::Float == propType) {
        srcData = jsonObj["propertySrcValue"].get<float>();
        dstData = jsonObj["propertyDstValue"].get<float>();
    } else if (eEnginePropertyType::Integer == propType) {
        srcData = jsonObj["propertySrcValue"].get<int32_t>();
        dstData = jsonObj["propertyDstValue"].get<int32_t>();
    } else if (eEnginePropertyType::Vec3 == propType) {
        auto x = jsonObj["propertySrcValue"].at("x").get<float>();
        auto y = jsonObj["propertySrcValue"].at("y").get<float>();
        auto z = jsonObj["propertySrcValue"].at("z").get<float>();
        srcData = glm::vec3(x, y, z);

        x = jsonObj["propertyDstValue"].at("x").get<float>();
        y = jsonObj["propertyDstValue"].at("y").get<float>();
        z = jsonObj["propertyDstValue"].at("z").get<float>();
        dstData = glm::vec3(x, y, z);
    } else {
        assert(false); // Not supported yet
    }

    return AnimationData(functionType, animationDuration, animatedPropertyName, srcData, dstData);
}

void LuaCommonUiFunctions::AddUiItemAnimation(
    const std::tuple<int32_t /*lua proxy id*/, std::string /*animation name*/, std::string /*animation json data*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    const auto& animationName = std::get<1>(data);
    const auto& animationJsonData = std::get<2>(data);
    const auto& animationDataJsonObj = nlohmann::json::parse(animationJsonData);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        const auto& uiItemBaseLuaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId));
        assert(uiItemBaseLuaProxySp);

        uiItemBaseLuaProxySp->AddAnimation(animationName, ParseAnimationDataFromJson(animationDataJsonObj));
    }
}

void LuaCommonUiFunctions::AddUiItemSequenceAnimation(
    const std::tuple<int32_t /*lua proxy id*/, std::string /*animation name*/, std::string /*animation json data*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    const auto& animationName = std::get<1>(data);
    const auto& animationJsonData = std::get<2>(data);
    const auto animationRootDataMap = nlohmann::json::parse(animationJsonData);

    std::vector<AnimationData> animationsDataList;
    for (auto it = animationRootDataMap.begin(); it != animationRootDataMap.end(); ++it) {
        const auto& animationSequenceMap = it.value();
        animationsDataList.emplace_back(ParseAnimationDataFromJson(animationSequenceMap));
    }
    AnimationSequence resultAnimationSequence;
    resultAnimationSequence.SetAnimationDataInSequence(animationsDataList);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        const auto& uiItemBaseLuaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId));
        assert(uiItemBaseLuaProxySp);

        uiItemBaseLuaProxySp->AddSequenceAnimation(animationName, resultAnimationSequence);
    }
}

void LuaCommonUiFunctions::StartUiItemAnimation(const std::tuple<int32_t /*lua proxy id*/, std::string /*animation name*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    const auto animationName = std::get<1>(data);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        const auto& uiItemBaseLuaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId));
        assert(uiItemBaseLuaProxySp);
        uiItemBaseLuaProxySp->StartAnimation(animationName);
    }
}

void LuaCommonUiFunctions::StartUiItemSequenceAnimation(
    const std::tuple<int32_t /*lua proxy id*/, std::string /*animation name*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    const auto animationName = std::get<1>(data);

    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        const auto& uiItemBaseLuaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId));
        assert(uiItemBaseLuaProxySp);
        uiItemBaseLuaProxySp->StartSequenceAnimation(animationName);
    }
}

void LuaCommonUiFunctions::EnableToggleButtonMouseInputReceiver(const std::tuple<int32_t /*lua proxy id*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        const auto& toggleButtonSp = std::dynamic_pointer_cast<UiToggleButtonLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId));
        assert(toggleButtonSp);
        toggleButtonSp->EnableMouseInputReceiver();
    }
}

void LuaCommonUiFunctions::EnableMouseInputReceiverBase(const std::tuple<int32_t /*lua proxy id*/>& data)
{
    const auto luaProxyId = std::get<0>(data);
    if (const auto& luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock()) {
        const auto& uiItemBaseSp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId));
        assert(uiItemBaseSp);
        uiItemBaseSp->EnableMouseInputReceiverBase();
    }
}
} // namespace Scripts
} // namespace EngineCore
