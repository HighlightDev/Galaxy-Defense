#include "BloomFxShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderType.h"
#include "Core/GraphicsCore/PostFX/Bloom/BloomConstants.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineUtility;
using namespace Graphics;

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

      void BloomFxShader::SetBluredColorTexture(const int32_t textureSlot)
      {
         u_bluredColorTexture.LoadUniform(textureSlot);
      }

      void BloomFxShader::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         Shader::AccessAllUniformLocations(shaderProgramID);

         u_sceneColorTexture = GetUniform("sceneColorTexture", shaderProgramID);
         u_bluredColorTexture = GetUniform("bluredColorTexture", shaderProgramID);
      }

      void BloomFxShader::AccessAllSubroutineIndices(uint32_t shaderProgramId)
      {
         mExtractBrightPartsSubroutineIndex = GetSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), "extractBrightParts");
         mVerticalBlurSubroutineIndex = GetSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), "runVerticalBlur");
         mHorizontalBlurSubroutineIndex = GetSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), "runHorizontalBlur");
      }

      void BloomFxShader::LoadExtractBrightPartsSubroutine()
      {
         LoadSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), 1, &mExtractBrightPartsSubroutineIndex);
      }

      void BloomFxShader::LoadRunVerticalBlurSubroutine()
      {
         LoadSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), 1, &mVerticalBlurSubroutineIndex);
      }

      void BloomFxShader::LoadRunHorizontalBlurSubroutine()
      {
         LoadSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), 1, &mHorizontalBlurSubroutineIndex);
      }

      void BloomFxShader::SetShaderPredefine()
      {
         const auto &cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
         assert(BloomQualitySettings::s_blurQualityMap.count(cfg.BloomQualityName));
         const auto &bloomQuality = BloomQualitySettings::s_blurQualityMap.at(cfg.BloomQualityName);

         DefineConstant<int32_t>(FragmentShader, "BLUR_WIDTH", bloomQuality.blurWidth);
         DefineConstant<float>(FragmentShader, "bloomThreshold", cfg.BloomThreshold);
         DefineConstantArray<float>(FragmentShader, "weights", EngineMath::CalculateGaussNormalizedWeights(bloomQuality.blurWidth));
      }
   }
}