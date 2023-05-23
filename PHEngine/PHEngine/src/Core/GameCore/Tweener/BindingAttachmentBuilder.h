#pragma once

#include "Core/GameCore/EngineObjectPropertyBindings/AnimationPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/FloatPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/BooleanPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/EulerAnglesRotationPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/Vec3PropertyBinding.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{

   template <typename EnginePropertyValueType>
   std::shared_ptr<EngineObjectProperty<EnginePropertyValueType>> CastBasePropertyToType(const std::shared_ptr<EngineObjectPropertyBase> &goProperty)
   {
      return std::static_pointer_cast<EngineObjectProperty<EnginePropertyValueType>>(goProperty);
   }

   struct BindingAttachmentBuilder
   {
      static void SetAttachment(const EngineObject *gameObject, PropertyBinding *binding, const std::string &gameObjectPropertyName)
      {
         binding->EngineObjectName = gameObject->GetEngineObjectName();
         binding->EngineObjectPropertyName = gameObjectPropertyName;

         switch (binding->GetBindingType())
         {
         case eEnginePropertyBindingType::Animation:
         {
            const auto animationBinding = static_cast<AnimationPropertyBinding *>(binding);
            auto propSrcTime = CastBasePropertyToType<float>(gameObject->GetEnginePropertyByName("SrcAnimTime"));
            auto propDstTime = CastBasePropertyToType<float>(gameObject->GetEnginePropertyByName("DstAnimTime"));
            auto propTransitionValue = CastBasePropertyToType<float>(gameObject->GetEnginePropertyByName("AnimTransitionValue"));
            auto propIsTransition = CastBasePropertyToType<bool>(gameObject->GetEnginePropertyByName("bAnimTransitionEnabled"));
            auto propSrcName = CastBasePropertyToType<std::string>(gameObject->GetEnginePropertyByName("SrcAnimName"));
            auto propDstName = CastBasePropertyToType<std::string>(gameObject->GetEnginePropertyByName("DstAnimName"));

            animationBinding->SetBindingProperties(propSrcName,
                                                   propDstName,
                                                   propSrcTime,
                                                   propDstTime,
                                                   propIsTransition,
                                                   propTransitionValue);
            break;
         }
         case eEnginePropertyBindingType::FloatScalar:
         {
            const auto floatBinding = static_cast<FloatPropertyBinding *>(binding);
            auto gameObjectProperty = CastBasePropertyToType<float>(gameObject->GetEnginePropertyByName(gameObjectPropertyName));
            floatBinding->SetEngineObjectProperty(gameObjectProperty);
            break;
         }
         case eEnginePropertyBindingType::EulerAnglesRotation:
         {
            const auto rotationBinding = static_cast<EulerAnglesRotationPropertyBinding *>(binding);
            auto gameObjectProperty = CastBasePropertyToType<glm::vec3>(gameObject->GetEnginePropertyByName(gameObjectPropertyName));
            rotationBinding->SetEngineObjectProperty(gameObjectProperty);
            break;
         }
         case eEnginePropertyBindingType::Boolean:
         {
            const auto booleanBinding = static_cast<BooleanPropertyBinding*>(binding);
            auto gameObjectProperty = CastBasePropertyToType<bool>(gameObject->GetEnginePropertyByName(gameObjectPropertyName));
            booleanBinding->SetEngineObjectProperty(gameObjectProperty);
            break;
         }
         case eEnginePropertyBindingType::Vec3:
         {
            const auto vec3Binding = static_cast<EulerAnglesRotationPropertyBinding *>(binding);
            auto gameObjectProperty = CastBasePropertyToType<glm::vec3>(gameObject->GetEnginePropertyByName(gameObjectPropertyName));
            vec3Binding->SetEngineObjectProperty(gameObjectProperty);
            break;
         }
         default:
            assert(false); // "unknown binding type."
            break;
         }
      }
   };
}