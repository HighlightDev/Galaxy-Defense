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
    uint64_t mHash;

    static uint64_t HashCombine(uint64_t seed, uint64_t value)
    {
        seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 12) + (seed >> 4);
        return seed;
    }

public:
    // Empty job: required so the fixed-size std::array job buffers can be default-constructed and cleared. Slots holding
    // an empty job are never invoked (the swap chain only runs slots below its step index).
    Job()
        : mCreatorObjectId(0)
        , mFunctionId(0)
        , mCallback(nullptr)
        , mHash(0)
    {
    }

    Job(const int32_t creatorObjectId, const uint64_t functionId, callback_t callback)
        : mCreatorObjectId(creatorObjectId)
        , mFunctionId(functionId)
        , mCallback(callback)
        , mHash(HashCombine(static_cast<uint64_t>(creatorObjectId), functionId))
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

    uint64_t GetHash() const
    {
        return mHash;
    }
};

} // namespace Thread
