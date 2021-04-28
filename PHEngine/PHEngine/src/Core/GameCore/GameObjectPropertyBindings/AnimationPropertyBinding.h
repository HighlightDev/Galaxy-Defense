#pragma once

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"

namespace Game {

   struct AnimationPropertyBinding
      : public PropertyBinding
   {
   private:

      propertyPtr_t<std::string> SrcName;
      propertyPtr_t<std::string> DstName;

      propertyPtr_t<float> SrcTime;
      propertyPtr_t<float> DstTime;

      propertyPtr_t<bool> bTranstitionEnabled;
      propertyPtr_t<float> TransitionValue;

   public:

      AnimationPropertyBinding(const std::string& bindingName, propertyPtr_t<std::string> srcName, propertyPtr_t<std::string> dstName,
         propertyPtr_t<float> srcTime, propertyPtr_t<float> dstTime, propertyPtr_t<bool> isTransitionEnabled, propertyPtr_t<float> transitionValue)
         : PropertyBinding(bindingName)
         , SrcName(srcName)
         , DstName(dstName)
         , SrcTime(srcTime)
         , DstTime(dstTime)
         , bTranstitionEnabled(isTransitionEnabled)
         , TransitionValue(transitionValue)
      {
      }

      AnimationPropertyBinding(const std::string& bindingName)
         : PropertyBinding(bindingName)
         , SrcName(nullptr)
         , DstName(nullptr)
         , SrcTime(nullptr)
         , DstTime(nullptr)
         , bTranstitionEnabled(nullptr)
         , TransitionValue(nullptr)
      {
      }

      void SetSrcName(const std::string& name) {
         *SrcName = name;
      }

      void SetDstName(const std::string& name) const {
         *DstName = name;
      }

      void SetSrcTime(const float value) {
         *SrcTime = value;
      }

      void SetDstTime(const float value) {
         *DstTime = value;
      }

      void SetIsTransitionEnabled(bool bEnabled) {
         *bTranstitionEnabled = bEnabled;
      }

      void SetTransitionValue(float transitionValue) {
         *TransitionValue = transitionValue;
      }

      std::string GetSrcName() const {
         assert(bValueSet);
         return *SrcName;
      }

      std::string GetDstName() const {
         assert(bValueSet);
         return *DstName;
      }

      float GetSrcTime() const {
         assert(bValueSet);

         return *SrcTime;
      }

      float GetDstTime() const {
         assert(bValueSet);

         return *DstTime;
      }

      bool GetIsTransitionEnabled() const {
         assert(bValueSet);
         return *bTranstitionEnabled;
      }

      float GetTransitionValue() const {
         assert(bValueSet);
         return *TransitionValue;
      }

      void SetBindingProperties(
         propertyPtr_t<std::string> srcName,
         propertyPtr_t<std::string> dstName,
         propertyPtr_t<float> srcTime,
         propertyPtr_t<float> dstTime,
         propertyPtr_t<bool> isTransitionEnabled,
         propertyPtr_t<float> transitionValue)
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
         return eBindingType::ANIMATION;
      }
   };
}