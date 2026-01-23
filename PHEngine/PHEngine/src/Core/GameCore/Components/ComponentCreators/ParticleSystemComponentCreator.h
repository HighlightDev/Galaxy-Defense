#pragma once

#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"
#include "Core/GameCore/Particles/Modules/Color/SimpleColorModule.h"
#include "Core/GameCore/Particles/Modules/Lifetime/SimpleLifeTimeModule.h"
#include "Core/GameCore/Particles/Modules/Size/SimpleSizeModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/ExplosionInitialVelocityModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/OrbitVelocityModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/SimpleVelocityModule.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GraphicsCore/RenderData/CpuParticleSystemRenderData.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/ParticlePoolParameters.h"
#include "IComponentCreatable.h"

#include <type_traits>

using namespace Resources;
using namespace Graphics::Data;
using namespace Graphics::OpenGL;

namespace EngineCore {
template<typename ComponentInstantiationType>
class ParticleSystemComponentCreator : public IComponentCreatable {
public:
    virtual
        typename std::enable_if<std::is_base_of<Component, ComponentInstantiationType>::value, std::shared_ptr<Component>>::type
        CreateComponent(const std::shared_ptr<Scene>& spScene, const std::shared_ptr<ComponentData>& data) const override
    {
        const auto& mData = std::static_pointer_cast<ParticleSystemComponentData>(data);

        ParticlePoolParameters params;
        params.mParticleComponentName = mData->EngineObjectName;
        params.mParticleCount = mData->m_particlesCount;

        const auto& materialProxy = mData->m_material->GetMaterialProxyWp().lock();
        ext_assert(materialProxy, "ParticleSystemComponentCreator::CreateComponent: materialProxy is null");

        auto component = std::make_shared<ComponentInstantiationType>(mData, CpuParticleSystemRenderData(params, materialProxy));

        // Create emitter from data
        if (mData->emitterData) {
            if (mData->emitterData->emitterType == "explosion") {
                auto emitter = std::make_shared<ParticleExplosionEmitter>();
                emitter->SetOwner(component);
                emitter->SetExplosionRadius(mData->emitterData->radius);
                emitter->SetThetaSlicesCount(mData->emitterData->thetaSlicesCount);
                component->SetParticleEmitter(emitter);
            }
        }

        // Create lifetime module from data
        if (mData->lifeTimeData) {
            if (mData->lifeTimeData->moduleType == "simple") {
                auto module = std::make_shared<SimpleLifeTimeModule>();
                module->SetOwner(component);
                module->SetLifeTime(mData->lifeTimeData->lifeTime);
                component->AddParticleModule(module);
            }
        }

        // Create color module from data
        if (mData->colorData) {
            if (mData->colorData->moduleType == "simple") {
                auto module = std::make_shared<SimpleColorModule>();
                module->SetOwner(component);
                module->SetColorBegin(mData->colorData->colorBegin);
                module->SetColorEnd(mData->colorData->colorEnd);
                component->AddParticleModule(module);
            }
        }

        // Create size module from data
        if (mData->sizeData) {
            if (mData->sizeData->moduleType == "simple") {
                auto module = std::make_shared<SimpleSizeModule>();
                module->SetOwner(component);
                module->SetSizeBegin(mData->sizeData->sizeBegin);
                module->SetSizeEnd(mData->sizeData->sizeEnd);
                component->AddParticleModule(module);
            }
        }

        // Create velocity modules from data
        for (const auto& velData : mData->velocityModules) {
            if (velData->moduleType == "simple") {
                auto module = std::make_shared<SimpleVelocityModule>();
                module->SetOwner(component);
                module->SetVelocityDirection(velData->velocityDirection);
                module->SetVelocityDeviation(velData->velocityDeviation);
                module->SetExtraVelocityPower(velData->extraVelocityPower);
                component->AddParticleModule(module);
            } else if (velData->moduleType == "explosionInitial") {
                auto module = std::make_shared<ExplosionInitialVelocityModule>();
                module->SetOwner(component);
                component->AddParticleModule(module);
            } else if (velData->moduleType == "orbit") {
                auto module = std::make_shared<OrbitVelocityModule>();
                module->SetOwner(component);
                module->SetExtraVelocityDirectionType(
                    velData->extraVelocityPower > 0.0f ? eOrbitExtraVelocityDirectionType::Outside
                                                       : eOrbitExtraVelocityDirectionType::Inside);
                module->SetExtraVelocityPower(velData->extraVelocityPower);
                component->AddParticleModule(module);
            }
        }

        return component;
    }
};
} // namespace EngineCore
