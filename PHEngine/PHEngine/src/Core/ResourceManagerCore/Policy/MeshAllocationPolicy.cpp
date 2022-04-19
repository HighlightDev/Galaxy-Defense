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
#include <vector>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace MeshLoader::Assimp;
using namespace EngineCore;
using namespace IO;

namespace Resources
{
	std::shared_ptr<Skin> MeshAllocationPolicy::AllocateMemory(const std::string &arg)
	{
		const int32_t countOfBonesInfluencingOnVertex = GlobalSettings::GetCountBonesPerVertexForAnimation();

		std::shared_ptr<Skin> resultSkin;

		{
			VertexArrayObject vao;

			Resource *outResource;
			const bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg);

			assert(bResourceValid);

			const MeshResource *meshResource = static_cast<MeshResource *>(outResource);
			const MeshResourceInfo *meshInfo = meshResource->GetMeshResourceInfo();

			MeshAttributes *meshAttributes = meshInfo->meshAttributes;

			IndexBufferObject *ibo = nullptr;

			VertexBufferObjectBase *normalsVBO = nullptr,
								   *texCoordsVBO = nullptr,
								   *tangentsVBO = nullptr,
								   *bitangentsVBO = nullptr,
								   *blendWeightsVBO = nullptr,
								   *blendIndicesVBO = nullptr;

			if (meshAttributes->VertexIndices.size())
				ibo = new IndexBufferObject(meshAttributes->VertexIndices, eDataCarryFlag::INVALIDATE);

			auto *vertexVBO = new VertexBufferObject<float,
													 3,
													 GL_FLOAT,
													 GL_STATIC_DRAW>(meshAttributes->Positions,
																	 eAttribArrayIndexName::POSITION,
																	 GL_ARRAY_BUFFER,
																	 eDataCarryFlag::STORE);

			if (meshAttributes->Normals.size())
			{
				normalsVBO = new VertexBufferObject<float,
													3,
													GL_FLOAT,
													GL_STATIC_DRAW>(meshAttributes->Normals,
																	eAttribArrayIndexName::NORMAL,
																	GL_ARRAY_BUFFER,
																	eDataCarryFlag::INVALIDATE);
			}
			if (meshAttributes->TextureCoordinates.size())
			{
				texCoordsVBO = new VertexBufferObject<float,
													  2,
													  GL_FLOAT,
													  GL_STATIC_DRAW>(meshAttributes->TextureCoordinates,
																	  eAttribArrayIndexName::TEXTURE_COORDINATES,
																	  GL_ARRAY_BUFFER,
																	  eDataCarryFlag::INVALIDATE);
			}
			if (meshAttributes->TangentNormals.size())
			{
				tangentsVBO = new VertexBufferObject<float,
													 3,
													 GL_FLOAT,
													 GL_STATIC_DRAW>(meshAttributes->TangentNormals,
																	 eAttribArrayIndexName::TANGENT,
																	 GL_ARRAY_BUFFER,
																	 eDataCarryFlag::INVALIDATE);
			}
			if (meshAttributes->BitangetNormals.size())
			{
				bitangentsVBO = new VertexBufferObject<float,
													   3,
													   GL_FLOAT,
													   GL_STATIC_DRAW>(meshAttributes->BitangetNormals,
																	   eAttribArrayIndexName::BITANGENT,
																	   GL_ARRAY_BUFFER,
																	   eDataCarryFlag::INVALIDATE);
			}

			if (meshAttributes->BoneIndices.size() && meshAttributes->BoneWeights.size())
			{
				blendWeightsVBO = new VertexBufferObject<float,
														 countOfBonesInfluencingOnVertex,
														 GL_FLOAT,
														 GL_STATIC_DRAW>(meshAttributes->BoneWeights,
																		 eAttribArrayIndexName::BONE_INDEX,
																		 GL_ARRAY_BUFFER,
																		 eDataCarryFlag::INVALIDATE);

				blendIndicesVBO = new VertexBufferObject<int32_t,
														 countOfBonesInfluencingOnVertex,
														 GL_FLOAT,
														 GL_STATIC_DRAW>(meshAttributes->BoneIndices,
																		 eAttribArrayIndexName::BONE_WEIGHT,
																		 GL_ARRAY_BUFFER,
																		 eDataCarryFlag::INVALIDATE);
			}

			vao.AddVBO(vertexVBO,
					   normalsVBO,
					   texCoordsVBO,
					   tangentsVBO,
					   bitangentsVBO,
					   blendWeightsVBO,
					   blendIndicesVBO);

			vao.AddIndexBuffer(ibo);
			vao.BindBuffersToVao();

			BoundingBoxBuilder builder;
			BoundingBox boundingBox = builder.Build(vertexVBO->GetCastedDataRef());
			vertexVBO->InvalidateData();

			if (meshInfo->meshAnimatedData)
			{
				resultSkin = std::make_shared<AnimatedSkin>(vao, std::shared_ptr<AnimatedMeshData>(meshInfo->meshAnimatedData), boundingBox);
			}
			else
			{
				resultSkin = std::make_shared<Skin>(vao, boundingBox);
			}
		}

		return resultSkin;
	}
	void MeshAllocationPolicy::DeallocateMemory(std::shared_ptr<Skin> arg)
	{
		arg->CleanUp();
	}

}
