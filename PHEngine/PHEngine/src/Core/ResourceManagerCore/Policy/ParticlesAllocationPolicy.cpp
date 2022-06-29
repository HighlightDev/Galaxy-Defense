#include "ParticlesAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/InstancedVertexBufferObject.h"
#include "Core/GameCore/BoundingBox.h"
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

			auto *vertexVBO = new VertexBufferObject<float,
													 3,
													 GL_FLOAT,
													 GL_STATIC_DRAW>(std::vector<float>({0.0f, 0.0f, 0.0f}),
																	 eAttribArrayIndexName::POSITION,
																	 GL_ARRAY_BUFFER,
																	 eDataCarryFlag::INVALIDATE);

			auto *instancedTransformVBO = new InstancedVertexBufferObject<float,
																		  3,
																		  GL_FLOAT,
																		  GL_STREAM_DRAW,
																		  1>(arg.mParticleCount,
																			 eAttribArrayIndexName::CUSTOM_0,
																			 GL_ARRAY_BUFFER);

			auto *instancedRotationSizeVBO = new InstancedVertexBufferObject<float,
																			 2,
																			 GL_FLOAT,
																			 GL_STREAM_DRAW,
																			 1>(arg.mParticleCount,
																				eAttribArrayIndexName::CUSTOM_1,
																				GL_ARRAY_BUFFER);

			auto *instancedColorVBO = new InstancedVertexBufferObject<float,
																	  4,
																	  GL_FLOAT,
																	  GL_STREAM_DRAW,
																	  1>(arg.mParticleCount,
																		 eAttribArrayIndexName::CUSTOM_2,
																		 GL_ARRAY_BUFFER);

			vao->AddVBO(vertexVBO,
					   instancedTransformVBO,
					   instancedRotationSizeVBO,
					   instancedColorVBO);
					   
			vao->BindBuffersToVao();

			resultSkin = std::make_shared<Skin>(vao, BoundingBox());
		}

		return resultSkin;
	}

	void ParticlesAllocationPolicy::DeallocateMemory(std::shared_ptr<Skin> arg)
	{
		arg->CleanUp();
	}

}
