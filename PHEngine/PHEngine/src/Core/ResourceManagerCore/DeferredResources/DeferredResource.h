#pragma once

#include <future>
#include <memory>

namespace Resources
{
   enum class eResourceType
   {
      NONE,
      TEXTURE,
      FLOAT
   };

   struct IDeferredResourceBase
   {
      virtual eResourceType GetResourceType() const = 0;
   };

   template <typename TResource, eResourceType resourceType>
   struct IDeferredResource 
      : public IDeferredResourceBase
   {
      using arg_t = TResource;

   private:

      std::shared_future<arg_t> mResourceFuture;

      bool bIsFutureInitialized;

      arg_t mResource;

      bool bResourceSaved;

   public:

      IDeferredResource()
         : bIsFutureInitialized(false)
         , bResourceSaved(false)
      {
      }

      virtual ~IDeferredResource() {

      }

      void Initialize(std::shared_future<arg_t> sharedFuture)
      {
         mResourceFuture = sharedFuture;
         bIsFutureInitialized = true;
      }

      bool GetIsFutureInitialized() const {
         return bIsFutureInitialized;
      }

      arg_t GetResource()
      {
         if (!bResourceSaved)
         {
            mResource = mResourceFuture.get();
            bResourceSaved = true;
         }

         return mResource;
      }

      bool TryGetResource(arg_t& out)
      {
         bool bSuccess = false;

         if (bResourceSaved)
         {
            out = mResource;
            bSuccess = true;
         }
         else if (IsReady())
         {
            out = mResourceFuture.get();
            mResource = out;
            bSuccess = true;
            bResourceSaved = true;
         }
        

         return bSuccess;
      }

      bool IsReady() {
         return mResourceFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
      }

      virtual eResourceType GetResourceType() const override
      {
         return resourceType;
      }

   };
}