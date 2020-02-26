#pragma once

#include <map>

#include "BoneTransform.h"

namespace Graphics
{
   namespace Animation
   {

      class AnimationFrame
      {
         std::map<double, BoneTransform> m_frames;
         std::string m_boneName;

      public:

         AnimationFrame(const std::string& boneName)
            : m_boneName(boneName)
         {
         }

         void AddFrame(const BoneTransform& boneFrameTransform, double frameTime)
         {
            m_frames.emplace(std::make_pair(frameTime, std::move(boneFrameTransform)));
         }

         std::string GetBoneName() const
         {
            return m_boneName;
         }

         const std::map<double, BoneTransform>& GetFrames() const
         {
            return m_frames;
         }

         std::vector<double> GetFrameTimeIntervals() const
         {
            std::vector<double> intervals;

            for (std::map<double, BoneTransform>::const_iterator cit = m_frames.cbegin(); cit != m_frames.cend(); ++cit) {
               intervals.push_back(cit->first);
            }

            return intervals;
         }

         std::pair<double, double> GetIntervalsOnTimeBoundaries(double time) const
         {
            std::vector<double> allTimeIntervals = GetFrameTimeIntervals();
            std::pair<double, double> resultInterval;

            // In case there is only one state in animation
            if (allTimeIntervals.size() == 1)
            {
               resultInterval = std::move(std::pair<double, double>(allTimeIntervals[0], allTimeIntervals[0]));
            }
            // -FIXME
            else if (time <= 0.000005)
            {
               resultInterval = std::move(std::pair<double, double>(allTimeIntervals[0], allTimeIntervals[1]));
            }
            else
            {
               double next = 0.0, prev = allTimeIntervals[0];
               for (size_t i = 0; i < allTimeIntervals.size(); i++)
               {
                  double reverseTimeInterval = allTimeIntervals[allTimeIntervals.size() - i - 1];
                  double forwardTimeInterval = allTimeIntervals[i];

                  if (reverseTimeInterval > time)
                     next = reverseTimeInterval;

                  if (forwardTimeInterval < time)
                     prev = forwardTimeInterval;
               }
               resultInterval = std::pair<double, double>(next, prev);
            }

            return resultInterval;
         }

         std::pair<double, BoneTransform> GetFrameByTime(double key)
         {
            return std::pair<double, BoneTransform>(key, m_frames[key]);
         }
      };

   }
}
