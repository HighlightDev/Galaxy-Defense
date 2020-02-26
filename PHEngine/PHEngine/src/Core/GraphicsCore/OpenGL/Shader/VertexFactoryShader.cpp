#include "VertexFactoryShader.h"

#include "Core/UtilityCore/StringStreamWrapper.h"

namespace Graphics
{
   namespace OpenGL
   {

      VertexFactoryShader::VertexFactoryShader(const std::string& vertexFactoryName)
         : IShader(vertexFactoryName)
      { 
      }

      VertexFactoryShader::~VertexFactoryShader()
      {
      }

      void VertexFactoryShader::InitShader(const std::string& pathToShaderSource)
      {
         mShaderSource = Base::LoadShaderSource(pathToShaderSource);
         SetShaderPredefine();
      }

      void VertexFactoryShader::ProcessAllPredefines()
      {
         if (mConstantDefines.size() > 0 || mDefines.size() > 0)
         {
            if ("" != mShaderSource)
            {
               ProcessPredefineToSource(mShaderSource, mConstantDefines, mDefines);
            }
         }
      }

      void VertexFactoryShader::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         // Base implementation
      }

      std::string VertexFactoryShader::GetShaderSource() const
      {
         return mShaderSource;
      }

      void VertexFactoryShader::Define(const std::string& name)
      {
         mDefines.emplace_back(name, true);
      }

      void VertexFactoryShader::Undefine(const std::string& name)
      {
         mDefines.emplace_back(name, false);
      }
   }
}