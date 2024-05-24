#include "ParticlesAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/InstancedVertexBufferObject.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/CommonCore/Assertion.h"

#include <gl/glew.h>
#include <vector>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources
{
	std::shared_ptr<Skin> ParticlesAllocationPolicy::AllocateMemory(const ParticlePoolParameters &arg)
	{
		std::shared_ptr<Skin> resultSkin;

		{
			const auto vao = std::make_shared<VertexArrayObject>();
			VertexBufferObject<float> *vertexVBO = nullptr;

			const auto &vertexAttributes = arg.mVertexAttributes;
			for (const auto &vertexAttribute : vertexAttributes)
			{
				if (vertexAttribute->GetAttributeType() == eAttributeType::STANDART)
				{
					const auto &standartAttribute = std::static_pointer_cast<StandartAttributeDataBase>(vertexAttribute);
					if (standartAttribute->GetAttribArrayIndex() == eAttribArrayIndex::VertexPosition)
					{
						vertexVBO = new VertexBufferObject<float>(std::vector<float>({0.0f, 0.0f, 0.0f}),
																  vertexAttribute->GetAttributeName(),
																  vertexAttribute->GetAttributeIndex(),
																  GL_FLOAT,
																  vertexAttribute->GetAttributeComponentsNumber(),
																  GL_ARRAY_BUFFER,
																  eDataCarryFlag::INVALIDATE);
					}
				}
				else
				{
					auto customVBO = new InstancedVertexBufferObject<float>(arg.mParticleCount,
																			vertexAttribute->GetAttributeName(),
																			vertexAttribute->GetAttributeIndex(),
																			vertexAttribute->GetAttributeComponentDataType() == eAttributeComponentDataType::FLOAT ? GL_FLOAT : GL_INT,
																			vertexAttribute->GetAttributeComponentsNumber(),
																			GL_ARRAY_BUFFER);
					vao->AddVBO(customVBO);
				}
			}

			assert(vertexVBO);
			vao->AddVBO(vertexVBO);
			vao->BindBuffersToVao();

			resultSkin = std::make_shared<Skin>(vao, BoundingBox3D());
		}

		return resultSkin;
	}

	void ParticlesAllocationPolicy::DeallocateMemory(std::shared_ptr<Skin> arg)
	{
		arg->CleanUp();
	}

}
