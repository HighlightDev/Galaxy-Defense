#pragma once

#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/GameCore/GameObjectPropertyBindings/AnimationPropertyBinding.h"
#include "Core/GameCore/GameObjectPropertyBindings/FloatPropertyBinding.h"
#include "Core/GameCore/GameObjectPropertyBindings/EulerAnglesRotationPropertyBinding.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore
{
   struct BaseStateProperty
   {
      std::weak_ptr<PropertyBinding> Binding;

      virtual eBindingType GetStatePropertyType() const = 0;

      BaseStateProperty(std::weak_ptr<PropertyBinding> propertyBinding)
          : Binding(propertyBinding)
      {
      }
   };

   template <eBindingType bindingType>
   struct StateProperty;

   template <>
   struct StateProperty<eBindingType::Animation>
       : public BaseStateProperty
   {
      std::string AnimationName;

      virtual eBindingType GetStatePropertyType() const override
      {
         return eBindingType::Animation;
      }

      StateProperty(const std::string &animationName, std::shared_ptr<AnimationPropertyBinding> animationPropertyBinding)
          : BaseStateProperty(animationPropertyBinding), AnimationName(animationName)
      {
      }
   };

   template <>
   struct StateProperty<eBindingType::FloatScalar>
       : public BaseStateProperty
   {
      float Value;

      virtual eBindingType GetStatePropertyType() const override
      {
         return eBindingType::FloatScalar;
      }

      StateProperty(float value, std::shared_ptr<FloatPropertyBinding> floatPropertyBinding)
          : BaseStateProperty(floatPropertyBinding), Value(value)
      {
      }
   };

   template <>
   struct StateProperty<eBindingType::EulerAnglesRotation>
       : public BaseStateProperty
   {
      glm::vec3 Value;
      glm::quat QuatValue;

      virtual eBindingType GetStatePropertyType() const override
      {
         return eBindingType::EulerAnglesRotation;
      }

      StateProperty(const glm::vec3 &value, std::shared_ptr<EulerAnglesRotationPropertyBinding> rotationPropertyBinding)
          : BaseStateProperty(rotationPropertyBinding), Value(value), QuatValue()
      {
         ConvertInternalEulerAnglesToQuaternion();
      }

   private:
      void ConvertInternalEulerAnglesToQuaternion()
      {
         glm::mat4 i_matrix(1);

         const glm::mat3 &cameraPitchRotation = glm::rotate(i_matrix, DEG_TO_RAD(Value.x), AXIS_RIGHT);
         const glm::mat3 &cameraYawRotation = glm::rotate(i_matrix, DEG_TO_RAD(Value.y), AXIS_UP);
         const glm::mat3 &cameraRollRotation = glm::rotate(i_matrix, DEG_TO_RAD(Value.z), AXIS_FORWARD);

         glm::mat3 conjugatedRotationMat = i_matrix;
         conjugatedRotationMat *= cameraPitchRotation;
         conjugatedRotationMat *= cameraYawRotation;
         conjugatedRotationMat *= cameraRollRotation;
         
         QuatValue = glm::toQuat(conjugatedRotationMat);
      }
   };

}
