#include "Shader.h"
#include "Core/GameCore/LoggerExtension.h"

#include <glm/vec2.hpp>
#include <gl/glew.h>
#include <fstream>

using namespace EngineCore;

namespace Graphics
{
   namespace OpenGL
   {

      Shader::Shader(const ShaderParams &params)
          : IShader(params.ShaderName),
            m_shaderParams(params),
            m_defineConstantParameters(),
            m_defines(),
            m_defineConstantArrays()
      {
      }

      Shader::~Shader()
      {
      }

      void Shader::ShaderInit()
      {
         std::string vsSourcePath = m_shaderParams.VertexShaderFile;
         std::string fsSourcePath = m_shaderParams.FragmentShaderFile;
         std::string gsSourcePath = m_shaderParams.GeometryShaderFile;

         SetShaderPredefine(); // start precompile shader customization
         ProcessAllPredefines();

         const bool bShaderLoadedSuccessfully = LoadShadersSourceToGpu();
         if (bShaderLoadedSuccessfully)
         {
            CompileShaders();
            m_shaderProgramID = glCreateProgram();
            LinkShaders();
            AccessAllUniformLocations(m_shaderProgramID);
            AccessAllSubroutineIndices(m_shaderProgramID);
         }
         else
         {
            LogInfo("Shader::ShaderInit => Shader source loading failed");
         }
      }

      ShaderParams Shader::GetShaderParams() const
      {
         return m_shaderParams;
      }

      bool Shader::LoadShadersSourceToGpu()
      {
         std::string vsSourcePath = m_shaderParams.VertexShaderFile;
         std::string fsSourcePath = m_shaderParams.FragmentShaderFile;
         std::string gsSourcePath = m_shaderParams.GeometryShaderFile;

         auto vsSource = LoadShaderSource(vsSourcePath);
         auto fsSource = LoadShaderSource(fsSourcePath);
         auto gsSource = LoadShaderSource(gsSourcePath);

         ProcessShaderIncludes(vsSource);
         ProcessShaderIncludes(gsSource);
         ProcessShaderIncludes(fsSource);

         return SendToGpuShadersSources(vsSource, gsSource, fsSource);
      }

      void Shader::ProcessAllPredefines()
      {
         std::vector<ShaderGenericDefineConstant> vertexConstantPredefine, fragmentConstantPredefine, geometryConstantPredefine;
         std::vector<ShaderGenericConstantArray> vertexArrayConstants, fragmentArrayConstants, geometryArrayConstants;
         std::vector<ShaderGenericDefine> vertexPredefine, fragmentPredefine, geometryPredefine;

         if (m_defineConstantParameters.size() > 0 || m_defines.size() > 0 || m_defineConstantArrays.size() > 0)
         {
            for (auto define_it = m_defineConstantParameters.begin(); define_it != m_defineConstantParameters.end(); ++define_it)
            {
               if (define_it->m_ShaderType & eShaderType::VertexShader)
               {
                  vertexConstantPredefine.emplace_back(*define_it);
               }
               if (define_it->m_ShaderType & eShaderType::FragmentShader)
               {
                  fragmentConstantPredefine.emplace_back(*define_it);
               }
               if (define_it->m_ShaderType & eShaderType::GeometryShader)
               {
                  geometryConstantPredefine.emplace_back(*define_it);
               }
            }

            for (const auto &arrayConstant : m_defineConstantArrays)
            {
               if (arrayConstant.m_ShaderType & eShaderType::VertexShader)
               {
                  vertexArrayConstants.emplace_back(arrayConstant);
               }
               if (arrayConstant.m_ShaderType & eShaderType::FragmentShader)
               {
                  fragmentArrayConstants.emplace_back(arrayConstant);
               }
               if (arrayConstant.m_ShaderType & eShaderType::GeometryShader)
               {
                  geometryArrayConstants.emplace_back(arrayConstant);
               }
            }

            for (auto define_it = m_defines.begin(); define_it != m_defines.end(); ++define_it)
            {
               if (define_it->m_ShaderType & eShaderType::VertexShader)
               {
                  vertexPredefine.emplace_back(*define_it);
               }
               if (define_it->m_ShaderType & eShaderType::FragmentShader)
               {
                  fragmentPredefine.emplace_back(*define_it);
               }
               if (define_it->m_ShaderType & eShaderType::GeometryShader)
               {
                  geometryPredefine.emplace_back(*define_it);
               }
            }
         }

         const bool processVsPredefines = m_shaderParams.VertexShaderFile != "" && (vertexConstantPredefine.size() || vertexPredefine.size() || vertexArrayConstants.size());
         const bool processFsPredefines = m_shaderParams.FragmentShaderFile != "" && (fragmentConstantPredefine.size() || fragmentPredefine.size() || fragmentArrayConstants.size());
         const bool processGsPredefines = m_shaderParams.GeometryShaderFile != "" && (geometryConstantPredefine.size() || geometryPredefine.size() || geometryArrayConstants.size());

         if (processVsPredefines)
         {
            ProcessPredefineToFile(m_shaderParams.VertexShaderFile, vertexConstantPredefine, vertexPredefine, vertexArrayConstants);
         }

         if (processFsPredefines)
         {
            ProcessPredefineToFile(m_shaderParams.FragmentShaderFile, fragmentConstantPredefine, fragmentPredefine, fragmentArrayConstants);
         }

         if (processGsPredefines)
         {
            ProcessPredefineToFile(m_shaderParams.GeometryShaderFile, geometryConstantPredefine, geometryPredefine, geometryArrayConstants);
         }
      }

      uint32_t Shader::GetSubroutineIndex(const int32_t shaderType, const std::string &subroutineName) const
      {
         return glGetSubroutineIndex(m_shaderProgramID, (GLenum)shaderType, subroutineName.c_str());
      }

      void Shader::LoadSubroutineIndex(const int32_t shaderType, int32_t countIndices, uint32_t *subroutineIndices) const
      {
         glUniformSubroutinesuiv((GLenum)shaderType, countIndices, subroutineIndices);
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
         LogInfo("Shader::RecompileShader => shader " + m_shaderParams.ShaderName + (bCompiledSuccesfully ? " has recompiled successfully " : "has not recompiled"));
      }

#endif

      void Shader::Define(eShaderType shaderType, const std::string &name)
      {
         m_defines.emplace_back(ShaderDefine(name, true, shaderType));
      }

      void Shader::Undefine(eShaderType shaderType, const std::string &name)
      {
         m_defines.emplace_back(ShaderDefine(name, false, shaderType));
      }

   }

}