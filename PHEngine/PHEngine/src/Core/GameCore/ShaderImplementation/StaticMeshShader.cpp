#include "StaticMeshShader.h"


namespace Game
{
	namespace ShaderImpl
	{

		StaticMeshShader::StaticMeshShader(const ShaderParams& params)
			: ShaderBase(params)
		{
         ShaderInit();
		}

		StaticMeshShader::~StaticMeshShader()
		{

		}

		void StaticMeshShader::SetTransformationMatrices(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
		{
			u_worldMatrix.LoadUniform(worldMatrix);
			u_viewMatrix.LoadUniform(viewMatrix);
			u_projectionMatrix.LoadUniform(projectionMatrix);
		}

		void StaticMeshShader::SetAlbedoTex(int32_t slot) {

			u_albedo.LoadUniform(slot);
		}

		void StaticMeshShader::SetNormalMapTex(int32_t slot) {

			u_normalMap.LoadUniform(slot);
		}

		void StaticMeshShader::SetSpecularMapTex(int32_t slot) {

			u_specularMap.LoadUniform(slot);
		}

		void StaticMeshShader::AccessAllUniformLocations(uint32_t shaderProgramId)
		{
         Base::AccessAllUniformLocations(shaderProgramId);

			u_worldMatrix = GetUniform("worldMatrix", shaderProgramId);
			u_viewMatrix = GetUniform("viewMatrix", shaderProgramId);
			u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramId);
			u_albedo = GetUniform("albedo", shaderProgramId);
			u_normalMap = GetUniform("normalMap", shaderProgramId);
			u_specularMap = GetUniform("specularMap", shaderProgramId);

			u_materialAmbient = GetUniform("materialAmbient", shaderProgramId);
			u_materialDiffuse = GetUniform("materialDiffuse", shaderProgramId);
			u_materialSpecular = GetUniform("materialSpecular", shaderProgramId);
			u_materialReflectivity = GetUniform("materialReflectivity", shaderProgramId);
			u_materialShineDamper = GetUniform("materialShineDamper", shaderProgramId);
			u_sunDirection = GetUniform("sunDirection", shaderProgramId);
			u_sunAmbientColour = GetUniform("sunAmbientColour", shaderProgramId);
			u_sunDiffuseColour = GetUniform("sunDiffuseColour", shaderProgramId);
			u_sunSpecularColour = GetUniform("sunSpecularColour", shaderProgramId);
			u_sunEnable = GetUniform("sunEnable", shaderProgramId);
			u_mistEnable = GetUniform("mistEnable", shaderProgramId);
			u_mistDensity = GetUniform("mistDensity", shaderProgramId);
			u_mistGradient = GetUniform("mistGradient", shaderProgramId);
			u_mistColour = GetUniform("mistColour", shaderProgramId);

			u_clipPlane = GetUniform("clipPlane", shaderProgramId);
			u_directionalLightShadowMap = GetUniform("dirLightShadowMap", shaderProgramId);
			u_directionalLightShadowMatrix = GetUniform("dirLightShadowMatrix", shaderProgramId);
			u_bEnableNormalMap = GetUniform("bEnableNormalMap", shaderProgramId);
			u_bEnableSpecularMap = GetUniform("bEnableSpecularMap", shaderProgramId);
		}

		void StaticMeshShader::SetShaderPredefine()
		{
			
		}

	}
}