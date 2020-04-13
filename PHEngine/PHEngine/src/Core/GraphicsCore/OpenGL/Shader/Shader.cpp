#include "Shader.h"

#include <glm/vec2.hpp>
#include <gl/glew.h>
#include <fstream>

namespace Graphics
{
	namespace OpenGL
	{

      Shader::Shader(const ShaderParams& params)
         : IShader(params.ShaderName)
         , m_shaderParams(params)
         , m_defineConstantParameters()
         , m_defines()
      {
      }

		Shader::~Shader()
		{
		}

		void Shader::ShaderInit()
		{
         std::string vsSourcePath = EngineUtility::ConvertFromRelativeToAbsolutePath(m_shaderParams.VertexShaderFile);
         std::string fsSourcePath = EngineUtility::ConvertFromRelativeToAbsolutePath(m_shaderParams.FragmentShaderFile);
         std::string gsSourcePath = EngineUtility::ConvertFromRelativeToAbsolutePath(m_shaderParams.GeometryShaderFile);

         SetIsShaderSourceFileChanged(m_shaderParams.VertexShaderFile, m_shaderParams.GeometryShaderFile, m_shaderParams.FragmentShaderFile);

			SetShaderPredefine(); // start precompile shader customization
         ProcessAllPredefines();

			const bool bShaderLoadedSuccessfully = LoadShadersSourceToGpu();
         if (bShaderLoadedSuccessfully)
         {
            CompileShaders();
            m_shaderProgramID = glCreateProgram();
            LinkShaders();
            AccessAllUniformLocations(m_shaderProgramID);
         }
      }

      ShaderParams Shader::GetShaderParams() const
      {
         return m_shaderParams;
      }

      bool Shader::LoadShadersSourceToGpu()
      {
         std::string vsSourcePath = EngineUtility::ConvertFromRelativeToAbsolutePath(m_shaderParams.VertexShaderFile);
         std::string fsSourcePath = EngineUtility::ConvertFromRelativeToAbsolutePath(m_shaderParams.FragmentShaderFile);
         std::string gsSourcePath = EngineUtility::ConvertFromRelativeToAbsolutePath(m_shaderParams.GeometryShaderFile);

         auto vsSource = LoadShaderSource(vsSourcePath);
         auto fsSource = LoadShaderSource(fsSourcePath);
         auto gsSource = LoadShaderSource(gsSourcePath);

         if (bUpdateVS) ProcessShaderIncludes(vsSource);
         if (bUpdateGS) ProcessShaderIncludes(gsSource);
         if (bUpdateFS) ProcessShaderIncludes(fsSource);

         return SendToGpuShadersSources(vsSource, gsSource, fsSource);
      }

		void Shader::ProcessAllPredefines()
		{
         std::vector<ShaderGenericDefineConstant> vertexConstantPredefine, fragmentConstantPredefine, geometryConstantPredefine;
         std::vector<ShaderGenericDefine> vertexPredefine, fragmentPredefine, geometryPredefine;
         
         if (m_defineConstantParameters.size() > 0 || m_defines.size() > 0)
         {
            for (auto define_it = m_defineConstantParameters.begin(); define_it != m_defineConstantParameters.end(); ++define_it)
            {
               if (define_it->m_ShaderType & ShaderType::VertexShader)
               {
                  vertexConstantPredefine.emplace_back(*define_it);
               }
               if (define_it->m_ShaderType & ShaderType::FragmentShader)
               {
                  fragmentConstantPredefine.emplace_back(*define_it);
               }
               if (define_it->m_ShaderType & ShaderType::GeometryShader)
               {
                  geometryConstantPredefine.emplace_back(*define_it);
               }
            }

            for (auto define_it = m_defines.begin(); define_it != m_defines.end(); ++define_it)
            {
               if (define_it->m_ShaderType & ShaderType::VertexShader)
               {
                  vertexPredefine.emplace_back(*define_it);
               }
               if (define_it->m_ShaderType & ShaderType::FragmentShader)
               {
                  fragmentPredefine.emplace_back(*define_it);
               }
               if (define_it->m_ShaderType & ShaderType::GeometryShader)
               {
                  geometryPredefine.emplace_back(*define_it);
               }
            }
         }

			if (vertexConstantPredefine.size() > 0 || vertexPredefine.size() > 0 && bUpdateVS)
			{
				if (m_shaderParams.VertexShaderFile != "")
				{
					std::string vsSourcePath = std::move(EngineUtility::ConvertFromRelativeToAbsolutePath(m_shaderParams.VertexShaderFile));
               ProcessPredefineToFile(vsSourcePath, vertexConstantPredefine, vertexPredefine);
				}
			}
			if (fragmentConstantPredefine.size() > 0 || fragmentPredefine.size() > 0 && bUpdateFS)
			{
				if (m_shaderParams.FragmentShaderFile != "")
				{
					std::string fsSourcePath = std::move(EngineUtility::ConvertFromRelativeToAbsolutePath(m_shaderParams.FragmentShaderFile));
               ProcessPredefineToFile(fsSourcePath, fragmentConstantPredefine, fragmentPredefine);
				}
			}
			if (geometryConstantPredefine.size() > 0 || geometryPredefine.size() > 0 && bUpdateGS)
			{
				if (m_shaderParams.GeometryShaderFile != "")
				{
					std::string gsSourcePath = std::move(EngineUtility::ConvertFromRelativeToAbsolutePath(m_shaderParams.GeometryShaderFile));
               ProcessPredefineToFile(gsSourcePath, geometryConstantPredefine, geometryPredefine);
				}
			}
		}

		int32_t Shader::GetSubroutineIndex(ShaderType shaderType, const std::string& subroutineName) const
		{
			return glGetSubroutineIndex(m_shaderProgramID, (GLenum)shaderType, subroutineName.c_str());
		}

		void Shader::LoadSubroutineIndex(ShaderType shaderType, int32_t countIndices, int32_t subroutineIndex) const
		{
			glUniformSubroutinesuiv((int32_t)shaderType, countIndices, (uint32_t*)subroutineIndex);
		}

#if DEBUG

		void Shader::RecompileShader()
		{
			CleanUp(false);
         const bool bLoaded = LoadShadersSourceToGpu();

			if (bLoaded)
			{
            CompileShaders();
				LinkShaders();
				AccessAllUniformLocations(m_shaderProgramID);
			}

         const bool bCompiledSuccesfully = IsShaderCompiled();
         std::cout << "Shader " + m_shaderParams.ShaderName + (bCompiledSuccesfully ? " has recompiled successfully " : "has not recompiled") << std::endl;
		}

#endif

      void Shader::Define(ShaderType shaderType, const std::string& name)
      {
         m_defines.emplace_back(ShaderDefine(name, true, shaderType));
      }

      void Shader::Undefine(ShaderType shaderType, const std::string& name)
      {
         m_defines.emplace_back(ShaderDefine(name, false, shaderType));
      }

	}

}