#include "BloomFxShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderType.h"

namespace EngineCore
{
   namespace ShaderImpl
   {
      BloomFxShader::BloomFxShader(const ShaderParams &params)
          : Shader(params)
      {
         ShaderInit();
      }

      BloomFxShader::~BloomFxShader()
      {
      }

      void BloomFxShader::SetSceneColorTexture(const int32_t textureSlot)
      {
         u_sceneColorTexture.LoadUniform(textureSlot);
      }

      void BloomFxShader::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         Shader::AccessAllUniformLocations(shaderProgramID);

         u_sceneColorTexture = GetUniform("sceneColorTexture", shaderProgramID);
      }

      void BloomFxShader::AccessAllSubroutineIndices(uint32_t shaderProgramId)
      {
         mExtractBrightPartsSubroutineIndex = GetSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), "extractBrightParts");
      }

      void BloomFxShader::LoadExtractBrightPartsSubroutine()
      {
         LoadSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), 1, &mExtractBrightPartsSubroutineIndex);
      }
   }
}