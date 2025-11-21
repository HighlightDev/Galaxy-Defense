#pragma once

#include "Core/GameCore/LoggerExtension.h"
#include "EnginePropertyType.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
namespace {
template<typename T>
struct ConvertTypeToEnginePropertyType;

template<>
struct ConvertTypeToEnginePropertyType<float> {
    static constexpr eEnginePropertyType value = eEnginePropertyType::Float;
};

template<>
struct ConvertTypeToEnginePropertyType<bool> {
    static constexpr eEnginePropertyType value = eEnginePropertyType::Boolean;
};

template<>
struct ConvertTypeToEnginePropertyType<glm::vec3> {
    static constexpr eEnginePropertyType value = eEnginePropertyType::Vec3;
};

template<>
struct ConvertTypeToEnginePropertyType<glm::ivec2> {
    static constexpr eEnginePropertyType value = eEnginePropertyType::iVec2;
};

template<>
struct ConvertTypeToEnginePropertyType<glm::vec2> {
    static constexpr eEnginePropertyType value = eEnginePropertyType::Vec2;
};

template<>
struct ConvertTypeToEnginePropertyType<int32_t> {
    static constexpr eEnginePropertyType value = eEnginePropertyType::Integer;
};
} // namespace

struct EngineObjectPropertyBase {
    std::string Key;

protected:
    eEnginePropertyType mEnginePropertyType{eEnginePropertyType::Undefined};

public:
    EngineObjectPropertyBase(const std::string& key)
        : Key(key)
    {
    }

    eEnginePropertyType GetPropertyType() const
    {
        return mEnginePropertyType;
    }
};

template<typename Type>
struct EngineObjectProperty : public EngineObjectPropertyBase {

    using Action_t = std::function<void(const Type&)>;

protected:
    std::unique_ptr<Action_t> Action;

    std::shared_ptr<Type> ValuePtr;

public:
    template<typename ValueType, typename FunctionType>
    EngineObjectProperty(const ValueType& value, const std::string& key, FunctionType action)
        : EngineObjectPropertyBase(key)
        , ValuePtr(std::make_shared<Type>(value))
        , Action(std::make_unique<Action_t>(action))
    {
        mEnginePropertyType = ConvertTypeToEnginePropertyType<typename std::decay<Type>::type>::value;
    }

    template<typename ValueType>
    EngineObjectProperty(const ValueType& value, const std::string& key)
        : EngineObjectPropertyBase(key)
        , ValuePtr(std::make_shared<Type>(value))
        , Action()
    {
    }

    std::shared_ptr<Type> GetValuePtr()
    {
        return ValuePtr;
    }

    Type GetValue() const
    {
        return *ValuePtr;
    }

    void SetValue(const Type& value, const bool triggerAction = true)
    {
        *ValuePtr = value;

        if (triggerAction && Action) {
            (*(Action.get()))(value);
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
