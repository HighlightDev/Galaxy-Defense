#pragma once

#include "Core/CommonCore/ObserverConnection.h"
#include "Core/GameCore/AnimationBlendState.h"
#include "Core/GameCore/LoggerExtension.h"
#include "EnginePropertyType.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <string>
#include <type_traits>

struct EngineObjectPropertyBase {
    std::string Key;

protected:
    eEnginePropertyType mEnginePropertyType{eEnginePropertyType::Undefined};

    mutable std::shared_ptr<EngineCore::ObserverConnection<void(void)>> mObserverConnection;

public:
    EngineObjectPropertyBase(const std::string& key)
        : Key(key)
    {
    }

    eEnginePropertyType GetPropertyType() const
    {
        return mEnginePropertyType;
    }

    std::weak_ptr<EngineCore::ObserverConnection<void(void)>> GetObserverConnection() const
    {
        if (mObserverConnection == nullptr) {
            mObserverConnection = EngineCore::ObserverConnection<void(void)>::CreateInstance();
        }
        return mObserverConnection;
    }
};

template<typename Type>
struct EngineObjectProperty : public EngineObjectPropertyBase {

    using Action_t = std::function<void(const Type&)>;

protected:
    std::unique_ptr<Action_t> Action;

    std::shared_ptr<Type> ValuePtr;

    Type InitialValue;

public:
    template<typename ValueType, typename FunctionType>
    EngineObjectProperty(const ValueType& value, const std::string& key, FunctionType action)
        : EngineObjectPropertyBase(key)
        , ValuePtr(std::make_shared<Type>(value))
        , Action(std::make_unique<Action_t>(action))
        , InitialValue(value)
    {
        using decayed_type_t = typename std::decay<Type>::type;
        if constexpr (std::is_same_v<decayed_type_t, bool>) {
            mEnginePropertyType = eEnginePropertyType::Boolean;
        } else if constexpr (std::is_same_v<decayed_type_t, float>) {
            mEnginePropertyType = eEnginePropertyType::Float;
        } else if constexpr (std::is_same_v<decayed_type_t, int32_t>) {
            mEnginePropertyType = eEnginePropertyType::Integer;
        } else if constexpr (std::is_same_v<decayed_type_t, glm::ivec2>) {
            mEnginePropertyType = eEnginePropertyType::iVec2;
        } else if constexpr (std::is_same_v<decayed_type_t, glm::vec2>) {
            mEnginePropertyType = eEnginePropertyType::Vec2;
        } else if constexpr (std::is_same_v<decayed_type_t, glm::vec3>) {
            mEnginePropertyType = eEnginePropertyType::Vec3;
        } else if constexpr (std::is_same_v<decayed_type_t, EngineCore::AnimationBlendState>) {
            mEnginePropertyType = eEnginePropertyType::Animation;
        }
    }

    template<typename ValueType>
    EngineObjectProperty(const ValueType& value, const std::string& key)
        : EngineObjectPropertyBase(key)
        , ValuePtr(std::make_shared<Type>(value))
        , Action()
        , InitialValue(value)
    {
        using decayed_type_t = typename std::decay<Type>::type;
        if constexpr (std::is_same_v<decayed_type_t, bool>) {
            mEnginePropertyType = eEnginePropertyType::Boolean;
        } else if constexpr (std::is_same_v<decayed_type_t, float>) {
            mEnginePropertyType = eEnginePropertyType::Float;
        } else if constexpr (std::is_same_v<decayed_type_t, int32_t>) {
            mEnginePropertyType = eEnginePropertyType::Integer;
        } else if constexpr (std::is_same_v<decayed_type_t, glm::ivec2>) {
            mEnginePropertyType = eEnginePropertyType::iVec2;
        } else if constexpr (std::is_same_v<decayed_type_t, glm::vec2>) {
            mEnginePropertyType = eEnginePropertyType::Vec2;
        } else if constexpr (std::is_same_v<decayed_type_t, glm::vec3>) {
            mEnginePropertyType = eEnginePropertyType::Vec3;
        } else if constexpr (std::is_same_v<decayed_type_t, EngineCore::AnimationBlendState>) {
            mEnginePropertyType = eEnginePropertyType::Animation;
        }
    }

    std::shared_ptr<Type> GetValuePtr()
    {
        return ValuePtr;
    }

    Type GetValue() const
    {
        return *ValuePtr;
    }

    Type GetInitialValue() const
    {
        return InitialValue;
    }

    void SetValue(const Type& value, const bool triggerAction = true)
    {
        if (value != *ValuePtr) {
            *ValuePtr = value;

            if (triggerAction && Action) {
                (*(Action.get()))(value);
            }

            if (mObserverConnection) {
                mObserverConnection->NotifyAll();
            }
        }
    }

    EngineObjectProperty<Type>& operator=(const Type& value)
    {
        SetValue(value);
        return *this;
    }

    operator Type() const
    {
        return *ValuePtr;
    }

    operator Type&()
    {
        return *ValuePtr;
    }

    operator const Type&() const
    {
        return *ValuePtr;
    }
};
