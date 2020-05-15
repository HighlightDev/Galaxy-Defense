#include "ResourceLoader.h"
#include "ResourceMap.h"
#include "Core/IoCore/TextureLoaderCore/StbLoader/StbLoader.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AssimpMeshLoader.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/IoCore/RawResource.h"

using namespace IO::Images;
using namespace IO::Images::Stb;
using namespace Graphics::Texture;
using namespace IO::MeshLoader::Assimp;

namespace IO
{
   /************************************************************************/
   /* ResourceLoader                                                          */
   /************************************************************************/
   ResourceLoader::ResourceLoader()
   {
   }

   /************************************************************************/
   /* TextureResourceLoader                                                   */
   /************************************************************************/
   TextureResourceLoader::TextureResourceLoader()
      : ResourceLoader()
   {
   }

   Resource* TextureResourceLoader::LoadResource(const std::string& key)
   {
      StbLoader textureResourceLoader;
      const std::string& absolutePath = EngineUtility::ConvertFromRelativeToAbsolutePath(key);

      TextureResourceInfo texResourceInfo;
      uint8_t* data = textureResourceLoader.AllocateTextureMemoryFromFile(absolutePath, texResourceInfo);

      size_t size = texResourceInfo.Height * texResourceInfo.Width * texResourceInfo.PixelComponents;
      void* localData = malloc(size);
      memcpy(localData, data, size);
      textureResourceLoader.ReleaseTextureMemory(); // Release memory allocated for texture

      TextureResource* resource = new TextureResource();
      resource->DATA = localData;
      resource->TexInfo = texResourceInfo;

      return resource;
   }

   /************************************************************************/
   /* MeshResourceLoader                                                      */
   /************************************************************************/

   MeshResourceLoader::MeshResourceLoader()
      : ResourceLoader()
   {
   }

   Resource* MeshResourceLoader::LoadResource(const std::string& key)
   {
      const std::string& absolutePath = EngineUtility::ConvertFromRelativeToAbsolutePath(key);
      AssimpMeshLoader<GlobalSettings::GetCountBonesPerVertexForAnimation()> loader(absolutePath);

      auto meshData = loader.GetMeshData();
      MeshAnimationData* animationData = loader.GetAnimationData();

      MeshResourceInfo* data = new MeshResourceInfo();
      data->MeshData = meshData;
      data->AninationData = animationData;

      MeshResource* resource = new MeshResource();
      resource->DATA = data;

      return resource;
   }
}