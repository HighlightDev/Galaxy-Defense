#include "LuaComponentHandlerFunctions.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/UtilityCore/JsonUtilities.h"

using namespace EngineUtility;
using namespace EngineCore::Scripts;

namespace EngineCore {
namespace Scripts {
LuaComponentHandlerFunctions::LuaComponentHandlerFunctions(LuaScriptExecutorBase* ownerPtr)
    : mOwnerPtr(ownerPtr)
    , mSceneWp()
{
}

LuaComponentHandlerFunctions::~LuaComponentHandlerFunctions()
{
}

void LuaComponentHandlerFunctions::Initialize()
{
}

void LuaComponentHandlerFunctions::SetScene(const std::weak_ptr<Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

void LuaComponentHandlerFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
{
    mLuaScriptProcessor = scriptProcessor;
}

void LuaComponentHandlerFunctions::OnScriptStarted(const LuaWrapper& luaWrapper)
{
}

void LuaComponentHandlerFunctions::OnScriptStopped(const LuaWrapper& luaWrapper)
{
}

void LuaComponentHandlerFunctions::RegisterCallbacks(const LuaWrapper& luaWrapper)
{
    LuaCallbackBindingHelper<
        Hash64_CT("LuaComponentHandlerFunctions::GetLuaProxyIdForComponent"),
        int32_t(std::string /*component name*/)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaComponentHandlerFunctions::GetLuaProxyIdForComponent, this, std::placeholders::_1),
            "_GetLuaProxyIdForComponent");

    LuaCallbackBindingHelper<
        Hash64_CT("LuaComponentHandlerFunctions::InvokeFunction"),
        bool(int32_t /*luaProxyId*/, std::string /*functionName*/, std::string /*jsonParameters*/)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaComponentHandlerFunctions::InvokeFunction, this, std::placeholders::_1),
            "_InvokeFunction");
}

int32_t LuaComponentHandlerFunctions::GetLuaProxyIdForComponent(const std::tuple<std::string /*component name*/>& data)
{
    const auto componentName = std::get<0>(data);
    int32_t luaProxyId = -1;

    if (const auto sceneSp = mSceneWp.lock()) {
        if (const auto luaScriptProcessorSp = mLuaScriptProcessor.lock()) {
            const auto& componentSp = sceneSp->GetEngineObjectByName(componentName);
            if (componentSp) {
                const auto componentPtr = std::dynamic_pointer_cast<Component>(componentSp);
                if (componentPtr) {
                    luaProxyId = componentPtr->GetLuaProxyId();
                }
            }
        }
    }

    return luaProxyId;
}

bool LuaComponentHandlerFunctions::InvokeFunction(const std::tuple<int32_t, std::string, std::string>& data)
{
    const auto luaProxyId = std::get<0>(data);
    const auto& functionName = std::get<1>(data);
    const auto& jsonParameters = std::get<2>(data);
    bool result = false;

    if (const auto sceneSp = mSceneWp.lock()) {
        if (const auto luaScriptProcessorSp = mLuaScriptProcessor.lock()) {
            const auto luaProxySp = luaScriptProcessorSp->GetLuaProxy(luaProxyId);
            if (luaProxySp) {
                result = luaProxySp->InvokeFunction(functionName, jsonParameters);
            }
        }
    }

    LogInfo(
        "LuaComponentHandlerFunctions::InvokeFunction: Invoked function:",
        functionName,
        "on LuaProxy with ID:",
        luaProxyId,
        "result:",
        result);

    return result;
}
} // namespace Scripts
} // namespace EngineCore
