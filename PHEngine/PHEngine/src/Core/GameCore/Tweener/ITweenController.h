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

   struct BaseStateProperty;

   class ITweenController
   {
   protected:

      std::shared_ptr<BaseStateProperty> TranstionProperties[2];

      std::weak_ptr<PropertyBinding> mPropertyBinding;

   public:

      ITweenController();

      virtual void OnTransitionFinished() = 0;

      virtual void OnTransitionStarted(const std::shared_ptr<BaseStateProperty>& srcStateProperty, const std::shared_ptr<BaseStateProperty>& dstStateProperty, const float transitionDuration);

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter);

      virtual void InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty);

      enum eBindingType GetControllerPropertyType() const;
   };
}