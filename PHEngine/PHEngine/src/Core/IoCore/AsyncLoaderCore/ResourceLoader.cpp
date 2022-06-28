#include "ResourceLoader.h"
#include "ResourceMap.h"
#include "Core/IoCore/TextureLoaderCore/StbLoader/StbLoader.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AssimpLoader.h"
#include "Core/IoCore/AudioLoaderCore/SndFileLoader/SndFileLoader.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/IoCore/RawResource.h"

using namespace IO::Images;
using namespace IO::Images::Stb;
using namespace IO::Audio;
using namespace Graphics::Texture;
using namespace MeshLoader::Assimp;

namespace IO
{
   /************************************************************************/
   /* ResourceLoader                                                       */
   /************************************************************************/
   ResourceLoader::ResourceLoader()
   {
   }

   /************************************************************************/
   /* TextureResourceLoader                                                */
   /************************************************************************/
   TextureResourceLoader::TextureResourceLoader()
       : ResourceLoader()
   {
   }

   Resource *TextureResourceLoader::LoadResource(const std::string &key)
   {
      StbLoader textureResourceLoader;

      TextureResourceInfo texResourceInfo;
      uint8_t *data = textureResourceLoader.AllocateTextureMemoryFromFile(key, texResourceInfo);

      size_t size = texResourceInfo.Height * texResourceInfo.Width * texResourceInfo.PixelComponents;
      void *localData = malloc(size);
      memcpy(localData, data, size);
      textureResourceLoader.ReleaseTextureMemory(); // Release memory allocated for texture

      TextureResource *resource = new TextureResource();
      resource->DATA = localData;
      resource->TexInfo = texResourceInfo;

      return resource;
   }

   /************************************************************************/
   /* MeshResourceLoader                                                    */
   /************************************************************************/

   MeshResourceLoader::MeshResourceLoader()
       : ResourceLoader()
   {
   }

   Resource *MeshResourceLoader::LoadResource(const std::string &key)
   {
      const std::string &absolutePath = key;
      AssimpLoader loader(absolutePath);

      MeshResourceInfo *data = new MeshResourceInfo();

      data->meshAnimatedData = loader.GetAnimatedMeshData();
      data->meshAttributes = loader.GetMeshAttributes();

      MeshResource *resource = new MeshResource();
      resource->DATA = data;

      return resource;
   }

   /************************************************************************/
   /* AudioResourceLoader                                                  */
   /************************************************************************/

   AudioResourceLoader::AudioResourceLoader()
       : ResourceLoader()
   {
   }

   Resource *AudioResourceLoader::LoadResource(const std::string &key)
   {
      SndFileLoader loader;

      AudioResourceInfo audioResourceInfo;
      void *data = loader.AllocateMemoryForAudioSource(key, audioResourceInfo);

      size_t size = 1;
      void *localData = malloc(size);
      memcpy(localData, data, size);
      loader.ReleaseAudioMemory();

      AudioResource *resource = new AudioResource();
      resource->DATA = localData;
      resource->AudioInfo = audioResourceInfo;
      return resource;
   }
}