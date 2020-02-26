#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>
#include <stdint.h>

using namespace Graphics::OpenGL;

namespace Game
{
	namespace ShaderImpl
	{

		class ResolveTextureShader : public ShaderBase
		{
			using Base = ShaderBase;

			Uniform u_textureSampler;

		public:

			ResolveTextureShader(const ShaderParams& params);

			virtual ~ResolveTextureShader();

			void SetTextureSlot(int32_t slot);

		protected:

			virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

			virtual void SetShaderPredefine() override;
		};

	}
}

