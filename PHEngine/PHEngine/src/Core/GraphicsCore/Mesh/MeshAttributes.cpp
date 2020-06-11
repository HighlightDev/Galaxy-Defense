#include "MeshAttributes.h"

#include <Core/IoCore/MeshLoaderCore/AssimpLoader/MeshDataCollector.h>

using namespace MeshLoader::Assimp;

namespace Graphics
{
   namespace Mesh
   {
      MeshAttributes::MeshAttributes(const MeshDataCollector& collector)
         : BoneWeights(std::move(collector.BoneWeights))
         , BoneIndices(std::move(collector.BoneIndices))
         , VertexIndices(std::move(collector.VertexIndices))
         , Positions(std::move(collector.Positions))
         , TextureCoordinates(std::move(collector.TextureCoordinates))
         , Normals(std::move(collector.Normals))
         , TangentNormals(std::move(collector.TangentNormals))
         , BitangetNormals(std::move(collector.BitangetNormals))
      {
      }
   }
}