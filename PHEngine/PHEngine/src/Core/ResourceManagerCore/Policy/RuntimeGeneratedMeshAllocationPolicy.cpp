#include "RuntimeGeneratedMeshAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"

#include <gl/glew.h>
#include <vector>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources
{
	std::shared_ptr<Skin> RuntimeGeneratedMeshAllocationPolicy::AllocateMemory(const RuntimeGeneratedMeshPoolParameters &arg)
	{
		std::shared_ptr<Skin> resultSkin;

		{
			const auto vao = std::make_shared<VertexArrayObject>();

			const auto &vertexAttributes = arg.mVertexAttributes;
			for (const auto &vertexAttribute : vertexAttributes)
			{
				if (vertexAttribute->GetAttributeType() == eAttributeType::STANDART)
				{
					const auto &standartAttribute = std::static_pointer_cast<StandartAttributeDataBase>(vertexAttribute);
					if (standartAttribute->GetAttribArrayIndex() == eAttribArrayIndex::VertexPosition ||
						standartAttribute->GetAttribArrayIndex() == eAttribArrayIndex::VertexTexCoords)
					{
						const auto &vbo = new VertexBufferObject<float>(arg.mMaxVerticesCount,
																		vertexAttribute->GetAttributeName(),
																		vertexAttribute->GetAttributeIndex(),
																		vertexAttribute->GetAttributeComponentDataType() == eAttributeComponentDataType::FLOAT ? GL_FLOAT : GL_INT,
																		vertexAttribute->GetAttributeComponentsNumber(),
																		GL_ARRAY_BUFFER);
						vao->AddVBO(vbo);
					}
				}
			}

			assert(vao->GetVertexBufferObjects().size());
			vao->BindBuffersToVao();

			resultSkin = std::make_shared<Skin>(vao, BoundingBox3D());
		}

		return resultSkin;
	}

	void RuntimeGeneratedMeshAllocationPolicy::DeallocateMemory(std::shared_ptr<Skin> arg)
	{
		arg->CleanUp();
	}

}
