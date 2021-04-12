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
         binding->GameObjectName = gameObject->GetGameObjectName();
         binding->GameObjectPropertyName = propertyName;

         switch (binding->GetBindingType())
         {
            case BindingType::ANIMATION:
            {
               AnimationPropertyBinding* animationBinding = static_cast<AnimationPropertyBinding*>(binding);
               EngineGOProperty<float>* propSrcTime = static_cast<EngineGOProperty<float>*>(gameObject->GetEnginePropertyByName("SrcAnimTime"));
               EngineGOProperty<float>* propDstTime = static_cast<EngineGOProperty<float>*>(gameObject->GetEnginePropertyByName("DstAnimTime"));
               EngineGOProperty<float>* propTransitionValue = static_cast<EngineGOProperty<float>*>(gameObject->GetEnginePropertyByName("AnimTransitionValue"));
               EngineGOProperty<bool>* propIsTransition = static_cast<EngineGOProperty<bool>*>(gameObject->GetEnginePropertyByName("bAnimTransitionEnabled"));
               EngineGOProperty<std::string>* propSrcName = static_cast<EngineGOProperty<std::string>*>(gameObject->GetEnginePropertyByName("SrcAnimName"));
               EngineGOProperty<std::string>* propDstName = static_cast<EngineGOProperty<std::string>*>(gameObject->GetEnginePropertyByName("DstAnimName"));
               animationBinding->SetBindingProperties(propSrcName->GetValuePtr(), propDstName->GetValuePtr(), propSrcTime->GetValuePtr(),
                  propDstTime->GetValuePtr(), propIsTransition->GetValuePtr(), propTransitionValue->GetValuePtr());
               break;
            }
            case BindingType::FLOAT:
            {
               FloatPropertyBinding* floatBinding = static_cast<FloatPropertyBinding*>(binding);
               EngineGOProperty<float>* propValue = static_cast<EngineGOProperty<float>*>(gameObject->GetEnginePropertyByName(propertyName));
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