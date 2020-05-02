#include "ResourceLoader.h"
#include "ResourceMap.h"
#include "Core/IoCore/TextureLoaderCore/StbLoader/StbLoader.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

using namespace IO::Images;
using namespace IO::Images::Stb;
using namespace Graphics::Texture;

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

   Resource TextureResourceLoader::LoadResource(const std::string& key)
   {
      StbLoader textureResourceLoader;

      TexParams texParams;


      const std::string& absolutePath = EngineUtility::ConvertFromRelativeToAbsolutePath(key);

      uint8_t* data = textureResourceLoader.AllocateTextureMemoryFromFile(absolutePath, texParams);

      size_t pixelFormatSize = 0;

      if (texParams.TexPixelFormat == GL_RGB)
      {
         pixelFormatSize = 3;
      }
      else if (texParams.TexPixelFormat == GL_RGBA)
      {
         pixelFormatSize = 4;
      }

      size_t size = texParams.TexBufferHeight * texParams.TexBufferWidth * pixelFormatSize;

      void* localData = malloc(size);
      memcpy(localData, data, size);
      textureResourceLoader.ReleaseTextureMemory(); // Release memory allocated for texture

      Resource res;
      res.DATA = localData;

      return res;
   }

   /************************************************************************/
   /* MeshResourceLoader                                                      */
   /************************************************************************/

   MeshResourceLoader::MeshResourceLoader()
      : ResourceLoader()
   {
   }

   Resource MeshResourceLoader::LoadResource(const std::string& key)
   {
      return Resource();
   }
}