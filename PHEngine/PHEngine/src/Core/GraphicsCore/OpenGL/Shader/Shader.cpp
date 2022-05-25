#include "Shader.h"

#include <glm/vec2.hpp>
#include <gl/glew.h>
#include <fstream>

#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;

namespace Graphics
{
   namespace OpenGL
   {

      Shader::Shader(const ShaderParams &params)
          : IShader(params.ShaderName), m_shaderParams(params), m_defineConstantParameters(), m_defines()
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
         }
         else
         {
            Logger::Out("Shader::ShaderInit => Shader source loading failed");
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

         if (vertexConstantPredefine.size() > 0 || vertexPredefine.size() > 0)
         {
            if (m_shaderParams.VertexShaderFile != "")
            {
               ProcessPredefineToFile(m_shaderParams.VertexShaderFile, vertexConstantPredefine, vertexPredefine);
            }
         }
         if (fragmentConstantPredefine.size() > 0 || fragmentPredefine.size() > 0)
         {
            if (m_shaderParams.FragmentShaderFile != "")
            {
               ProcessPredefineToFile(m_shaderParams.FragmentShaderFile, fragmentConstantPredefine, fragmentPredefine);
            }
         }
         if (geometryConstantPredefine.size() > 0 || geometryPredefine.size() > 0)
         {
            if (m_shaderParams.GeometryShaderFile != "")
            {
               ProcessPredefineToFile(m_shaderParams.GeometryShaderFile, geometryConstantPredefine, geometryPredefine);
            }
         }
      }

      int32_t Shader::GetSubroutineIndex(ShaderType shaderType, const std::string &subroutineName) const
      {
         return glGetSubroutineIndex(m_shaderProgramID, (GLenum)shaderType, subroutineName.c_str());
      }

      void Shader::LoadSubroutineIndex(ShaderType shaderType, int32_t countIndices, uint32_t *subroutineIndices) const
      {
         glUniformSubroutinesuiv((int32_t)shaderType, countIndices, subroutineIndices);
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
         Logger::Out("Shader::RecompileShader => shader " + m_shaderParams.ShaderName + (bCompiledSuccesfully ? " has recompiled successfully " : "has not recompiled"));
      }

#endif

      void Shader::Define(ShaderType shaderType, const std::string &name)
      {
         m_defines.emplace_back(ShaderDefine(name, true, shaderType));
      }

      void Shader::Undefine(ShaderType shaderType, const std::string &name)
      {
         m_defines.emplace_back(ShaderDefine(name, false, shaderType));
      }

   }

}