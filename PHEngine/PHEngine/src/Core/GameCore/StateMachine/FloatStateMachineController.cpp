#include "FloatStateMachineController.h"
#include "Core/UtilityCore/EngineMath.h"

namespace Game
{

   FloatStateMachineController::FloatStateMachineController()
      : IStateMachineController()
   {
   }


   FloatStateMachineController::~FloatStateMachineController()
   {
   }

   std::shared_ptr<FloatPropertyBinding> FloatStateMachineController::GetFloatPropertyBindingSP() const
   {
      std::shared_ptr<FloatPropertyBinding> result(nullptr);

      if (auto baseSp = mPropertyBinding.lock()) {
         result = std::static_pointer_cast<FloatPropertyBinding>(baseSp);
         assert(result);
      }

      return result;
   }

   void FloatStateMachineController::OnTransitionUpdate(const float deltaTime, const float transitionParameter)
   {
      Base::OnTransitionUpdate(deltaTime, transitionParameter);

      if (auto floatBinding = GetFloatPropertyBindingSP())
      {
         StateProperty<StatePropertyType::Float>* srcFloatProperty = static_cast<StateProperty<StatePropertyType::Float>*>(TranstionProperties[(int)StateType::SourceState]);
         StateProperty<StatePropertyType::Float>* dstFloatProperty = static_cast<StateProperty<StatePropertyType::Float>*>(TranstionProperties[(int)StateType::DestinationState]);

         *floatBinding->Value = EngineMath::LerpFloat(srcFloatProperty->Value, dstFloatProperty->Value, transitionParameter);
      }
   }

   void FloatStateMachineController::InitWithPropsInstant(struct BaseStateProperty* dstStateProperty)
   {
      mPropertyBinding = dstStateProperty->PropertyBinding;
      if (auto floatBinding = GetFloatPropertyBindingSP())
      {
         StateProperty<StatePropertyType::Float>* dstFloatProperty = static_cast<StateProperty<StatePropertyType::Float>*>(dstStateProperty);

         *floatBinding->Value = dstFloatProperty->Value;
      }
   }

   void FloatStateMachineController::OnTransitionStarted(
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

   void FloatStateMachineController::OnTransitionFinished()
   {
      if (auto floatBinding = GetFloatPropertyBindingSP())
      {
         StateProperty<StatePropertyType::Float>* dstFloatProperty = static_cast<StateProperty<StatePropertyType::Float>*>(TranstionProperties[(int)StateType::DestinationState]);

         *floatBinding->Value = dstFloatProperty->Value;
      }
   }

}
