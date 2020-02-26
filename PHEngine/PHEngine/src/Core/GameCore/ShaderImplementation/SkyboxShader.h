#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"

#include <string>

using namespace Graphics::OpenGL;

namespace Game
{
	namespace ShaderImpl
	{

		class SkyboxShader : public ShaderBase
		{

		private:
			Uniform u_worldMatrix, u_viewMatrix, u_projectionMatrix, u_dayTexture, u_nightTexture;

		public:
			SkyboxShader(const ShaderParams& params);

			virtual ~SkyboxShader();

			void SetTransformMatrices(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix);

			void SetTextures(int32_t dayTexSlot, int32_t nightTexSlot);

		protected:

			virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

			virtual void SetShaderPredefine() override;
		};

	}
}

