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
            KeyboardButtonDownLuaThreadEvent::GetInstance()->RemoveListener(KeyboardButtonDownLuaThreadEvent::GetInstanceId());
            MouseMovedLuaThreadEvent::GetInstance()->RemoveListener(MouseMovedLuaThreadEvent::GetInstanceId());
            MouseScrollLuaThreadEvent::GetInstance()->RemoveListener(MouseScrollLuaThreadEvent::GetInstanceId());
            MouseButtonDownLuaThreadEvent::GetInstance()->RemoveListener(MouseButtonDownLuaThreadEvent::GetInstanceId());
        }

        void EngineInputLuaProxy::Initialize()
        {
            const auto thisSp = shared_from_this();
            KeyboardButtonDownLuaThreadEvent::GetInstance()->AddListener(thisSp);
            MouseMovedLuaThreadEvent::GetInstance()->AddListener(thisSp);
            MouseScrollLuaThreadEvent::GetInstance()->AddListener(thisSp);
            MouseButtonDownLuaThreadEvent::GetInstance()->AddListener(thisSp);
        }

        void EngineInputLuaProxy::CleanUp()
        {
            mIsPressedKeyboardKeys = false;
            mIsReleasedKeyboardKeys = false;

            mPressedKeysOnCurrentTick.clear();
            mReleasedKeysOnCurrentTick.clear();
        }

        void EngineInputLuaProxy::ProcessEvent(const KeyboardButtonDownLuaThreadEvent* sender, const typename KeyboardButtonDownLuaThreadEvent::EventData_t &data)
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

        void EngineInputLuaProxy::ProcessEvent(const MouseMovedLuaThreadEvent* sender, const typename MouseMovedLuaThreadEvent::EventData_t &data)
        {
            // todo: to be implemented later
        }

        void EngineInputLuaProxy::ProcessEvent(const MouseScrollLuaThreadEvent* sender, const typename MouseScrollLuaThreadEvent::EventData_t &data)
        {
            // todo: to be implemented later
        }

        void EngineInputLuaProxy::ProcessEvent(const MouseButtonDownLuaThreadEvent* sender, const typename MouseButtonDownLuaThreadEvent::EventData_t &data)
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