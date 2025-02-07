#pragma once
#include "AVertexFactoryMaterialShaderModule.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "VertexFactoryShader.h"

#include <type_traits>

namespace Graphics {
namespace OpenGL {
template<typename VertexFactoryShaderType, typename ShaderType>
class VertexFactoryMaterialCompositeShader : public AVertexFactoryMaterialShaderModule {

protected:
    std::shared_ptr<Shader> mBaseShader;

public:
    VertexFactoryMaterialCompositeShader(const CompositeShaderParams& compositeShaderParams)
        : AVertexFactoryMaterialShaderModule(
              compositeShaderParams,
              std::make_shared<VertexFactoryShaderType>(),
              std::make_shared<MaterialShader>(
                  static_cast<CompositeMaterialShaderParams&>(const_cast<CompositeShaderParams&>(compositeShaderParams))
                      .mMaterialProxy))
        , mBaseShader(std::make_shared<ShaderType>(compositeShaderParams.mShaderParams))
    {
        Init();
    }

    std::shared_ptr<ShaderType> GetShader() const
    {
        return std::static_pointer_cast<ShaderType>(mBaseShader);
    }

    std::shared_ptr<Shader> GetBaseShader() const override
    {
        return mBaseShader;
    }

    std::shared_ptr<typename std::enable_if<
        std::is_base_of<VertexFactoryShader, VertexFactoryShaderType>::value,
        VertexFactoryShaderType>::type>
    GetVertexFactoryShader() const
    {
        return std::static_pointer_cast<VertexFactoryShaderType>(mVertexFactoryShader);
    }

    void BindAttributeLocations(const int32_t shaderProgramId) override
    {
        return mVertexFactoryShader->BindAttributeLocations(shaderProgramId);
    }

    std::vector<std::shared_ptr<AttributeDataBase>> GetVertexAttributes() const
    {
        return mVertexFactoryShader->GetVertexAttributes(m_shaderProgramID);
    }
};
} // namespace OpenGL
} // namespace Graphics
