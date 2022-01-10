#pragma once

#include "ResourceExtensionsInfo.h"
#include "Core/IoCore/TextureLoaderCore/TextureResourceInfo.h"
#include "Core/IoCore/MeshLoaderCore/MeshResourceInfo.h"

namespace IO
{

   struct Resource {

      RESOURCE_TYPE ResourceType = RESOURCE_TYPE::UNDEFINED_TYPE;

      void* DATA;

      virtual void Clear()
      {
         free(DATA);
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

      virtual void Clear() override {
         free(DATA);
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

      virtual void Clear() override
      {
         MeshResourceInfo* data = GetMeshResourceInfo();
         delete data;
      }

      MeshResourceInfo* GetMeshResourceInfo() const {
         MeshResourceInfo* data = (MeshResourceInfo*)DATA;
         return data;
      }
   };

}