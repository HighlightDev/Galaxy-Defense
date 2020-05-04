#pragma once

#include "ResourceExtensionsInfo.h"
#include "Core/IoCore/TextureLoaderCore/TextureResourceInfo.h"

namespace IO
{
   // when texture is loading I get uint8*
   // when animation is loading I get AninationData*
   // when mesh is loading I get MeshData*

   struct Resource {

      RESOURCE_TYPE ResourceType = RESOURCE_TYPE::UNDEFINED_TYPE;

      void* DATA;

      void Clear()
      {
         delete DATA;
      }

   };

   struct TextureResource 
      : public Resource
   {
      TextureResourceInfo TexInfo;

      TextureResource()
         : Resource()
      {
         ResourceType = RESOURCE_TYPE::TEXTURE;
      }
   };

   struct MeshResource
      : public Resource
   {
      MeshResource()
         : Resource()
      {
         ResourceType = RESOURCE_TYPE::MESH;
      }
   };

}