#pragma once

#include "StatePropertyBinding.h"
#include "Core/GameCore/GameObject.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{
   struct BindingAttachmentBuilder
   {
      static void SetAttachment(GameObject* gameObject, StatePropertyBinding* binding, const std::string& propertyName)
      {
         switch (binding->GetBindingType())
         {
            case BindingType::ANIMATION:
            {
               AnimationPropertyBinding* animationBinding = static_cast<AnimationPropertyBinding*>(binding);
               GenericObjectProperty<float>* propSrcTime = static_cast<GenericObjectProperty<float>*>(gameObject->GetEnginePropertyByName("SrcAnimTime"));
               GenericObjectProperty<float>* propDstTime = static_cast<GenericObjectProperty<float>*>(gameObject->GetEnginePropertyByName("DstAnimTime"));
               GenericObjectProperty<float>* propTransitionValue = static_cast<GenericObjectProperty<float>*>(gameObject->GetEnginePropertyByName("AnimTransitionValue"));
               GenericObjectProperty<bool>* propIsTransition = static_cast<GenericObjectProperty<bool>*>(gameObject->GetEnginePropertyByName("bAnimTransitionEnabled"));
               GenericObjectProperty<std::string>* propSrcName = static_cast<GenericObjectProperty<std::string>*>(gameObject->GetEnginePropertyByName("SrcAnimName"));
               GenericObjectProperty<std::string>* propDstName = static_cast<GenericObjectProperty<std::string>*>(gameObject->GetEnginePropertyByName("DstAnimName"));
               animationBinding->SetBindingProperties(propSrcName->GetValuePtr(), propDstName->GetValuePtr(), propSrcTime->GetValuePtr(),
                  propDstTime->GetValuePtr(), propIsTransition->GetValuePtr(), propTransitionValue->GetValuePtr());
               break;
            }
            case BindingType::FLOAT:
            {
               FloatPropertyBinding* floatBinding = static_cast<FloatPropertyBinding*>(binding);
               GenericObjectProperty<float>* propValue = static_cast<GenericObjectProperty<float>*>(gameObject->GetEnginePropertyByName(propertyName));
               floatBinding->SetBindingProperty(propValue->GetValuePtr());
               break;
            }
            default:
               assert(false); // "unknown binding type."
               break;
         }
      }
   };
}