#include "FloatTweenController.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore
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
         StateProperty<eBindingType::FloatScalar>* srcFloatProperty = static_cast<StateProperty<eBindingType::FloatScalar>*>(TranstionProperties[(int)StateType::SourceState]);
         StateProperty<eBindingType::FloatScalar>* dstFloatProperty = static_cast<StateProperty<eBindingType::FloatScalar>*>(TranstionProperties[(int)StateType::DestinationState]);

         floatBinding->SetValue(EngineMath::LerpNormalizedFloat(srcFloatProperty->Value, dstFloatProperty->Value, transitionParameter));
      }
   }

   void FloatTweenController::InitWithPropsInstant(struct BaseStateProperty* dstStateProperty)
   {
      mPropertyBinding = dstStateProperty->Binding;
      if (auto floatBinding = GetFloatPropertyBindingSP())
      {
         StateProperty<eBindingType::FloatScalar>* dstFloatProperty = static_cast<StateProperty<eBindingType::FloatScalar>*>(dstStateProperty);

         floatBinding->SetValue(dstFloatProperty->Value);
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
         StateProperty<eBindingType::FloatScalar>* srcFloatProperty = static_cast<StateProperty<eBindingType::FloatScalar>*>(TranstionProperties[(int)StateType::SourceState]);

         floatBinding->SetValue(srcFloatProperty->Value);
      }
   }

   void FloatTweenController::OnTransitionFinished()
   {
      if (auto floatBinding = GetFloatPropertyBindingSP())
      {
         StateProperty<eBindingType::FloatScalar>* dstFloatProperty = static_cast<StateProperty<eBindingType::FloatScalar>*>(TranstionProperties[(int)StateType::DestinationState]);

         floatBinding->SetValue(dstFloatProperty->Value);
      }
   }

}
