#pragma once

#include <functional>
#include <cstddef>

namespace Thread
{

   class Job
   {
      using callback_t = std::function<void(void)>;

      int32_t mCreatorObjectId;
      uint64_t mFunctionId;
      callback_t mCallback;

   public:

      Job(const int32_t creatorObjectId, const uint64_t functionId, callback_t callback);

      ~Job();

      int32_t GetCreatorObjectId() const;
      uint64_t GetFunctionId() const;
      callback_t GetCallback() const;

      void operator()() const;
   };

}

