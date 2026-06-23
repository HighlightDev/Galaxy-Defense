#include "LuaCommonEngineFunctions.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontAtlas.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontParams.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeTextMeshCreator.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/ResourceManagerCore/Pool/FreeTypeFontMeshPool.h"

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

    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetMusicGain"), float(void)>::Bind(
        luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::GetMusicGain, this, std::placeholders::_1), "_GetMusicGain");

    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetSoundGain"), float(void)>::Bind(
        luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::GetSoundGain, this, std::placeholders::_1), "_GetSoundGain");

    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetMouseCursorPositionX"), int32_t(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonEngineFunctions::GetMouseCursorPositionX, this, std::placeholders::_1),
        "_GetMouseCursorPositionX");

    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetMouseCursorPositionY"), int32_t(void)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaCommonEngineFunctions::GetMouseCursorPositionY, this, std::placeholders::_1),
        "_GetMouseCursorPositionY");

    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetLabelWidthPx"), int32_t(std::string, int32_t, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaCommonEngineFunctions::GetLabelWidthPx, this, std::placeholders::_1),
            "_GetLabelWidthPx");

    LuaCallbackBindingHelper<Hash64_CT("LuaCommonEngineFunctions::GetPlaySpeed"), float(void)>::Bind(
        luaWrapper, mOwnerPtr, std::bind(&LuaCommonEngineFunctions::GetPlaySpeed, this, std::placeholders::_1), "_GetPlaySpeed");
}

int32_t LuaCommonEngineFunctions::GetWindowHeight(const std::tuple<>& data) const
{
    return GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight();
}

int32_t LuaCommonEngineFunctions::GetWindowWidth(const std::tuple<>& data) const
{
    return GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth();
}

bool LuaCommonEngineFunctions::HasPressedKeyboardButtons(const std::tuple<>& data) const
{
    if (const auto& luaProcessorSp = mLuaScriptProcessor.lock()) {
        return luaProcessorSp->GetEngineInputLuaProxy()->GetIsPressedKeyboardKeys();
    }
    return false;
}

bool LuaCommonEngineFunctions::HasReleasedKeyboardButtons(const std::tuple<>& data) const
{
    if (const auto& luaProcessorSp = mLuaScriptProcessor.lock()) {
        return luaProcessorSp->GetEngineInputLuaProxy()->GetIsReleasedKeyboardKeys();
    }
    return false;
}

std::string LuaCommonEngineFunctions::GetKeyboardJsonData(const std::tuple<>& data) const
{
    if (const auto& luaProcessorSp = mLuaScriptProcessor.lock()) {
        return luaProcessorSp->GetEngineInputLuaProxy()->GetKeyboardJsonData();
    }
    return "";
}

float LuaCommonEngineFunctions::GetMusicGain(const std::tuple<>& data) const
{
    return GeneralSystemSettingsDataProvider::GetInstance()->GetMusicGain();
}

float LuaCommonEngineFunctions::GetSoundGain(const std::tuple<>& data) const
{
    return GeneralSystemSettingsDataProvider::GetInstance()->GetSoundGain();
}

int32_t LuaCommonEngineFunctions::GetMouseCursorPositionX(const std::tuple<>& data) const
{
    if (const auto& luaProcessorSp = mLuaScriptProcessor.lock()) {
        return luaProcessorSp->GetEngineInputLuaProxy()->GetMouseCursorPosition().x;
    }
    return 0;
}

int32_t LuaCommonEngineFunctions::GetMouseCursorPositionY(const std::tuple<>& data) const
{
    if (const auto& luaProcessorSp = mLuaScriptProcessor.lock()) {
        const int32_t windowHeight = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight();
        return windowHeight - luaProcessorSp->GetEngineInputLuaProxy()->GetMouseCursorPosition().y;
    }
    return 0;
}

int32_t LuaCommonEngineFunctions::GetLabelWidthPx(const std::tuple<std::string, int32_t, std::string>& data) const
{
    const auto& fontName = std::get<0>(data);
    const int32_t fontSize = std::get<1>(data);
    const auto& text = std::get<2>(data);
    if (fontName.empty() || fontSize <= 0 || text.empty()) {
        return 0;
    }
    // Reuses the same FreeType atlas the font batcher draws from, so the
    // answer matches what would actually be rendered. The pool's
    // GetOrAllocateResource caches per (fontName, size), so consecutive
    // measurements of the same font cost a single hashmap lookup. The first
    // call for a brand-new font triggers atlas allocation (heavier).
    const FreeTypeFontParams params(fontName, fontSize);
    const auto& atlas = ::Resources::FreeTypeFontMeshPool::GetInstance()->GetOrAllocateResource(params);
    if (!atlas) {
        return 0;
    }
    return FreeTypeTextMeshCreator::CalcWidth(text, atlas);
}

float LuaCommonEngineFunctions::GetPlaySpeed(const std::tuple<>& data) const
{
    return 1.0f; // todo: provide real speed
}

} // namespace Scripts
} // namespace EngineCore
