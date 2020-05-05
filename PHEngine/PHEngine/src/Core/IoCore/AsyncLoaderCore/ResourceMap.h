#pragma once

#include <map>
#include "ResourceLoader.h"

namespace IO {

   struct ResourceMap {

   private:

      TextureResourceLoader textureLoader;

      MeshResourceLoader meshLoader;

      static ResourceMap* mInstance;

   public:

      class AsyncDataProxy* mAsyncDataProxy;

      ~ResourceMap();

      void AllocateAsync(const std::string& key);

      void AllocateSync(const std::string& key);

      void WaitUntilResourcesLoad();

      bool TryGetResource(Resource*& outResource, const std::string& key);

      static ResourceMap* GetInstance()
      {
         if (!mInstance)
            mInstance = new ResourceMap();

         return mInstance;
      }

      static void DeleteInstance()
      {
         delete mInstance;
         mInstance = nullptr;
      }

   private:

      ResourceMap();

   };

}
