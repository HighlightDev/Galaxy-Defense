#include "LuaCommonEngineFunctions.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

using namespace EngineCore;
using namespace EngineCore::DataProviders;
using namespace IO;

namespace EngineCore {
namespace Scripts {
LuaCommonEngineFunctions::LuaCommonEngineFunctions(LuaScriptExecutorBase* ownerPtr)
    : mOwnerPtr(ownerPtr)
{
}

void LuaCommonEngineFunctions::SetScene(const std::weak_ptr<Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

void LuaCommonEngineFunctions::Initialize()
{
}

void LuaCommonEngineFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
{
    mLuaScriptProcessor = scriptProcessor;
}

void LuaCommonEngineFunctions::OnScriptStarted(const LuaWrapper& luaWrapper)
{
}

void LuaCommonEngineFunctions::OnScriptStopped(const LuaWrapper& luaWrapper)
{
}

void LuaCommonEngineFunctions::RegisterCallbacks(const LuaWrapper& luaWrapper)
{
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetWindowHeight"), int32_t(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonEngineFunctions::GetWindowHeight, this, std::placeholders::_1),
        "_GetWindowHeight");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetWindowWidth"), int32_t(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonEngineFunctions::GetWindowWidth, this, std::placeholders::_1),
        "_GetWindowWidth");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::HasPressedKeyboardButtons"), bool(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonEngineFunctions::HasPressedKeyboardButtons, this, std::placeholders::_1),
        "_HasPressedKeyboardButtons");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::HasReleasedKeyboardButtons"), bool(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonEngineFunctions::HasReleasedKeyboardButtons, this, std::placeholders::_1),
        "_HasReleasedKeyboardButtons");
    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetKeyboardJsonData"), std::string(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonEngineFunctions::GetKeyboardJsonData, this, std::placeholders::_1),
        "_GetKeyboardJsonData");
}

int32_t LuaCommonEngineFunctions::GetWindowHeight(const std::tuple<>& data)
{
    return GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight();
}

int32_t LuaCommonEngineFunctions::GetWindowWidth(const std::tuple<>& data)
{
    return GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth();
}

bool LuaCommonEngineFunctions::HasPressedKeyboardButtons(const std::tuple<>& data)
{
    if (const auto& luaProcessorSp = mLuaScriptProcessor.lock()) {
        return luaProcessorSp->GetEngineInputLuaProxy()->GetIsPressedKeyboardKeys();
    }
    return false;
}

bool LuaCommonEngineFunctions::HasReleasedKeyboardButtons(const std::tuple<>& data)
{
    if (const auto& luaProcessorSp = mLuaScriptProcessor.lock()) {
        return luaProcessorSp->GetEngineInputLuaProxy()->GetIsReleasedKeyboardKeys();
    }
    return false;
}

std::string LuaCommonEngineFunctions::GetKeyboardJsonData(const std::tuple<>& data)
{
    if (const auto& luaProcessorSp = mLuaScriptProcessor.lock()) {
        return luaProcessorSp->GetEngineInputLuaProxy()->GetKeyboardJsonData();
    }
    return "";
}
} // namespace Scripts
} // namespace EngineCore
