#include "MeshAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/IndexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"

#include "Core/IoCore/MeshLoaderCore/MeshResourceInfo.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/RawResource.h"

#include "Core/CommonCore/Assertion.h"

#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/GameCore/BoundingBoxBuilder.h"

#include <gl/glew.h>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace MeshLoader::Assimp;
using namespace Game;
using namespace IO;

namespace Resources
{
   template class MeshAllocationPolicy<std::string>;

   template <typename Model>
	std::shared_ptr<Skin> MeshAllocationPolicy<Model>::AllocateMemory(const Model& arg)
	{
		const int32_t countOfBonesInfluencingOnVertex = GlobalSettings::GetCountBonesPerVertexForAnimation();

		std::shared_ptr<Skin> resultSkin;

		{
         VertexArrayObject vao;

         Resource* outResource;
         bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg);

         assert(bResourceValid);

         const MeshResource* meshResource = static_cast<MeshResource*>(outResource);
         const MeshResourceInfo* meshInfo = meshResource->GetMeshResourceInfo();

         MeshAttributes* meshAttributes = meshInfo->MeshAttributes;

			IndexBufferObject* ibo = nullptr;

			VertexBufferObjectBase *normalsVBO = nullptr, *texCoordsVBO = nullptr, *tangentsVBO = nullptr, *bitangentsVBO = nullptr, *blendWeightsVBO = nullptr, *blendIndicesVBO = nullptr;

			if (meshAttributes->VertexIndices.size())
				ibo = new IndexBufferObject(meshAttributes->VertexIndices);

         VertexBufferObject<float, 3, GL_FLOAT>* vertexVBO = new VertexBufferObject<float, 3, GL_FLOAT>(meshAttributes->Positions, GL_ARRAY_BUFFER, 0, DataCarryFlag::Store);

			if (meshAttributes->Normals.size())
				normalsVBO = new VertexBufferObject<float, 3, GL_FLOAT>(meshAttributes->Normals, GL_ARRAY_BUFFER, 1, DataCarryFlag::Invalidate);
			if (meshAttributes->TextureCoordinates.size())
				texCoordsVBO = new VertexBufferObject<float, 2, GL_FLOAT>(meshAttributes->TextureCoordinates, GL_ARRAY_BUFFER, 2, DataCarryFlag::Invalidate);
			if (meshAttributes->TangentNormals.size())
				tangentsVBO = new VertexBufferObject<float, 3, GL_FLOAT>(meshAttributes->TangentNormals, GL_ARRAY_BUFFER, 4, DataCarryFlag::Invalidate);
			if (meshAttributes->BitangetNormals.size())
				bitangentsVBO = new VertexBufferObject<float, 3, GL_FLOAT>(meshAttributes->BitangetNormals, GL_ARRAY_BUFFER, 5, DataCarryFlag::Invalidate);

			if (meshAttributes->BoneIndices.size() && meshAttributes->BoneWeights.size())
			{
				blendWeightsVBO = new VertexBufferObject<float, countOfBonesInfluencingOnVertex, GL_FLOAT>(meshAttributes->BoneWeights, GL_ARRAY_BUFFER, 6, DataCarryFlag::Invalidate);
				blendIndicesVBO = new VertexBufferObject<int32_t, countOfBonesInfluencingOnVertex, GL_FLOAT>(meshAttributes->BoneIndices, GL_ARRAY_BUFFER, 7, DataCarryFlag::Invalidate);
			}

			vao.AddVBO(vertexVBO, normalsVBO, texCoordsVBO, tangentsVBO, bitangentsVBO, blendWeightsVBO, blendIndicesVBO);

			vao.AddIndexBuffer(ibo);
			vao.BindBuffersToVao();

         BoundingBoxBuilder builder;
         BoundingBox boundingBox = builder.Build(vertexVBO->GetCastedDataRef());
         vertexVBO->InvalidateData();

			if (meshInfo->MeshAnimatedData)
			{
				resultSkin = std::make_shared<AnimatedSkin>(vao, std::shared_ptr<AnimatedMeshData>(meshInfo->MeshAnimatedData), boundingBox);
			}
			else
			{
				resultSkin = std::make_shared<Skin>(vao, boundingBox);
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
