#pragma once

#include "PropertyBinding.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{

   struct AnimationPropertyBinding
       : public PropertyBinding
   {
   private:
      std::shared_ptr<EngineObjectProperty<std::string>> SrcName;
      std::shared_ptr<EngineObjectProperty<std::string>> DstName;

      std::shared_ptr<EngineObjectProperty<float>> SrcTime;
      std::shared_ptr<EngineObjectProperty<float>> DstTime;

      std::shared_ptr<EngineObjectProperty<bool>> bTranstitionEnabled;
      std::shared_ptr<EngineObjectProperty<float>> TransitionValue;

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
         assert(bPropertyConnected);
         SrcName->SetValue(name);
      }

      void SetDstName(const std::string &name) const
      {
         assert(bPropertyConnected);
         DstName->SetValue(name);
      }

      void SetSrcTime(const float value)
      {
         assert(bPropertyConnected);
         SrcTime->SetValue(value);
      }

      void SetDstTime(const float value)
      {
         assert(bPropertyConnected);
         DstTime->SetValue(value);
      }

      void SetIsTransitionEnabled(bool bEnabled)
      {
         assert(bPropertyConnected);
         bTranstitionEnabled->SetValue(bEnabled);
      }

      void SetTransitionValue(float transitionValue)
      {
         assert(bPropertyConnected);
         TransitionValue->SetValue(transitionValue);
      }

      std::string GetSrcName() const
      {
         assert(bPropertyConnected);
         return SrcName->GetValue();
      }

      std::string GetDstName() const
      {
         assert(bPropertyConnected);
         return DstName->GetValue();
      }

      float GetSrcTime() const
      {
         assert(bPropertyConnected);

         return SrcTime->GetValue();
      }

      float GetDstTime() const
      {
         assert(bPropertyConnected);
         return DstTime->GetValue();
      }

      bool GetIsTransitionEnabled() const
      {
         assert(bPropertyConnected);
         return bTranstitionEnabled->GetValue();
      }

      float GetTransitionValue() const
      {
         assert(bPropertyConnected);
         return TransitionValue->GetValue();
      }

      void SetBindingProperties(const std::shared_ptr<EngineObjectProperty<std::string>> &srcName,
                                const std::shared_ptr<EngineObjectProperty<std::string>> &dstName,
                                const std::shared_ptr<EngineObjectProperty<float>> &srcTime,
                                const std::shared_ptr<EngineObjectProperty<float>> &dstTime,
                                const std::shared_ptr<EngineObjectProperty<bool>> &isTransitionEnabled,
                                const std::shared_ptr<EngineObjectProperty<float>> &transitionValue)
      {
         SrcName = srcName;
         DstName = dstName;
         SrcTime = srcTime;
         DstTime = dstTime;
         bTranstitionEnabled = isTransitionEnabled;
         TransitionValue = transitionValue;

         bPropertyConnected = true;
      }

      eEnginePropertyBindingType GetBindingType() const override
      {
         return eEnginePropertyBindingType::Animation;
      }
   };
}