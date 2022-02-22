#pragma once

#include <memory>

#include "Core/GameCore/GameObjectPropertyBindings/PropertyBinding.h"

namespace EngineCore
{
   enum class StateType
   {
      SourceState = 0,
      DestinationState = 1
   };

   class ITweenController
   {
   protected:

      struct BaseStateProperty* TranstionProperties[2];

      std::weak_ptr<PropertyBinding> mPropertyBinding;

   public:

      ITweenController();

      virtual void OnTransitionFinished() = 0;

      virtual void OnTransitionStarted(struct BaseStateProperty* srcStateProperty, struct BaseStateProperty* dstStateProperty, const float transitionDuration);

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter);

      virtual void InitWithPropsInstant(struct BaseStateProperty* dstStateProperty);

      enum eBindingType GetControllerPropertyType() const;
   };
}