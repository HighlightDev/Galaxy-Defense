#include "AssimpLoader.h"
#include "Core/CommonCore/Assertion.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

namespace MeshLoader
{
   namespace Assimp
   {
      AssimpLoader::AssimpLoader(const std::string& modelFilePath)
      {
         size_t LOAD_FLAGS = (
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace | 
            aiProcess_LimitBoneWeights);

         const aiScene* scene = importer_t.ReadFile(modelFilePath, LOAD_FLAGS);

         assert((scene));
         LoadMeshAndAnimations(scene);
      }

      AssimpLoader::~AssimpLoader()
      {
      }

      MeshAttributes* AssimpLoader::GetMeshAttributes() const
      {
         return m_meshAttributes;
      }

      AnimatedMeshData* AssimpLoader::GetAnimatedMeshData() const
      {
         return m_animatedMeshData;
      }

      void AssimpLoader::LoadMeshAndAnimations(const struct aiScene* scene)
      {
         MeshDataCollector collector(scene);
         collector.Collect();

         m_meshAttributes = new MeshAttributes(collector);

         if (scene->HasAnimations())
         {
            m_animatedMeshData = new AnimatedMeshData(collector);
         }
      }

   }
}
