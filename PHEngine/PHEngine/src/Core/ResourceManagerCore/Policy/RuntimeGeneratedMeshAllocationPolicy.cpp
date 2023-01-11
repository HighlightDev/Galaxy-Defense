#include "RuntimeGeneratedMeshAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/CommonCore/Assertion.h"

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

			auto *vertexVBO = new VertexBufferObject<float,
													 3,
													 GL_FLOAT,
													 GL_STATIC_DRAW>(std::vector<float>(arg.mMaxVerticesCount * 3),
																	 eAttribArrayIndexName::POSITION,
																	 GL_ARRAY_BUFFER,
																	 eDataCarryFlag::INVALIDATE);

			auto *texCoordsVBO = new VertexBufferObject<float,
													 2,
													 GL_FLOAT,
													 GL_STATIC_DRAW>(std::vector<float>(arg.mMaxVerticesCount * 2),
																	 eAttribArrayIndexName::TEXTURE_COORDINATES,
																	 GL_ARRAY_BUFFER,
																	 eDataCarryFlag::INVALIDATE);

			vao->AddVBO(vertexVBO,
					   texCoordsVBO);
					   
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
