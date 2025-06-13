#pragma once
#include "Core/GameCore/Scene.h"
#include "ILuaFunctionable.h"

#include <glm/vec3.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>

using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
class LuaScriptExecutorBase;
class LuaScriptProcessor;

class LuaCommonEngineFunctions : public ILuaFunctionable {
protected:
    LuaScriptExecutorBase* mOwnerPtr;

    std::weak_ptr<Scene> mSceneWp;

    std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessor;

public:
    LuaCommonEngineFunctions(LuaScriptExecutorBase* ownerPtr);

    virtual ~LuaCommonEngineFunctions() = default;

    void OnScriptStarted(const LuaWrapper& luaWrapper) override;

    void OnScriptStopped(const LuaWrapper& luaWrapper) override;

    void RegisterCallbacks(const LuaWrapper& luaWrapper) override;

    void SetScene(const std::weak_ptr<Scene>& sceneWp) override;

    void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor) override;

    void Initialize() override;

private:
    /*Get Window height*/
    int32_t GetWindowHeight(const std::tuple<>& data) const;

    /*Get Window width*/
    int32_t GetWindowWidth(const std::tuple<>& data) const;

    /*Getters for scene properties*/
    float GetMusicGain(const std::tuple<>& data) const;
    
    float GetSoundGain(const std::tuple<>& data) const;

    /*Input callbacks*/
    bool HasPressedKeyboardButtons(const std::tuple<>& data) const;

    bool HasReleasedKeyboardButtons(const std::tuple<>& data) const;

    std::string GetKeyboardJsonData(const std::tuple<>& data) const;
};
} // namespace Scripts
} // namespace EngineCore
