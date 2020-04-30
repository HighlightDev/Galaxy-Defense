#include "MeshAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AssimpMeshLoader.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshVertexData.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshAnimationData.h"
#include "Core/GraphicsCore/OpenGL/IndexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/GraphicsCore/Animation/Bone.h"
#include "Core/UtilityCore/AssimpSkeletonConverter.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"

#include <gl/glew.h>

using namespace Graphics::OpenGL;
using namespace IO::MeshLoader::Assimp;
using namespace Graphics::Animation;
using namespace Graphics::Mesh;

namespace Resources
{
   template class MeshAllocationPolicy<std::string>;

   template <typename Model>
	std::shared_ptr<Skin> MeshAllocationPolicy<Model>::AllocateMemory(Model& arg)
	{
		const int32_t countOfBonesInfluencingOnVertex = 3;

		std::shared_ptr<Skin> resultSkin;

		{
			std::string absolutePath = std::move(EngineUtility::ConvertFromRelativeToAbsolutePath(arg));
			AssimpMeshLoader<countOfBonesInfluencingOnVertex> loader(absolutePath);

         VertexArrayObject vao;

			MeshVertexData<countOfBonesInfluencingOnVertex>* meshData = loader.LoadAndGetMeshData();

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
				Bone* rootBone = EngineUtility::AssimpSkeletonConverter::GetInstance()->ConvertAssimpBoneToEngineBone(meshData->SkeletonRoot);
            meshData->SkeletonRoot->CleanUp();
            delete meshData->SkeletonRoot;
            meshData->SkeletonRoot = nullptr;
				resultSkin = std::make_shared<AnimatedSkin>(vao, std::make_shared<Bone>(*rootBone));
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
