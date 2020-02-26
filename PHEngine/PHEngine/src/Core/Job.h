#pragma once

#include <functional>
#include <stdint.h>

namespace Thread
{

   class Job
   {
      using callback_t = std::function<void(void)>;

      uint64_t mCreatorObjectId;
      uint64_t mFunctionId;
      callback_t mCallback;

   public:

      Job(const uint64_t creatorObjectId, const uint64_t functionId, const callback_t& callback);

      ~Job();

      uint64_t GetCreatorObjectId() const;
      uint64_t GetFunctionId() const;
      callback_t GetCallback() const;

      void operator()() const;
   };

}

