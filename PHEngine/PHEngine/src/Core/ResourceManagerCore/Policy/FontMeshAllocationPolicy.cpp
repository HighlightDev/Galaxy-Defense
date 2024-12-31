#include "FontMeshAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/eAttribArrayIndex.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/GameCore/LoggerExtension.h"

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
		LogInfo("FontMeshAllocationPolicy::AllocateMemory: ", arg.FontName);
		VertexArrayObject vao;

		static constexpr size_t verticesPerCharacter = 6;
		const size_t maxFontCharactersCount = EngineConfigHolder::GetInstance()->GetEngineConfig().MaxFontCharactersCount;

		auto *positionsVBO = new VertexBufferObject<float,
													GL_DYNAMIC_DRAW>(maxFontCharactersCount * verticesPerCharacter,
																	 "VertexPosition",
																	 (int32_t)eAttribArrayIndex::VertexPosition,
																	 GL_FLOAT,
																	 2,
																	 GL_ARRAY_BUFFER);

		auto *texCoordsVBO = new VertexBufferObject<float,
													GL_DYNAMIC_DRAW>(maxFontCharactersCount * verticesPerCharacter,
																	 "VertexTexCoords",
																	 (int32_t)eAttribArrayIndex::VertexTexCoords,
																	 GL_FLOAT,
																	 2,
																	 GL_ARRAY_BUFFER);

		vao.AddVBO(positionsVBO,
				   texCoordsVBO);

		vao.BindBuffersToVao();

		return std::make_shared<TextMesh>(vao);
	}

	void FontMeshAllocationPolicy::DeallocateMemory(const std::shared_ptr<TextMesh> &arg)
	{
		LogInfo("FontMeshAllocationPolicy::DeallocateMemory: ", arg->GetBuffer()->GetDescriptor());
		arg->CleanUp();
	}

}
