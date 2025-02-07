#include "Job.h"

namespace Thread {

Job::Job(const int32_t creatorObjectId, const uint64_t functionId, callback_t callback)
    : mCreatorObjectId(creatorObjectId)
    , mFunctionId(functionId)
    , mCallback(callback)
{
}

Job::~Job()
{
}

int32_t Job::GetCreatorObjectId() const
{
    return mCreatorObjectId;
}

uint64_t Job::GetFunctionId() const
{
    return mFunctionId;
}

Job::callback_t Job::GetCallback() const
{
    return mCallback;
}

void Job::operator()() const
{
    mCallback();
}
} // namespace Thread
