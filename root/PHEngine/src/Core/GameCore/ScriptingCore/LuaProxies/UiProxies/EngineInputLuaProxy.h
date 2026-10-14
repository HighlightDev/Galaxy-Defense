#pragma once

#include "Core/GameCore/Event/KeyboardInputEvent.h"
#include "Core/GameCore/Event/MouseButtonDownEvent.h"
#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"
#include "Core/GameCore/Input/Keys.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"

#include <glm/vec2.hpp>

#include <string>
#include <vector>

using namespace Event;

namespace EngineCore {
namespace Scripts {
class EngineInputLuaProxy : public LuaProxy,
                            public KeyboardButtonDownLuaThreadEvent,
                            public MouseMovedLuaThreadEvent,
                            public MouseScrollLuaThreadEvent,
                            public MouseButtonDownLuaThreadEvent,
                            public std::enable_shared_from_this<EngineInputLuaProxy>

{
    // keyboard event data
    std::vector<eKeyboardKeys> mReleasedKeysOnCurrentTick;
    std::vector<eKeyboardKeys> mPressedKeysOnCurrentTick;

    bool mIsPressedKeyboardKeys;
    bool mIsReleasedKeyboardKeys;

    std::string mKeyboardJsonData;

    // mouse event data
    glm::ivec2 mMouseCursorPosition{0, 0};

public:
    EngineInputLuaProxy();

    ~EngineInputLuaProxy() override;

    void Initialize();

    void CleanUp();

    void ProcessEvent(
        const KeyboardButtonDownLuaThreadEvent* sender,
        const typename KeyboardButtonDownLuaThreadEvent::EventData_t& data) override;

    void
    ProcessEvent(const MouseMovedLuaThreadEvent* sender, const typename MouseMovedLuaThreadEvent::EventData_t& data) override;

    void
    ProcessEvent(const MouseScrollLuaThreadEvent* sender, const typename MouseScrollLuaThreadEvent::EventData_t& data) override;

    void ProcessEvent(
        const MouseButtonDownLuaThreadEvent* sender, const typename MouseButtonDownLuaThreadEvent::EventData_t& data) override;

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override
    {
        // No functions to invoke for now
        return false;
    }

    std::string GetGameThreadData() override;

    bool GetIsPressedKeyboardKeys() const;

    bool GetIsReleasedKeyboardKeys() const;

    std::string GetKeyboardJsonData() const;

    glm::ivec2 GetMouseCursorPosition() const;

private:
    void PrepareKeyboardJsonData();
};
} // namespace Scripts
} // namespace EngineCore