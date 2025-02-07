#pragma once

#include <future>
#include <memory>

namespace Resources {
enum class eDeferredResourceType { NONE, TEXTURE, FLOAT };

struct IDeferredResource {
    virtual eDeferredResourceType GetResourceType() const = 0;
};

template<typename TResource, eDeferredResourceType resourceType>
struct DeferredResource : public IDeferredResource {
    using arg_t = TResource;

private:
    std::shared_future<arg_t> mResourceFuture;

    bool bIsFutureInitialized;

    arg_t mResource;

    bool bResourceSaved;

public:
    DeferredResource()
        : bIsFutureInitialized(false)
        , bResourceSaved(false)
    {
    }

    virtual ~DeferredResource()
    {
    }

    void Initialize(std::shared_future<arg_t> sharedFuture)
    {
        mResourceFuture = sharedFuture;
        bIsFutureInitialized = true;
    }

    bool GetIsFutureInitialized() const
    {
        return bIsFutureInitialized;
    }

    arg_t GetResource()
    {
        if (!bResourceSaved) {
            mResource = mResourceFuture.get();
            bResourceSaved = true;
        }

        return mResource;
    }

    bool TryGetResource(arg_t& out)
    {
        bool bSuccess = false;

        if (bResourceSaved) {
            out = mResource;
            bSuccess = true;
        } else if (IsReady()) {
            out = mResourceFuture.get();
            mResource = out;
            bSuccess = true;
            bResourceSaved = true;
        }

        return bSuccess;
    }

    bool IsReady()
    {
        return mResourceFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    eDeferredResourceType GetResourceType() const override
    {
        return resourceType;
    }
};
} // namespace Resources