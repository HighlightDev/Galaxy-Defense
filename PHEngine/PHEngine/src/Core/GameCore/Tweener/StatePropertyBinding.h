#pragma once
#include <string>

namespace Game {

   enum class BindingType
   {
      ANIMATION,
      FLOAT
   };

   struct StatePropertyBinding
   {
      std::string BindingName;
      std::string GameObjectName;
      std::string GameObjectPropertyName;

      StatePropertyBinding(const std::string& bindingName)
         : BindingName(bindingName)
         , GameObjectName("")
      {

      }

      virtual BindingType GetBindingType() const = 0;
   };

   struct FloatPropertyBinding
      : public StatePropertyBinding
   {
      float* Value;

      FloatPropertyBinding(const std::string& bindingName, float* value)
         : StatePropertyBinding(bindingName)
         , Value(value)
      {
      }

      FloatPropertyBinding(const std::string& bindingName)
         : StatePropertyBinding(bindingName)
         , Value(nullptr)
      {
      }

      void SetBindingProperty(float* value)
      {
         Value = value;
      }

      virtual BindingType GetBindingType() const override
      {
         return BindingType::FLOAT;
      }
   };

   struct AnimationPropertyBinding
      : public StatePropertyBinding
   {
      std::string* SrcName;
      std::string* DstName;

      float* SrcTime;
      float* DstTime;

      bool* bTranstitionEnabled;
      float* TransitionValue;

      AnimationPropertyBinding(const std::string& bindingName, std::string* srcName, std::string* dstName,
         float* srcTime, float* dstTime, bool* isTransitionEnabled, float* transitionValue)
         : StatePropertyBinding(bindingName)
         , SrcName(srcName)
         , DstName(dstName)
         , SrcTime(srcTime)
         , DstTime(dstTime)
         , bTranstitionEnabled(isTransitionEnabled)
         , TransitionValue(transitionValue)
      {
      }

      AnimationPropertyBinding(const std::string& bindingName)
         : StatePropertyBinding(bindingName)
         , SrcName(nullptr)
         , DstName(nullptr)
         , SrcTime(nullptr)
         , DstTime(nullptr)
         , bTranstitionEnabled(nullptr)
         , TransitionValue(nullptr)
      {
      }

      void SetBindingProperties(std::string* srcName, std::string* dstName,
         float* srcTime, float* dstTime, bool* isTransitionEnabled, float* transitionValue)
      {
         SrcName = srcName;
         DstName = dstName;
         SrcTime = srcTime;
         DstTime = dstTime;
         bTranstitionEnabled = isTransitionEnabled;
         TransitionValue = transitionValue;
      }

      virtual BindingType GetBindingType() const override
      {
         return BindingType::ANIMATION;
      }
   };
}