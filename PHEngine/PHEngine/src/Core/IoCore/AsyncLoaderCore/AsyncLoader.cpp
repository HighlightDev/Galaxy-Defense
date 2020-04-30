#include "AsyncLoader.h"
#include "ResourceMap.h"

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
      
      while (true)
      {

      }

      // do job


      return Resource();
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

   /************************************************************************/
   /* MeshAnimationAsyncLoader                                             */
   /************************************************************************/

   MeshAnimationAsyncLoader::MeshAnimationAsyncLoader()
      : AsyncLoader()
   {

   }

   Resource MeshAnimationAsyncLoader::DoAsyncJob(const std::string& key)
   {
      return Resource();
   }
}