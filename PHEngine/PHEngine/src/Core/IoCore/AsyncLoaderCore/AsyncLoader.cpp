#include "AsyncLoader.h"
#include "ResourceMap.h"
#include "Core/IoCore/TextureLoaderCore/StbLoader/StbLoader.h"
#include "Core/GraphicsCore/Texture/TexParams.h"

using namespace IO::Images;
using namespace IO::Images::Stb;
using namespace Graphics::Texture;

namespace IO
{
   /************************************************************************/
   /* AsyncLoader                                                          */
   /************************************************************************/
   AsyncLoader::AsyncLoader()
   {
   }

   /************************************************************************/
   /* TextureAsyncLoader                                                   */
   /************************************************************************/
   TextureAsyncLoader::TextureAsyncLoader()
      : AsyncLoader()
   {

   }

   Resource TextureAsyncLoader::DoAsyncJob(const std::string& key)
   {
      StbLoader textureLoader;

      TexParams texParams;

      uint8_t* data = textureLoader.AllocateTextureMemoryFromFile(key, texParams);

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

      uint8_t* localData = nullptr;
      memcpy(localData, data, size);

      textureLoader.ReleaseTextureMemory(); // Release memory allocated for texture

      Resource res;
      res.DATA = localData;

      return res;
   }

   /************************************************************************/
   /* MeshAsyncLoader                                                      */
   /************************************************************************/

   MeshAsyncLoader::MeshAsyncLoader()
      : AsyncLoader()
   {
   }

   Resource MeshAsyncLoader::DoAsyncJob(const std::string& key)
   {
      return Resource();
   }
}