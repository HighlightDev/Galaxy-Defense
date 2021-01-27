#pragma once

#include "MeshDataCollector.h"
#include "Core/GraphicsCore/Mesh/MeshAttributes.h"
#include "Core/GraphicsCore/Mesh/AnimatedMeshData.h"

#include <assimp/Importer.hpp>

using namespace Graphics::Mesh;

namespace MeshLoader
{
   namespace Assimp
   {
      class AssimpLoader
      {
         using importer_t = ::Assimp::Importer;

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
