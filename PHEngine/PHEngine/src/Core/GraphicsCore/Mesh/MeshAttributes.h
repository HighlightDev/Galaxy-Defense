#pragma once

#include <vector>
#include <cstddef>
#include <stdint.h>

namespace MeshLoader {
   namespace Assimp
   {
      struct MeshDataCollector;
   }
}

namespace Graphics
{
   namespace Mesh
   {

      using MeshLoader::Assimp::MeshDataCollector;

      struct MeshAttributes
      {
         std::vector<float> BoneWeights;

         std::vector<int32_t> BoneIndices;

         std::vector<uint32_t> VertexIndices;

         std::vector<float> Positions;

         std::vector<float> TextureCoordinates;

         std::vector<float> Normals;

         std::vector<float> TangentNormals;

         std::vector<float> BitangetNormals;

         MeshAttributes(const struct MeshDataCollector& collector);
      };

   }
}

