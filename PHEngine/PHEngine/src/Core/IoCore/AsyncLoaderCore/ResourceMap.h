#pragma once

#include <map>
#include "ResourceLoader.h"

namespace IO {

   struct ResourceMap {

      class AsyncDataProxy* mAsyncDataProxy;

      TextureResourceLoader textureLoader;

      MeshResourceLoader meshLoader;

      ResourceMap();

      void AllocateAsync(const std::string& key);

      void AllocateSync(const std::string& key);

      void WaitUntilResourcesLoad();

      ~ResourceMap();

   private:

      // make sure that ResourceMap is created only on stack
      void* operator new(size_t size);

   };

}
