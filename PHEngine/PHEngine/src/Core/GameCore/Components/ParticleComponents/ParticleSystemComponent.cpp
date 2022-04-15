#include "ParticleSystemComponent.h"
#include "Core/GameCore/BoundingBox.h"
#include "Core/GraphicsCore/SceneProxy/ParticleSystemSceneProxy.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/CommonCore/Random.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <cmath>

using namespace Graphics::Proxy;
using namespace Graphics;
using namespace EngineMath;

namespace EngineCore
{
    ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponentData &meshComponentData,
                                                     const ParticleSystemRenderData &renderData)
        : PrimitiveComponent(meshComponentData.GameObjectName,
                             meshComponentData.m_translation,
                             glm::vec3(),
                             glm::vec3(1.0f),
                             BoundingBox()),
          mParticlesPool(),
          mParticleProxyPropertiesPool(),
          mRenderData(renderData)
    {
        mParticlesPool.resize(1000);
        mParticleProxyPropertiesPool.resize(mParticlesPool.size());
        InitParticlePool();
    }

    ParticleSystemComponent::~ParticleSystemComponent()
    {
    }

    void ParticleSystemComponent::PostLevelInit()
    {
    }

    ComponentType ParticleSystemComponent::GetComponentType() const
    {
        return PRIMITIVE_COMPONENT;
    }

    void ParticleSystemComponent::Tick(const float deltaTime)
    {
        for (const auto &module : mParticleModules)
        {
            module->Tick(deltaTime);
        }

        for (size_t i = 0; i < mParticlesPool.size(); ++i)
        {
            auto &particle = mParticlesPool[i];

            if (!particle.isActive)
                continue;

            if ((particle.LifeRemaining - deltaTime) <= 0.0f)
            {
                particle.isActive = false;
            }
            else
            {
                particle.LifeRemaining -= deltaTime;
            }

            particle.Position = particle.Position + (particle.Velocity * 100.0f * deltaTime);
            particle.Velocity += (EngineMath::G * -AXIS_UP) * deltaTime * 2.0f;

            const float invLife = particle.LifeTime - particle.LifeRemaining;

            mParticleProxyPropertiesPool[i] = ParticleProxyProperties{
                .Position = particle.Position,
                .Color = EngineMath::LerpVec4(invLife,
                                              0.0f,
                                              particle.LifeTime,
                                              particle.ColorBegin,
                                              particle.ColorEnd),
                .Rotation = particle.Rotation,
                .Size = EngineMath::LerpFloat(particle.SizeBegin, particle.SizeEnd, invLife),
                .isActive = particle.isActive};
        }

        SyncDataWithRenderThread();
    }

    void ParticleSystemComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
    {
    }

    std::shared_ptr<PrimitiveSceneProxy> ParticleSystemComponent::CreateSceneProxy() const
    {
        return std::make_shared<ParticleSystemSceneProxy>(this);
    }

    void ParticleSystemComponent::AddParticleModule(const std::shared_ptr<ParticleModule> &particleModule)
    {
        mParticleModules.emplace_back(particleModule);
    }

    void ParticleSystemComponent::EmitParticles(const std::vector<ParticleProperties> &particleProperties)
    {
    }

    void ParticleSystemComponent::InitParticlePool()
    {
        static constexpr float radius = 20.0f;
        for (size_t i = 0; i < mParticlesPool.size(); ++i)
        {
            const float random_radius = 1.0f;
            //(Random::Float() * radius) - 10.0f;
            const float random_theta_rad = Random::Float() * EngineMath::PI * 2;
            const float random_phi_rad = Random::Float() * EngineMath::PI;

            Particle &p = mParticlesPool[i];

            p.Position = glm::vec3(
                random_radius * std::cos(random_theta_rad) * std::sin(random_phi_rad),
                random_radius * std::sin(random_theta_rad) * std::sin(random_theta_rad),
                random_radius * std::cos(random_phi_rad));
            p.Rotation = Random::Float() * EngineMath::PI * 2;

            const auto randomNdcValue = []()
            {
                return (Random::Float() * 2.0f) - 1.0f;
            };

            p.Velocity = glm::vec3(randomNdcValue() * 2.0f, randomNdcValue() * 2.0f, randomNdcValue() * 2.0f);

            p.ColorBegin = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            p.ColorEnd = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            p.SizeBegin = 0.5f;
            p.SizeEnd = 0.1f;
            p.LifeRemaining = p.LifeTime;

            p.isActive = true;
        }
    }

    void ParticleSystemComponent::SyncDataWithRenderThread()
    {
        static const uint64_t functionId = Hash("ParticleSystemComponent: SyncDataWithRenderThread");
        if (const auto &sceneSP = m_sceneWP.lock())
        {
            if (const auto &sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
            {
                sceneSP->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
                                               GetObjectId(),
                                               functionId,
                                               [=, particleProperties = mParticleProxyPropertiesPool]() mutable
                                               {
               ParticleSystemSceneProxy* proxyPtr = static_cast<ParticleSystemSceneProxy*>
               (sceneRenderer->SceneProxiesMap[SceneProxyId].get());
               proxyPtr->SetParticleProxyProperties(std::move(particleProperties)); });
            }
        }
    }
}