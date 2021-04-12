#include "FloatTweenController.h"
#include "Core/UtilityCore/EngineMath.h"

namespace Game
{

   FloatTweenController::FloatTweenController()
      : ITweenController()
   {
   }


   FloatTweenController::~FloatTweenController()
   {
   }

   std::shared_ptr<FloatPropertyBinding> FloatTweenController::GetFloatPropertyBindingSP() const
   {
      std::shared_ptr<FloatPropertyBinding> result(nullptr);

      if (auto baseSp = mPropertyBinding.lock()) {
         result = std::static_pointer_cast<FloatPropertyBinding>(baseSp);
         assert(result);
      }

      return result;
   }

   void FloatTweenController::OnTransitionUpdate(const float deltaTime, const float transitionParameter)
   {
      Base::OnTransitionUpdate(deltaTime, transitionParameter);

      if (auto floatBinding = GetFloatPropertyBindingSP())
      {
         StateProperty<StatePropertyType::Float>* srcFloatProperty = static_cast<StateProperty<StatePropertyType::Float>*>(TranstionProperties[(int)StateType::SourceState]);
         StateProperty<StatePropertyType::Float>* dstFloatProperty = static_cast<StateProperty<StatePropertyType::Float>*>(TranstionProperties[(int)StateType::DestinationState]);

         *floatBinding->Value = EngineMath::LerpFloat(srcFloatProperty->Value, dstFloatProperty->Value, transitionParameter);
      }
   }

   void FloatTweenController::InitWithPropsInstant(struct BaseStateProperty* dstStateProperty)
   {
      mPropertyBinding = dstStateProperty->PropertyBinding;
      if (auto floatBinding = GetFloatPropertyBindingSP())
      {
         StateProperty<StatePropertyType::Float>* dstFloatProperty = static_cast<StateProperty<StatePropertyType::Float>*>(dstStateProperty);

         *floatBinding->Value = dstFloatProperty->Value;
      }
   }

   void FloatTweenController::OnTransitionStarted(
      BaseStateProperty* srcProperty,
      BaseStateProperty* dstProperty,
      const float transitionDuration)
   {
      Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

      if (auto floatBinding = GetFloatPropertyBindingSP())
      {
         StateProperty<StatePropertyType::Float>* srcFloatProperty = static_cast<StateProperty<StatePropertyType::Float>*>(TranstionProperties[(int)StateType::SourceState]);

         *floatBinding->Value = srcFloatProperty->Value;
      }
   }

   void FloatTweenController::OnTransitionFinished()
   {
      if (auto floatBinding = GetFloatPropertyBindingSP())
      {
         StateProperty<StatePropertyType::Float>* dstFloatProperty = static_cast<StateProperty<StatePropertyType::Float>*>(TranstionProperties[(int)StateType::DestinationState]);

         *floatBinding->Value = dstFloatProperty->Value;
      }
   }

}
