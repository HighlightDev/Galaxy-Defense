#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Particles/ParticlePoolParameters.h"
#include "Core/ResourceManagerCore/Pool/ParticlesPool.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/InstancedStaticMeshVertexFactory.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryCompositeShader.h"
#include "Core/GraphicsCore/RenderData/ParticleSystemRenderData.h"

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;
using namespace Graphics::OpenGL;

namespace EngineCore
{
    class Scene;

    template <typename ComponentInstantiationType>
    class ParticleSystemComponentCreator
        : public ComponentCreatorBase
    {
    public:
        virtual typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene> &spScene, const ComponentData &data) const override
        {
            const ParticleSystemComponentData &mData = static_cast<const ParticleSystemComponentData &>(data);

            ParticlePoolParameters params;
            params.mParticleComponentName = mData.GameObjectName;
            params.mParticleCount = mData.m_particlesCount;

            ParticlesPool::sharedValue_t particlesSkin =
                ParticlesPool::GetInstance()->GetOrAllocateResource(params);
            ShaderParams particlesShaderParams(
                "ParticleShader",
                FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleVS.glsl",
                FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleFS.glsl",
                FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleGS.glsl");

            TemplatedCompositeShaderParams particlesCompositeShaderParams("InstancedStaticMeshVertexFactory_SimpleShader", particlesShaderParams);

            const typename CompositeShaderPool::sharedValue_t &shader =
                CompositeShaderPool::GetInstance()->template GetOrAllocateResource<VertexFactoryCompositeShader<InstancedStaticMeshVertexFactory, SimpleShader>>(particlesCompositeShaderParams);

            return std::make_shared<ComponentInstantiationType>(mData, ParticleSystemRenderData(particlesSkin, shader));
        }
    };
}
