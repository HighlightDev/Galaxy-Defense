#pragma once

#include <string>

namespace IO
{

   struct Resource;

   class ResourceLoader
   {
    
   public:

      ResourceLoader();

      virtual Resource* LoadResource(const std::string& key) = 0;
   };

   class TextureResourceLoader
      : public ResourceLoader
   {
   public:

      TextureResourceLoader();

      virtual Resource* LoadResource(const std::string& key) override;
   };

   class MeshResourceLoader
      : public ResourceLoader
   {
   public:

      MeshResourceLoader();

      virtual Resource* LoadResource(const std::string& key) override;
   };

   class AudioResourceLoader
   : public ResourceLoader
   {
      public:

       AudioResourceLoader();

       virtual Resource* LoadResource(const std::string& key);
   };

}

