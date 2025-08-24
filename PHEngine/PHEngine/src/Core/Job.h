#pragma once

#include <stdint.h>

#include <cstddef>
#include <functional>

namespace Thread {

template<typename... JobArgumentsT>
class Job {
    using callback_t = std::function<void(JobArgumentsT...)>;

    int32_t mCreatorObjectId;
    uint64_t mFunctionId;
    callback_t mCallback;

public:
    Job(const int32_t creatorObjectId, const uint64_t functionId, callback_t callback)
        : mCreatorObjectId(creatorObjectId)
        , mFunctionId(functionId)
        , mCallback(callback)
    {
    }

    int32_t GetCreatorObjectId() const
    {
        return mCreatorObjectId;
    }

    uint64_t GetFunctionId() const
    {
        return mFunctionId;
    }

    Job::callback_t GetCallback() const
    {
        return mCallback;
    }

    template<typename... ArgumentsT>
    void operator()(ArgumentsT&&... arguments) const
    {
        mCallback(std::forward<ArgumentsT>(arguments)...);
    }
};

} // namespace Thread
