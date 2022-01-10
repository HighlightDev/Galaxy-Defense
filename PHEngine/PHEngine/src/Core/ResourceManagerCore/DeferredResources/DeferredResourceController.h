#pragma once
#include "DeferredResource.h"
#include "Core/CommonCore/Assertion.h"

namespace Resources
{
   template <typename TResource, eResourceType resourceType>
   struct DeferredResourceController
   {
      using DeferredResource_t = IDeferredResource<TResource, resourceType>;

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
         if (!mDeferredResource->GetIsFutureInitialized())
         {
            mDeferredResource->Initialize(mPromise.get_future());
         }

         return mDeferredResource;
      }

      bool IsValueSet() const {
         return bValueSet;
      }

      void SetResource(TResource const& resource) {
         assert(mDeferredResource);
         assert(mDeferredResource->GetIsFutureInitialized());
         mPromise.set_value(resource);
         bValueSet = true;
      }
   };
}