#pragma once
#include "Core/CommonCore/Assertion.h"
#include "DeferredResource.h"

namespace Resources {
template<typename TResource, eDeferredResourceType resourceType>
struct DeferredResourceController {
    using DeferredResource_t = DeferredResource<TResource, resourceType>;

private:
    std::promise<TResource> mPromise;

    std::shared_ptr<DeferredResource_t> mDeferredResource;

    bool bValueSet;

public:
    DeferredResourceController()
        : mDeferredResource(std::make_shared<DeferredResource_t>())
        , bValueSet(false)
    {
    }

    virtual ~DeferredResourceController()
    {
    }

    std::shared_ptr<DeferredResource_t> GetDeferredResource()
    {
        if (!mDeferredResource->GetIsFutureInitialized()) {
            mDeferredResource->Initialize(mPromise.get_future());
        }

        return mDeferredResource;
    }

    bool IsValueSet() const
    {
        return bValueSet;
    }

    void SetResource(TResource const& resource)
    {
        ext_assert(mDeferredResource, "DeferredResource is null in DeferredResourceController::SetResource");
        ext_assert(
            mDeferredResource->GetIsFutureInitialized(), "Future is not initialized in DeferredResourceController::SetResource");
        mPromise.set_value(resource);
        bValueSet = true;
    }
};
} // namespace Resources