#pragma once

#include "ResourceLoader.h"

#include <map>
#include <memory>

namespace IO
{
   class AsyncDataProxy;

   struct ResourceMap
   {

   private:
      TextureResourceLoader mTextureLoader;

      MeshResourceLoader mMeshLoader;

      AudioResourceLoader mAudioLoader;

      std::map<std::string, Resource *> ReadyToReadResources;

   public:
      std::unique_ptr<AsyncDataProxy> mAsyncDataProxy;

      ~ResourceMap();

      void AllocateAsync(const std::string &key);

      void AllocateSync(const std::string &key);

      void WaitUntilResourcesLoad();

      bool TryGetResource(Resource *&outResource, const std::string &key);

      void SaveToPool(const std::string &key);

      static ResourceMap *GetInstance();

      void CleanUp();

   private:
      ResourceMap();
   };

}
