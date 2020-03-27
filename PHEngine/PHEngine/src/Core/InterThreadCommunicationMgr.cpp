#include "InterThreadCommunicationMgr.h"

#include <iostream>
#include <algorithm>

namespace Thread
{

   InterThreadCommunicationMgr::InterThreadCommunicationMgr()
   {
      m_gameThreadJobs.reserve(ThreadPoolCapacity);
      m_renderThreadJobs.reserve(ThreadPoolCapacity);
   }

   InterThreadCommunicationMgr::~InterThreadCommunicationMgr()
   {
   }

   void InterThreadCommunicationMgr::PushGameThreadJob(const EnqueueJobPolicy policy, const Job& job)
   {
      std::lock_guard<std::mutex> lock(m_gameThreadMutex);
      ProcessPushGameThreadJob(policy, job);
   }

   void InterThreadCommunicationMgr::PushRenderThreadJob(const EnqueueJobPolicy policy, const Job& job)
   {
      std::lock_guard<std::mutex> lock(m_renderThreadMutex);
      ProcessPushRenderThreadJob(policy, job);
   }

   void InterThreadCommunicationMgr::ProcessPushRenderThreadJob(const EnqueueJobPolicy policy, const Job& job)
   {
      ProcessPushJob(policy, job, m_renderThreadJobs);
   }

   void InterThreadCommunicationMgr::ProcessPushGameThreadJob(const EnqueueJobPolicy policy, const Job& job)
   {
      ProcessPushJob(policy, job, m_gameThreadJobs);
   }

   void InterThreadCommunicationMgr::ProcessPushJob(const EnqueueJobPolicy policy, const Job& job, std::vector<Job>& jobs)
   {
      switch (policy)
      {
         case EnqueueJobPolicy::PUSH_ANYWAY:
         {
            jobs.emplace_back(job);
            break;
         }
         case EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH:
         {
            const auto duplicateIt = std::find_if(jobs.begin(), jobs.end(),
               [&](const Job& collectionJob)
            {
               return (collectionJob.GetCreatorObjectId() == job.GetCreatorObjectId() && collectionJob.GetFunctionId() == job.GetFunctionId());
            });

            if (jobs.end() == duplicateIt)
            {
               jobs.emplace_back(job);
            }

            break;
         }
         case EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH:
         {
            const auto duplicateIt = std::find_if(jobs.begin(), jobs.end(),
               [&](const Job& collectionJob)
            {
               return (collectionJob.GetCreatorObjectId() == job.GetCreatorObjectId() && collectionJob.GetFunctionId() == job.GetFunctionId());
            });

            if (jobs.end() == duplicateIt)
            {
               jobs.emplace_back(job);
            }
            else
            {
               *(duplicateIt) = job;
            }

            break;
         }
      }
   }

   void InterThreadCommunicationMgr::SpinGameThreadJobs()
   {
      using Clock_t = std::chrono::high_resolution_clock;

      typename Clock_t::time_point start_time = Clock_t::now();

      std::lock_guard<std::mutex> lock(m_gameThreadMutex);
      while (AreGameJobsAwaiting())
      {
         auto jobIt = m_gameThreadJobs.begin();
         (*jobIt)();
         m_gameThreadJobs.erase(jobIt);
      }
   }

   void InterThreadCommunicationMgr::SpinRenderThreadJobs()
   {
      using Clock_t = std::chrono::high_resolution_clock;

      typename Clock_t::time_point start_time = Clock_t::now();

      std::lock_guard<std::mutex> lock(m_renderThreadMutex);
      while (AreRenderJobsAwaiting())
      {
         auto jobIt = m_renderThreadJobs.begin();
         (*jobIt)();
         m_renderThreadJobs.erase(jobIt);
      }
   }
}
