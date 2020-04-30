#pragma once

#include <map>

namespace IO {

   // when texture is loading I get uint8*
   // when animation is loading I get AninationData*
   // when mesh is loading I get MeshData*

   struct Resource {

      void* DATA;

      bool bAllocated;

      ~Resource() {

         delete DATA;
      }

   };

   struct ResourceMap {

      class Proxy* PROXY;
      //std::map<std::string, std::future<Resource>> DATA;

      ResourceMap();

      void AllocateAsync(const std::string& key);

      void AllocateSync(const std::string& key);

   private:

      // make sure that ResourceMap is created only on stack
      void* operator new(size_t size);

   };

}
