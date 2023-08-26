#include "EngineInputLuaProxy.h"

#include <json/json.hpp>

namespace EngineCore
{
    namespace Scripts
    {
        EngineInputLuaProxy::EngineInputLuaProxy()
            : mIsPressedKeyboardKeys(false),
              mIsReleasedKeyboardKeys(false),
              mKeyboardJsonData("")
        {
            mLuaProxyId = CreateUniqueLuaProxyId();
        }

        EngineInputLuaProxy::~EngineInputLuaProxy()
        {
            LuaThreadKeyboardButtonDownEvent::GetInstance()->RemoveListener(LuaThreadKeyboardButtonDownEvent::GetInstanceId());
            LuaThreadMouseMovedEvent::GetInstance()->RemoveListener(LuaThreadMouseMovedEvent::GetInstanceId());
            LuaThreadMouseScrollEvent::GetInstance()->RemoveListener(LuaThreadMouseScrollEvent::GetInstanceId());
            LuaThreadMouseButtonDownEvent::GetInstance()->RemoveListener(LuaThreadMouseButtonDownEvent::GetInstanceId());
        }

        void EngineInputLuaProxy::Initialize()
        {
            const auto thisSp = shared_from_this();
            LuaThreadKeyboardButtonDownEvent::GetInstance()->AddListener(thisSp);
            LuaThreadMouseMovedEvent::GetInstance()->AddListener(thisSp);
            LuaThreadMouseScrollEvent::GetInstance()->AddListener(thisSp);
            LuaThreadMouseButtonDownEvent::GetInstance()->AddListener(thisSp);
        }

        void EngineInputLuaProxy::CleanUp()
        {
            mIsPressedKeyboardKeys = false;
            mIsReleasedKeyboardKeys = false;

            mPressedKeysOnCurrentTick.clear();
            mReleasedKeysOnCurrentTick.clear();
        }

        void EngineInputLuaProxy::ProcessEvent(const typename LuaThreadKeyboardButtonDownEvent::EventData_t &data)
        {
            const auto &keyboardKeysState = std::get<0>(data);

            mPressedKeysOnCurrentTick.clear();
            mReleasedKeysOnCurrentTick.clear();

            for (const auto &keyboardKeyData : keyboardKeysState)
            {
                if (KeyState::PRESSED == keyboardKeyData.State)
                {
                    mPressedKeysOnCurrentTick.emplace_back(keyboardKeyData.Key);
                }
                else
                {
                    mReleasedKeysOnCurrentTick.emplace_back(keyboardKeyData.Key);
                }
            }

            const auto &isCurrentPressedKeyboardKeys = mPressedKeysOnCurrentTick.size() > 0;
            const auto &isCurrentReleasedKeyboardKeys = mReleasedKeysOnCurrentTick.size() > 0;
            bool isKeyboardDataDirty = false;
            if (mIsPressedKeyboardKeys != isCurrentPressedKeyboardKeys)
            {
                mIsPressedKeyboardKeys = isCurrentPressedKeyboardKeys;
                isKeyboardDataDirty = true;
            }

            if (mIsReleasedKeyboardKeys != isCurrentReleasedKeyboardKeys)
            {
                mIsReleasedKeyboardKeys = isCurrentReleasedKeyboardKeys;
                isKeyboardDataDirty = true;
            }

            if (isKeyboardDataDirty)
            {
                PrepareKeyboardJsonData();
            }
        }

        void EngineInputLuaProxy::ProcessEvent(const typename LuaThreadMouseMovedEvent::EventData_t &data)
        {
            // todo: to be implemented later
        }

        void EngineInputLuaProxy::ProcessEvent(const typename LuaThreadMouseScrollEvent::EventData_t &data)
        {
            // todo: to be implemented later
        }

        void EngineInputLuaProxy::ProcessEvent(const typename LuaThreadMouseButtonDownEvent::EventData_t &data)
        {
            // todo: to be implemented later
        }

        bool EngineInputLuaProxy::GetIsPressedKeyboardKeys() const
        {
            return mIsPressedKeyboardKeys;
        }

        bool EngineInputLuaProxy::GetIsReleasedKeyboardKeys() const
        {
            return mIsReleasedKeyboardKeys;
        }

        std::string EngineInputLuaProxy::GetKeyboardJsonData() const
        {
            return mKeyboardJsonData;
        }

        void EngineInputLuaProxy::PrepareKeyboardJsonData()
        {
            if (mPressedKeysOnCurrentTick.size())
            {
                nlohmann::json pressedKeysJson;
                pressedKeysJson["pressed_keys"] = mPressedKeysOnCurrentTick;
                mKeyboardJsonData = pressedKeysJson.dump();
            }
        }

        void EngineInputLuaProxy::OnLuaThreadDataUpdated(const std::string &jsonParameters)
        {
        }

        std::string EngineInputLuaProxy::GetGameThreadData()
        {
            return "";
        }
    }
}