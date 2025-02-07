#include "VertexFactoryShader.h"

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/UtilityCore/StringStreamWrapper.h"

namespace Graphics {
namespace OpenGL {

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
    if (mConstantDefines.size() > 0 || mDefines.size() > 0) {
        if ("" != mShaderSource) {
            ProcessPredefineToSource(mShaderSource, mConstantDefines, mDefines, mDefineConstantArrays);
        }
    }
}

void VertexFactoryShader::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    // Base implementation
}

void VertexFactoryShader::BindAttributeLocations(const int32_t shaderProgramId)
{
    const auto& vertexAttributes = GetVertexAttributes(shaderProgramId);
    for (const auto& vertexAttribute : vertexAttributes) {
        glBindAttribLocation(shaderProgramId, vertexAttribute->GetAttributeIndex(), vertexAttribute->GetAttributeName().c_str());
    }
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
} // namespace OpenGL
} // namespace Graphics