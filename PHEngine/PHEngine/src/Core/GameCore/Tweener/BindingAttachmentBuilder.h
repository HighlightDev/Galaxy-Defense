#pragma once

#include "Core/GameCore/GameObjectPropertyBindings/AnimationPropertyBinding.h"
#include "Core/GameCore/GameObjectPropertyBindings/FloatPropertyBinding.h"
#include "Core/GameCore/GameObject.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{

   template <typename EnginePropertyValueType>
   EngineGOProperty<EnginePropertyValueType>* CastBasePropertyToType(EngineGOPropertyBase* goProperty)
   {
      return static_cast<EngineGOProperty<EnginePropertyValueType>*>(goProperty);
   }

   struct BindingAttachmentBuilder
   {
      static void SetAttachment(const GameObject* gameObject, PropertyBinding* binding, const std::string& gameObjectPropertyName)
      {
         binding->GameObjectName = gameObject->GetGameObjectName();
         binding->GameObjectPropertyName = gameObjectPropertyName;

         switch (binding->GetBindingType())
         {
            case eBindingType::ANIMATION:
            {
               AnimationPropertyBinding* animationBinding = static_cast<AnimationPropertyBinding*>(binding);
               auto propSrcTime = CastBasePropertyToType<float>(gameObject->GetEnginePropertyByName("SrcAnimTime"));
               auto propDstTime = CastBasePropertyToType<float>(gameObject->GetEnginePropertyByName("DstAnimTime"));
               auto propTransitionValue = CastBasePropertyToType<float>(gameObject->GetEnginePropertyByName("AnimTransitionValue"));
               auto propIsTransition = CastBasePropertyToType<bool>(gameObject->GetEnginePropertyByName("bAnimTransitionEnabled"));
               auto propSrcName = CastBasePropertyToType<std::string>(gameObject->GetEnginePropertyByName("SrcAnimName"));
               auto propDstName = CastBasePropertyToType<std::string>(gameObject->GetEnginePropertyByName("DstAnimName"));
               animationBinding->SetBindingProperties(propSrcName->GetValuePtr(), propDstName->GetValuePtr(), propSrcTime->GetValuePtr(),
                  propDstTime->GetValuePtr(), propIsTransition->GetValuePtr(), propTransitionValue->GetValuePtr());
               break;
            }
            case eBindingType::FLOAT:
            {
               FloatPropertyBinding* floatBinding = static_cast<FloatPropertyBinding*>(binding);
               auto propValue = CastBasePropertyToType<float>(gameObject->GetEnginePropertyByName(gameObjectPropertyName));
               floatBinding->SetValuePtr(propValue->GetValuePtr());
               break;
            }
            default:
               assert(false); // "unknown binding type."
               break;
         }
      }
   };
}