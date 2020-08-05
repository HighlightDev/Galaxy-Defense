#pragma once
#include <string>

namespace Game {

   struct StatePropertyBinding
   {

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

      AnimationPropertyBinding(std::string& srcName, std::string& dstName, float& srcTime, float& dstTime, bool& isTransitionEnabled, float& transitionValue)
         : SrcName(srcName)
         , DstName(dstName)
         , SrcTime(srcTime)
         , DstTime(dstTime)
         , bTranstitionEnabled(isTransitionEnabled)
         , TransitionValue(transitionValue)
      {
      }
   };
}