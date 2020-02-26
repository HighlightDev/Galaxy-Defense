#pragma once

#include <string>
#include <vector>
#include <stdint.h>

#include "AnimationFrame.h"

namespace Graphics
{
   namespace Animation
   {

      class AnimationSequence
      {

         std::string m_Name;
         std::vector<AnimationFrame> m_Frames;
         double m_sequenceDurationInSec;

      public:

         AnimationSequence(const std::string& animationName, const std::vector<AnimationFrame>& frames, double sequenceDurationInSec)
            : m_Name(animationName)
            , m_Frames(frames)
            , m_sequenceDurationInSec(sequenceDurationInSec)
         {

         }

         std::string GetName() const {
            return m_Name;
         }

         size_t GetBonesCount() const {
            return m_Frames.size();
         }

         std::pair<std::vector<std::pair<double, BoneTransform>>, std::vector<std::pair<double, BoneTransform>>> GetNextAndPrevFrames(double animationLoopTime)
         {
            std::pair<std::vector<std::pair<double, BoneTransform>>, std::vector<std::pair<double, BoneTransform>>> result;

            using frame_t = std::vector<std::pair<double, BoneTransform>>;

            frame_t nextFrames;
            frame_t prevFrames;

            for (auto& animationFrame : m_Frames)
            {
               std::pair<double, double> interval_max_min = animationFrame.GetIntervalsOnTimeBoundaries(animationLoopTime);
               const auto& nextFrame = animationFrame.GetFrameByTime(interval_max_min.first);
               const auto& prevFrame = animationFrame.GetFrameByTime(interval_max_min.second);
               nextFrames.push_back(nextFrame);
               prevFrames.push_back(prevFrame);
            }

            result = std::make_pair<frame_t, frame_t>(std::move(nextFrames), std::move(prevFrames));
            return result;
         }

         std::vector<AnimationFrame>& GetAnimationFrames()
         {
            return m_Frames;
         }

         double GetAnimationDurationInSec() const
         {
            return m_sequenceDurationInSec;
         }

         void SetAnimationDurationInSec(double sequenceDurationInSec)
         {
            m_sequenceDurationInSec = sequenceDurationInSec;
         }

         void SetAnimationFrames(const std::vector<AnimationFrame>& frames)
         {
            m_Frames = frames;
         }

         void AddAnimationFrame(AnimationFrame& frame)
         {
            m_Frames.push_back(frame);
         }

      };
   }
}