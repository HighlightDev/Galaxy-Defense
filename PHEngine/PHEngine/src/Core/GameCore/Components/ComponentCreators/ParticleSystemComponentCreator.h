#pragma once

#include <type_traits>

#include "IComponentCreatable.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GraphicsCore/RenderData/ParticleSystemRenderData.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/ParticlePoolParameters.h"

using namespace Resources;
using namespace Graphics::Data;
using namespace Graphics::OpenGL;

namespace EngineCore
{
    template <typename ComponentInstantiationType>
    class ParticleSystemComponentCreator
        : public IComponentCreatable
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

            return std::make_shared<ComponentInstantiationType>(mData, ParticleSystemRenderData(params, materialProxy));
        }
    };
}
