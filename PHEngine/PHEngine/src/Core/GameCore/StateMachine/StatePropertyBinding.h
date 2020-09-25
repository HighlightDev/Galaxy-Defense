#pragma once
#include <string>

namespace Game {

   struct StatePropertyBinding
   {
      std::string MutualName;
      StatePropertyBinding(const std::string& mutualName)
         : MutualName(mutualName)
      {

      }
   };

   struct AnimationPropertyBinding
      : public StatePropertyBinding
   {
      std::string& SrcName;
      std::string& DstName;

      float& SrcTime;
      float& DstTime;

      bool& bTranstitionEnabled;
      float& TransitionValue;

      AnimationPropertyBinding(const std::string& mutualName, std::string& srcName, std::string& dstName, float& srcTime, float& dstTime, bool& isTransitionEnabled, float& transitionValue)
         : StatePropertyBinding(mutualName)
         , SrcName(srcName)
         , DstName(dstName)
         , SrcTime(srcTime)
         , DstTime(dstTime)
         , bTranstitionEnabled(isTransitionEnabled)
         , TransitionValue(transitionValue)
      {
      }
   };
}