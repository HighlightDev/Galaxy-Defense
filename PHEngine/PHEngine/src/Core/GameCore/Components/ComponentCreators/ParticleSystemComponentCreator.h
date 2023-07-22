#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Particles/ParticlePoolParameters.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/InstancedStaticMeshVertexFactory.h"
#include "Core/GraphicsCore/RenderData/ParticleSystemRenderData.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;
using namespace Graphics::OpenGL;

namespace EngineCore
{
    template <typename ComponentInstantiationType>
    class ParticleSystemComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const std::shared_ptr<ComponentData> &data) const override
        {
            const auto &mData = std::static_pointer_cast<ParticleSystemComponentData>(data);

            ParticlePoolParameters params;
            params.mParticleComponentName = mData->EngineObjectName;
            params.mParticleCount = mData->m_particlesCount;

            const auto &materialProxy = mData->m_material->GetMaterialProxyWp().lock();
            assert(materialProxy);
            ShaderParams particlesShaderParams(
                "ParticleShader",
                FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleFS.glsl",
                FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleGS.glsl");

            CompositeShaderParams particlesCompositeShaderParams("InstancedStaticMeshVertexFactory_SimpleShader",
                                                                          particlesShaderParams);

            typename CompositeShaderPool::sharedValue_t particleSystemShader =
                CreateMaterialShader<InstancedStaticMeshVertexFactory, SimpleShader>(
                    "InstancedStaticMeshVertexFactory_SimpleShader_" + materialProxy->MaterialName,
                    particlesShaderParams, materialProxy);

            return std::make_shared<ComponentInstantiationType>(mData, ParticleSystemRenderData(params, particleSystemShader, materialProxy));
        }
    };
}
