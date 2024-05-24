#include "MeshAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/IndexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"
#include "Core/GraphicsCore/OpenGL/eAttribArrayIndex.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"

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
	std::shared_ptr<Skin> MeshAllocationPolicy::AllocateMemory(const MeshPoolParameters &arg)
	{
		std::shared_ptr<Skin> resultSkin;
		BoundingBox3D boundingBox;

		{
			const auto vao = std::make_shared<VertexArrayObject>();

			Resource *outResource;
			const bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg.mModelPath);

			assert(bResourceValid);

			const MeshResource *meshResource = static_cast<MeshResource *>(outResource);
			const MeshResourceInfo *meshInfo = meshResource->GetMeshResourceInfo();

			MeshAttributes *meshAttributes = meshInfo->meshAttributes;

			const auto &vertexAttributes = arg.mVertexAttributes;
			for (const auto &vertexAttribute : vertexAttributes)
			{
				if (vertexAttribute->GetAttributeType() == eAttributeType::STANDART)
				{
					const auto &standartAttribute = std::static_pointer_cast<StandartAttributeDataBase>(vertexAttribute);
					if (eAttribArrayIndex::VertexBlendIndex == standartAttribute->GetAttribArrayIndex() &&
						meshAttributes->BoneIndices.size())
					{
						auto blendIndicesVBO = new VertexBufferObject<int32_t>(std::move(meshAttributes->BoneIndices),
																			   standartAttribute->GetAttributeName(),
																			   standartAttribute->GetAttributeIndex(),
																			   GL_INT,
																			   standartAttribute->GetAttributeComponentsNumber(),
																			   GL_ARRAY_BUFFER,
																			   eDataCarryFlag::INVALIDATE);
						vao->AddVBO(blendIndicesVBO);
					}
					else
					{
						std::vector<float> data;
						switch (standartAttribute->GetAttribArrayIndex())
						{
						case eAttribArrayIndex::VertexPosition:
						{
							data = std::move(meshAttributes->Positions);
							BoundingBoxBuilder builder;
							boundingBox = builder.Build(data);
							break;
						}
						case eAttribArrayIndex::VertexNormal:
							data = std::move(meshAttributes->Normals);
							break;
						case eAttribArrayIndex::VertexTexCoords:
							data = std::move(meshAttributes->TextureCoordinates);
							break;
						case eAttribArrayIndex::VertexTangent:
							data = std::move(meshAttributes->TangentNormals);
							break;
						case eAttribArrayIndex::VertexBitangent:
							data = std::move(meshAttributes->BitangetNormals);
							break;
						case eAttribArrayIndex::VertexBlendWeights:
							data = std::move(meshAttributes->BoneWeights);
							break;

						default:
							assert(false);
							break;
						}

						if (data.size())
						{
							auto vbo = new VertexBufferObject<float>(std::move(data),
																	 standartAttribute->GetAttributeName(),
																	 standartAttribute->GetAttributeIndex(),
																	 standartAttribute->GetAttributeComponentDataType() == eAttributeComponentDataType::FLOAT ? GL_FLOAT : GL_INT,
																	 standartAttribute->GetAttributeComponentsNumber(),
																	 GL_ARRAY_BUFFER,
																	 eDataCarryFlag::INVALIDATE);
							vao->AddVBO(vbo);
						}
					}
				}
			}

			if (meshAttributes->VertexIndices.size())
			{
				const auto &ibo = new IndexBufferObject(std::move(meshAttributes->VertexIndices), eDataCarryFlag::INVALIDATE);
				vao->AddIndexBuffer(ibo);
			}

			assert(vao->GetVertexBufferObjects().size());
			vao->BindBuffersToVao();

			if (meshInfo->meshAnimatedData)
			{
				// todo: this part will crash due to pointer deletion when resource will be deleted
				resultSkin = std::make_shared<AnimatedSkin>(vao, std::shared_ptr<AnimatedMeshData>(meshInfo->meshAnimatedData), boundingBox);
			}
			else
			{
				resultSkin = std::make_shared<Skin>(vao, boundingBox);
			}

			ResourceMap::GetInstance()->UnloadResource(arg.mModelPath);
		}

		return resultSkin;
	}
	void MeshAllocationPolicy::DeallocateMemory(std::shared_ptr<Skin> arg)
	{
		arg->CleanUp();
	}

}
