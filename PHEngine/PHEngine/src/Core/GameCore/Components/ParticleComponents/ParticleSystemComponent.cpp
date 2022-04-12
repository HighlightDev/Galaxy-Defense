#include "ParticleSystemComponent.h"
#include "Core/GameCore/BoundingBox.h"
#include "Core/GraphicsCore/SceneProxy/ParticleSystemSceneProxy.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

#include <glm/vec3.hpp>

using namespace Graphics::Proxy;
using namespace Graphics;

namespace EngineCore
{
    ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponentData &meshComponentData,
                                                     const ParticleSystemRenderData &renderData)
        : PrimitiveComponent(meshComponentData.GameObjectName,
                             meshComponentData.m_translation,
                             glm::vec3(),
                             glm::vec3(1.0f),
                             BoundingBox()),
          mParticlesPool()
    {
        mParticlesPool.resize(100);
        mParticleProxyPropertiesPool.resize(100);
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
            mParticleProxyPropertiesPool[i] = ParticleProxyProperties{
                .Position = mParticlesPool[i].Position,
                .Color = mParticlesPool[i].ColorEnd,
                .Rotation = mParticlesPool[i].Rotation,
                .Size = mParticlesPool[i].SizeBegin,
                .isActive = mParticlesPool[i].isActive,
            };
        }
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
                                               [=, particleProperties = std::move(mParticleProxyPropertiesPool)]() mutable
                                               {
               ParticleSystemSceneProxy* proxyPtr = static_cast<ParticleSystemSceneProxy*>
               (sceneRenderer->SceneProxiesMap[SceneProxyId].get());
               proxyPtr->SetParticleProxyProperties(std::move(particleProperties)); });
            }
        }
    }
}