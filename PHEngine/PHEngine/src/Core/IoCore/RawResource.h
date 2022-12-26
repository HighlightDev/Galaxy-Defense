#pragma once

#include "ResourceExtensionsInfo.h"
#include "Core/IoCore/TextureLoaderCore/TextureResourceInfo.h"
#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"
#include "Core/IoCore/MeshLoaderCore/MeshResourceInfo.h"

using namespace IO::Audio;

namespace IO
{
   struct Resource {

      eResourceType ResourceType = eResourceType::UNDEFINED_TYPE;

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
         ResourceType = eResourceType::TEXTURE;
      }

      void Clear() override {
         free(DATA);
      }
   };

   struct MeshResource
      : public Resource
   {
      MeshResource()
         : Resource()
      {
         ResourceType = eResourceType::MESH;
      }

      void Clear() override
      {
         MeshResourceInfo* data = GetMeshResourceInfo();
         delete data;
      }

      MeshResourceInfo* GetMeshResourceInfo() const {
         MeshResourceInfo* data = (MeshResourceInfo*)DATA;
         return data;
      }
   };

   struct AudioResource
      : public Resource
   {
       AudioResourceInfo AudioInfo;

      AudioResource()
         : Resource()
      {
         ResourceType = eResourceType::AUDIO;
      }

      void Clear() override
      {
         free(DATA);
      }
   };

}