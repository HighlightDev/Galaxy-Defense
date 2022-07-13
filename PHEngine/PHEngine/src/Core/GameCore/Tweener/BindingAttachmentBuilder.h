#pragma once

#include "Core/GameCore/GameObjectPropertyBindings/AnimationPropertyBinding.h"
#include "Core/GameCore/GameObjectPropertyBindings/FloatPropertyBinding.h"
#include "Core/GameCore/GameObjectPropertyBindings/BooleanPropertyBinding.h"
#include "Core/GameCore/GameObjectPropertyBindings/EulerAnglesRotationPropertyBinding.h"
#include "Core/GameCore/GameObjectPropertyBindings/Vec3PropertyBinding.h"
#include "Core/GameCore/GameObject.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{

   template <typename EnginePropertyValueType>
   std::shared_ptr<EngineGOProperty<EnginePropertyValueType>> CastBasePropertyToType(const std::shared_ptr<EngineGOPropertyBase> &goProperty)
   {
      return std::static_pointer_cast<EngineGOProperty<EnginePropertyValueType>>(goProperty);
   }

   struct BindingAttachmentBuilder
   {
      static void SetAttachment(const GameObject *gameObject, PropertyBinding *binding, const std::string &gameObjectPropertyName)
      {
         binding->GameObjectName = gameObject->GetGameObjectName();
         binding->GameObjectPropertyName = gameObjectPropertyName;

         switch (binding->GetBindingType())
         {
         case eBindingType::Animation:
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
         case eBindingType::FloatScalar:
         {
            const auto floatBinding = static_cast<FloatPropertyBinding *>(binding);
            auto gameObjectProperty = CastBasePropertyToType<float>(gameObject->GetEnginePropertyByName(gameObjectPropertyName));
            floatBinding->SetGameObjectProperty(gameObjectProperty);
            break;
         }
         case eBindingType::EulerAnglesRotation:
         {
            const auto rotationBinding = static_cast<EulerAnglesRotationPropertyBinding *>(binding);
            auto gameObjectProperty = CastBasePropertyToType<glm::vec3>(gameObject->GetEnginePropertyByName(gameObjectPropertyName));
            rotationBinding->SetGameObjectProperty(gameObjectProperty);
            break;
         }
         case eBindingType::Boolean:
         {
            const auto booleanBinding = static_cast<BooleanPropertyBinding*>(binding);
            auto gameObjectProperty = CastBasePropertyToType<bool>(gameObject->GetEnginePropertyByName(gameObjectPropertyName));
            booleanBinding->SetGameObjectProperty(gameObjectProperty);
            break;
         }
         case eBindingType::Vec3:
         {
            const auto vec3Binding = static_cast<EulerAnglesRotationPropertyBinding *>(binding);
            auto gameObjectProperty = CastBasePropertyToType<glm::vec3>(gameObject->GetEnginePropertyByName(gameObjectPropertyName));
            vec3Binding->SetGameObjectProperty(gameObjectProperty);
            break;
         }
         default:
            assert(false); // "unknown binding type."
            break;
         }
      }
   };
}