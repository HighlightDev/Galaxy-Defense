#pragma once

#include <string>
#include <glm/mat4x4.hpp>
#include <stdint.h>

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

using namespace Graphics::OpenGL;

namespace Game
{
	namespace ShaderImpl
	{

		class StaticMeshShader :
			public ShaderBase
		{
         using Base = ShaderBase;

			#define MAX_LIGHT_COUNT 1

			Uniform u_worldMatrix, u_viewMatrix, u_projectionMatrix, u_albedo, u_normalMap, u_specularMap, u_materialAmbient,
				u_materialDiffuse,
				u_materialSpecular,
				u_materialReflectivity,
				u_materialShineDamper,
				u_sunEnable,
				u_sunDirection,
				u_sunAmbientColour,
				u_sunDiffuseColour,
				u_sunSpecularColour,
				u_clipPlane,
				u_mistEnable,
				u_mistDensity,
				u_mistGradient,
				u_mistColour,
				u_directionalLightShadowMap,
				u_directionalLightShadowMatrix,
				u_bEnableSpecularMap,
				u_bEnableNormalMap;

			Uniform* u_lightPosition = new Uniform[MAX_LIGHT_COUNT],
				*u_attenuation = new Uniform[MAX_LIGHT_COUNT],
				*u_diffuseColour = new Uniform[MAX_LIGHT_COUNT],
				*u_specularColour = new Uniform[MAX_LIGHT_COUNT],
				*u_enableLight = new Uniform[MAX_LIGHT_COUNT];

		public:

			StaticMeshShader(const ShaderParams& params);

			virtual ~StaticMeshShader();

			void SetTransformationMatrices(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix);

			void SetAlbedoTex(int32_t slot);

			void SetNormalMapTex(int32_t slot);

			void SetSpecularMapTex(int32_t slot);

		protected:

			virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

			virtual void SetShaderPredefine() override;
		};

	}
}

