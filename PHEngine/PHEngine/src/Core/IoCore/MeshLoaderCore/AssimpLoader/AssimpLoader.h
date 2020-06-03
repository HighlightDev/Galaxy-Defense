#pragma once

#include "MeshData.h"

#include <assimp/Importer.hpp>

namespace MeshLoader
{
   namespace Assimp
   {
      class AssimpLoader
      {
         typename ::Assimp::Importer importer_t;

         AnimatedMeshData* m_animatedMeshData;

         MeshAttributes* m_meshAttributes;

      public:

         AssimpLoader(const std::string& modelFilePath);

         ~AssimpLoader();

         AnimatedMeshData* GetAnimatedMeshData() const;

         MeshAttributes* GetMeshAttributes() const;

      private:

         void LoadMeshAndAnimations(const struct aiScene* scene);
      };
   }
}
