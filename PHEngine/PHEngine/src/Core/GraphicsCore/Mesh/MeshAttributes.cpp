#include "MeshAttributes.h"

#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshDataCollector.h"

using namespace MeshLoader::Assimp;

namespace Graphics
{
   namespace Mesh
   {
      MeshAttributes::MeshAttributes(const MeshDataCollector& collector)
         : BoneWeights(collector.BoneWeights)
         , BoneIndices(collector.BoneIndices)
         , VertexIndices(collector.VertexIndices)
         , Positions(collector.Positions)
         , TextureCoordinates(collector.TextureCoordinates)
         , Normals(collector.Normals)
         , TangentNormals(collector.TangentNormals)
         , BitangetNormals(collector.BitangetNormals)
      {
      }
   }
}