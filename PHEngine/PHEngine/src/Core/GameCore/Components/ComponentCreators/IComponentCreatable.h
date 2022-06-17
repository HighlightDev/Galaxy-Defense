#pragma once

#include <memory>

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"

using namespace Resources;

namespace EngineCore
{
    class Scene;
    class Component;

    class IComponentCreatable
    {
    public:
        virtual std::shared_ptr<Component> CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const = 0;
    };

    class ComponentCreatorBase 
    : public IComponentCreatable
    {
    public:
        template <typename VertexFactoryType, typename BaseShaderType>
        typename CompositeShaderPool::sharedValue_t
        CreateMaterialShader(const std::string &compositeShaderName,
                             const ShaderParams &shaderParams,
                             std::shared_ptr<MaterialProxy> materialProxy) const
        {
            TemplatedCompositeMaterialShaderParams compositeParams(compositeShaderName,
                                                                   shaderParams,
                                                                   materialProxy);

            return CompositeShaderPool::GetInstance()
                ->template GetOrAllocateResource<VertexFactoryMaterialCompositeShader<
                    VertexFactoryType,
                    BaseShaderType>>(compositeParams);
        }
    };
}