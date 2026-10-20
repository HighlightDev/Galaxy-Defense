#pragma once

#include "Core/GameCore/EnginePropertyType.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <string>

using namespace EngineMath;

namespace EngineCore {
struct BaseStateProperty {
    std::string BindingName;

    virtual eEnginePropertyBindingType GetStatePropertyType() const = 0;

    explicit BaseStateProperty(const std::string& bindingName)
        : BindingName(bindingName)
    {
    }

    virtual ~BaseStateProperty() = default;
};

template<eEnginePropertyBindingType bindingType>
struct StateProperty;

template<>
struct StateProperty<eEnginePropertyBindingType::Animation> : public BaseStateProperty {
    std::string AnimationName;

    eEnginePropertyBindingType GetStatePropertyType() const override
    {
        return eEnginePropertyBindingType::Animation;
    }

    StateProperty(const std::string& animationName, const std::string& bindingName)
        : BaseStateProperty(bindingName)
        , AnimationName(animationName)
    {
    }
};

template<>
struct StateProperty<eEnginePropertyBindingType::FloatScalar> : public BaseStateProperty {
    float Value;

    eEnginePropertyBindingType GetStatePropertyType() const override
    {
        return eEnginePropertyBindingType::FloatScalar;
    }

    StateProperty(const float value, const std::string& bindingName)
        : BaseStateProperty(bindingName)
        , Value(value)
    {
    }
};

template<>
struct StateProperty<eEnginePropertyBindingType::EulerAnglesRotation> : public BaseStateProperty {
    glm::vec3 Value;
    glm::quat QuatValue;

    eEnginePropertyBindingType GetStatePropertyType() const override
    {
        return eEnginePropertyBindingType::EulerAnglesRotation;
    }

    StateProperty(const glm::vec3& value, const std::string& bindingName)
        : BaseStateProperty(bindingName)
        , Value(value)
        , QuatValue()
    {
        ConvertInternalEulerAnglesToQuaternion();
    }

    void SetValue(const glm::vec3& value)
    {
        Value = value;
        ConvertInternalEulerAnglesToQuaternion();
    }

private:
    void ConvertInternalEulerAnglesToQuaternion()
    {
        glm::mat4 i_matrix(1);

        const glm::mat3& cameraPitchRotation = glm::rotate(i_matrix, DEG_TO_RAD(Value.x), AXIS_RIGHT);
        const glm::mat3& cameraYawRotation = glm::rotate(i_matrix, DEG_TO_RAD(Value.y), AXIS_UP);
        const glm::mat3& cameraRollRotation = glm::rotate(i_matrix, DEG_TO_RAD(Value.z), AXIS_FORWARD);

        glm::mat3 conjugatedRotationMat = i_matrix;
        conjugatedRotationMat *= cameraPitchRotation;
        conjugatedRotationMat *= cameraYawRotation;
        conjugatedRotationMat *= cameraRollRotation;

        QuatValue = glm::toQuat(conjugatedRotationMat);
    }
};

template<>
struct StateProperty<eEnginePropertyBindingType::Boolean> : public BaseStateProperty {
    bool Value;

    eEnginePropertyBindingType GetStatePropertyType() const override
    {
        return eEnginePropertyBindingType::Boolean;
    }

    StateProperty(const bool value, const std::string& bindingName)
        : BaseStateProperty(bindingName)
        , Value(value)
    {
    }
};

template<>
struct StateProperty<eEnginePropertyBindingType::Vec3> : public BaseStateProperty {
    glm::vec3 Value;

    eEnginePropertyBindingType GetStatePropertyType() const override
    {
        return eEnginePropertyBindingType::Vec3;
    }

    StateProperty(const glm::vec3& value, const std::string& bindingName)
        : BaseStateProperty(bindingName)
        , Value(value)
    {
    }
};

template<>
struct StateProperty<eEnginePropertyBindingType::Vec3QuadraticBezier> : public BaseStateProperty {
    glm::vec3 Value;
    glm::vec3 ControlPoint;

    eEnginePropertyBindingType GetStatePropertyType() const override
    {
        return eEnginePropertyBindingType::Vec3QuadraticBezier;
    }

    StateProperty(
        const glm::vec3& value,
        const glm::vec3& controlPoint,
        const std::string& bindingName)
        : BaseStateProperty(bindingName)
        , Value(value)
        , ControlPoint(controlPoint)
    {
    }
};

} // namespace EngineCore
