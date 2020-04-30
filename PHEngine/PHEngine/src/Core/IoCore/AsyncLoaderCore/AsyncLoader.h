#pragma once

#include <string>

namespace IO
{

   struct Resource;

   class AsyncLoader
   {
    
   public:

      AsyncLoader();

      virtual Resource DoAsyncJob(const std::string& key) = 0;
   };

   class TextureAsyncLoader
      : public AsyncLoader
   {
   public:

      TextureAsyncLoader();

      virtual Resource DoAsyncJob(const std::string& key);
   };

   class MeshAsyncLoader
      : public AsyncLoader
   {
   public:

      MeshAsyncLoader();

      virtual Resource DoAsyncJob(const std::string& key);
   };

   class MeshAnimationAsyncLoader
      : public AsyncLoader 
   {
      MeshAnimationAsyncLoader();

      virtual Resource DoAsyncJob(const std::string& key);
   };

}

