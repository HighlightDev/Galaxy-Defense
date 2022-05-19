#include "FontMeshAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

#include "Core/CommonCore/Assertion.h"

#include <gl/glew.h>
#include <vector>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace EngineCore;
using namespace EngineUtility;

namespace Resources
{
	std::shared_ptr<TextMesh> FontMeshAllocationPolicy::AllocateMemory(const FontParams &arg)
	{
		VertexArrayObject vao;

		static constexpr size_t verticesPerCharacter = 6;
		const size_t maxFontCharactersCount = EngineConfigHolder::GetInstance()->GetEngineConfig().MaxFontCharactersCount;

		auto *positionsVBO = new VertexBufferObject<float,
													2,
													GL_FLOAT,
													GL_DYNAMIC_DRAW>(maxFontCharactersCount * verticesPerCharacter,
																	 eAttribArrayIndexName::POSITION,
																	 GL_ARRAY_BUFFER);

		auto *texCoordsVBO = new VertexBufferObject<float,
													2,
													GL_FLOAT,
													GL_DYNAMIC_DRAW>(maxFontCharactersCount * verticesPerCharacter,
																	 eAttribArrayIndexName::TEXTURE_COORDINATES,
																	 GL_ARRAY_BUFFER);

		vao.AddVBO(positionsVBO,
				   texCoordsVBO);

		vao.BindBuffersToVao();

		return std::make_shared<TextMesh>(vao);
	}

	void FontMeshAllocationPolicy::DeallocateMemory(const std::shared_ptr<TextMesh> &arg)
	{
		arg->CleanUp();
	}

}
