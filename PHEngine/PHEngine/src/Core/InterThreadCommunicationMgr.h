#pragma once

#include <vector>
#include <mutex>

#include "Job.h"

namespace Thread
{

#define ENQUEUE_RENDER_THREAD_JOB(InterThreadMgrInstance, Policy, JOB) \
InterThreadMgrInstance.PushRenderThreadJob(Policy, JOB)

#define ENQUEUE_GAME_THREAD_JOB(InterThreadMgrInstance, Policy, JOB) \
InterThreadMgrInstance.PushGameThreadJob(Policy, JOB)

#define SPIN_RENDER_THREAD_JOBS(InterThreadMgrInstance) \
InterThreadMgrInstance.SpinRenderThreadJobs();

#define SPIN_GAME_THREAD_JOBS(InterThreadMgrInstance) \
InterThreadMgrInstance.SpinGameThreadJobs();

   enum class EnqueueJobPolicy
   {
      IF_DUPLICATE_NO_PUSH,
      IF_DUPLICATE_REPLACE_AND_PUSH,
      PUSH_ANYWAY
   };

   class InterThreadCommunicationMgr
   {
      const size_t ThreadPoolCapacity = 80;

      std::mutex m_gameThreadMutex;

      std::mutex m_renderThreadMutex;

      std::vector<Job> m_gameThreadJobs;

      std::vector<Job> m_renderThreadJobs;

   public:

      InterThreadCommunicationMgr();

      ~InterThreadCommunicationMgr();

      void PushGameThreadJob(const EnqueueJobPolicy, const Job& job);

      void PushRenderThreadJob(const EnqueueJobPolicy, const Job& job);

      /* @ Should be executed only on game thread! */
      void SpinGameThreadJobs();

      /* @ Should be executed only on render thread! */
      void SpinRenderThreadJobs();

   private:

      void ProcessPushRenderThreadJob(const EnqueueJobPolicy policy, const Job& job);
      void ProcessPushGameThreadJob(const EnqueueJobPolicy policy, const Job& job);
      void ProcessPushJob(const EnqueueJobPolicy policy, const Job& job, std::vector<Job>& jobs);

      inline bool AreGameJobsAwaiting() const {

         return m_gameThreadJobs.size() > 0;
      }

      inline bool AreRenderJobsAwaiting() const {

         return m_renderThreadJobs.size() > 0;
      }
   };

}

