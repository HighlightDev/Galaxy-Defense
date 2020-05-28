#include "MeshAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshData.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AnimationData.h"
#include "Core/GraphicsCore/OpenGL/IndexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/GraphicsCore/Animation/Bone.h"
#include "Core/UtilityCore/AssimpSkeletonConverter.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"
#include "Core/IoCore/MeshLoaderCore/MeshResourceInfo.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/RawResource.h"
#include "Core/CommonCore/Assertion.h"

#include <gl/glew.h>

using namespace Graphics::OpenGL;
using namespace Graphics::Animation;
using namespace Graphics::Mesh;
using namespace IO::MeshLoader::Assimp;
using namespace IO;

namespace Resources
{
   template class MeshAllocationPolicy<std::string>;

   template <typename Model>
	std::shared_ptr<Skin> MeshAllocationPolicy<Model>::AllocateMemory(Model& arg)
	{
		const int32_t countOfBonesInfluencingOnVertex = 3;

		std::shared_ptr<Skin> resultSkin;

		{
         VertexArrayObject vao;

         Resource* outResource;
         bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg);

         assert(bResourceValid);

         MeshResource* meshResource = static_cast<MeshResource*>(outResource);
         MeshResourceInfo* meshInfo = meshResource->GetMeshResourceInfo();

         MeshData<countOfBonesInfluencingOnVertex>* meshData = meshInfo->MeshData;

         const std::vector<float>& vertices = meshData->Verts;
         const std::vector<float>& normals = meshData->N_Verts;
         const std::vector<float>& texCoords = meshData->T_Verts;
         const std::vector<float>& tangents = meshData->Tangent_Verts;
         const std::vector<float>& bitangents = meshData->Bitanget_Verts;
         const std::vector<float>& blendWeights = meshData->BlendWeights;
			const std::vector<int32_t>& blendIndices = meshData->BlendIndices;
			const std::vector<uint32_t>& indices = meshData->Indices;

			IndexBufferObject* ibo = nullptr;

			VertexBufferObjectBase* vertexVBO, *normalsVBO = nullptr, *texCoordsVBO = nullptr, *tangentsVBO = nullptr, *bitangentsVBO = nullptr, *blendWeightsVBO = nullptr, *blendIndicesVBO = nullptr;

			if (meshData->bHasIndices)
				ibo = new IndexBufferObject(indices);

			vertexVBO = new VertexBufferObject<float, 3, GL_FLOAT>(vertices, GL_ARRAY_BUFFER, 0, DataCarryFlag::Invalidate);

			if (meshData->bHasNormals)
				normalsVBO = new VertexBufferObject<float, 3, GL_FLOAT>(normals, GL_ARRAY_BUFFER, 1, DataCarryFlag::Invalidate);
			if (meshData->bHasTextureCoordinates)
				texCoordsVBO = new VertexBufferObject<float, 2, GL_FLOAT>(texCoords, GL_ARRAY_BUFFER, 2, DataCarryFlag::Invalidate);
			if (meshData->bHasTangentVertices)
				tangentsVBO = new VertexBufferObject<float, 3, GL_FLOAT>(tangents, GL_ARRAY_BUFFER, 4, DataCarryFlag::Invalidate);
			if (meshData->bHasTangentVertices)
				bitangentsVBO = new VertexBufferObject<float, 3, GL_FLOAT>(bitangents, GL_ARRAY_BUFFER, 5, DataCarryFlag::Invalidate);
			if (meshData->bHasAnimation)
			{
				blendWeightsVBO = new VertexBufferObject<float, countOfBonesInfluencingOnVertex, GL_FLOAT>(blendWeights, GL_ARRAY_BUFFER, 6, DataCarryFlag::Invalidate);
				blendIndicesVBO = new VertexBufferObject<int32_t, countOfBonesInfluencingOnVertex, GL_FLOAT>(blendIndices, GL_ARRAY_BUFFER, 7, DataCarryFlag::Invalidate);
			}

			vao.AddVBO(vertexVBO, normalsVBO, texCoordsVBO, tangentsVBO, bitangentsVBO, blendWeightsVBO, blendIndicesVBO);

			vao.AddIndexBuffer(ibo);
			vao.BindBuffersToVao();

			if (meshData->bHasAnimation)
			{
				Bone* rootBone = EngineUtility::AssimpSkeletonConverter::ConvertAssimpBoneToEngineBone(meshData->SkeletonRoot);
            meshData->SkeletonRoot->CleanUp();
            delete meshData->SkeletonRoot;
            meshData->SkeletonRoot = nullptr;
				resultSkin = std::make_shared<AnimatedSkin>(vao, std::make_shared<Bone>(*rootBone), std::shared_ptr<AnimatedMeshData>(meshInfo->MeshAnimatedData));
			}
			else
			{
				resultSkin = std::make_shared<Skin>(vao);
			}

		}

		return resultSkin;
	}

   template <typename Model>
	void MeshAllocationPolicy<Model>::DeallocateMemory(std::shared_ptr<Skin> arg)
	{
		arg->CleanUp();
	}

}
