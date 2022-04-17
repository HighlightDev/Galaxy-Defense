#pragma once

#include "PropertyBinding.h"
#include "Core/GameCore/GameObjectProperty.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{

   struct AnimationPropertyBinding
       : public PropertyBinding
   {
   private:
      std::shared_ptr<EngineGOProperty<std::string>> SrcName;
      std::shared_ptr<EngineGOProperty<std::string>> DstName;

      std::shared_ptr<EngineGOProperty<float>> SrcTime;
      std::shared_ptr<EngineGOProperty<float>> DstTime;

      std::shared_ptr<EngineGOProperty<bool>> bTranstitionEnabled;
      std::shared_ptr<EngineGOProperty<float>> TransitionValue;

   public:
      AnimationPropertyBinding(const std::string &bindingName)
          : PropertyBinding(bindingName),
            SrcName(),
            DstName(),
            SrcTime(),
            DstTime(),
            bTranstitionEnabled(),
            TransitionValue()
      {
      }

      void SetSrcName(const std::string &name)
      {
         assert(bValueSet);
         SrcName->SetValue(name);
      }

      void SetDstName(const std::string &name) const
      {
         assert(bValueSet);
         DstName->SetValue(name);
      }

      void SetSrcTime(const float value)
      {
         assert(bValueSet);
         SrcTime->SetValue(value);
      }

      void SetDstTime(const float value)
      {
         assert(bValueSet);
         DstTime->SetValue(value);
      }

      void SetIsTransitionEnabled(bool bEnabled)
      {
         assert(bValueSet);
         bTranstitionEnabled->SetValue(bEnabled);
      }

      void SetTransitionValue(float transitionValue)
      {
         assert(bValueSet);
         TransitionValue->SetValue(transitionValue);
      }

      std::string GetSrcName() const
      {
         assert(bValueSet);
         return SrcName->GetValue();
      }

      std::string GetDstName() const
      {
         assert(bValueSet);
         return DstName->GetValue();
      }

      float GetSrcTime() const
      {
         assert(bValueSet);

         return SrcTime->GetValue();
      }

      float GetDstTime() const
      {
         assert(bValueSet);
         return DstTime->GetValue();
      }

      bool GetIsTransitionEnabled() const
      {
         assert(bValueSet);
         return bTranstitionEnabled->GetValue();
      }

      float GetTransitionValue() const
      {
         assert(bValueSet);
         return TransitionValue->GetValue();
      }

      void SetBindingProperties(const std::shared_ptr<EngineGOProperty<std::string>> &srcName,
                                const std::shared_ptr<EngineGOProperty<std::string>> &dstName,
                                const std::shared_ptr<EngineGOProperty<float>> &srcTime,
                                const std::shared_ptr<EngineGOProperty<float>> &dstTime,
                                const std::shared_ptr<EngineGOProperty<bool>> &isTransitionEnabled,
                                const std::shared_ptr<EngineGOProperty<float>> &transitionValue)
      {
         SrcName = srcName;
         DstName = dstName;
         SrcTime = srcTime;
         DstTime = dstTime;
         bTranstitionEnabled = isTransitionEnabled;
         TransitionValue = transitionValue;

         bValueSet = true;
      }

      virtual eBindingType GetBindingType() const override
      {
         return eBindingType::Animation;
      }
   };
}