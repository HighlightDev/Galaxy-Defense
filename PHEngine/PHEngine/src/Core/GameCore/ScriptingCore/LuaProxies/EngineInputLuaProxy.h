#pragma once

#include "LuaProxy.h"
#include "Core/GameCore/Input/Keys.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"
#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"
#include "Core/GameCore/Event/MouseButtonDownEvent.h"

#include <string>
#include <vector>

using namespace Event;

namespace EngineCore
{
    namespace Scripts
    {
        class EngineInputLuaProxy : public LuaProxy,
                                    public LuaThreadKeyboardButtonDownEvent,
                                    public LuaThreadMouseMovedEvent,
                                    public LuaThreadMouseScrollEvent,
                                    public LuaThreadMouseButtonDownEvent,
                                    public std::enable_shared_from_this<EngineInputLuaProxy>

        {
            // keyboard event data
            std::vector<eKeyboardKeys> mReleasedKeysOnCurrentTick;
            std::vector<eKeyboardKeys> mPressedKeysOnCurrentTick;

            bool mIsPressedKeyboardKeys;
            bool mIsReleasedKeyboardKeys;

            std::string mKeyboardJsonData;

            // mouse event data

        public:
            EngineInputLuaProxy();

            ~EngineInputLuaProxy() override;

            void Initialize();

            void CleanUp();

            void ProcessEvent(const typename LuaThreadKeyboardButtonDownEvent::EventData_t &data) override;

            void ProcessEvent(const typename LuaThreadMouseMovedEvent::EventData_t &data) override;

            void ProcessEvent(const typename LuaThreadMouseScrollEvent::EventData_t &data) override;

            void ProcessEvent(const typename LuaThreadMouseButtonDownEvent::EventData_t &data) override;

            void OnLuaThreadDataUpdated(const std::string &jsonParameters) override;

            std::string GetGameThreadData() override;

            bool GetIsPressedKeyboardKeys() const;

            bool GetIsReleasedKeyboardKeys() const;

            std::string GetKeyboardJsonData() const;

        private:
            void PrepareKeyboardJsonData();
        };
    }
}