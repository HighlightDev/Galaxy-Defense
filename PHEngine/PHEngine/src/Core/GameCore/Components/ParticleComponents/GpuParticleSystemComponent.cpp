#include "GpuParticleSystemComponent.h"

#include "Core/CommonCore/Random.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Particles/Emitters/IEmitter.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/ParticleSystemComponentLuaProxy.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/CpuParticleSystemSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/GpuParticleSystemSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <cmath>
#include <iterator>

using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Graphics;
using namespace EngineMath;
using namespace TinyLogger;
using namespace EngineCore::Scripts;

namespace EngineCore {
GpuParticleSystemComponent::GpuParticleSystemComponent(
    const std::shared_ptr<ParticleSystemComponentData>& meshComponentData, const GpuParticleSystemRenderData& renderData)
    : ParticleSystemBaseComponent(
        meshComponentData->EngineObjectName,
        meshComponentData->m_translation,
        glm::vec3(),
        meshComponentData->m_scale,
        meshComponentData->m_particlesCount,
        meshComponentData->m_isEndlessRespawn)
    , mRenderData(renderData)
{
}

GpuParticleSystemComponent::~GpuParticleSystemComponent()
{
}

void GpuParticleSystemComponent::Tick(const float deltaTimeSec)
{
    if (bIsSceneProxyReady.load(std::memory_order::seq_cst) && (IsParticlesDataDirty() || IsParticleModulesProxiesDirty() || IsEndlessRespawnEnabledDirty())) {
        SyncDataWithRenderThread(0);
    }
}

void GpuParticleSystemComponent::EmitParticles()
{
    ParticleSystemBaseComponent::EmitParticles();
    SetIsParticlesDataDirty(true);
}

std::shared_ptr<PrimitiveSceneProxy> GpuParticleSystemComponent::CreateSceneProxy() const
{
    return std::make_shared<GpuParticleSystemSceneProxy>(this);
}

std::shared_ptr<Scripts::LuaProxy> GpuParticleSystemComponent::ReplicateLuaProxy()
{
    return std::make_shared<ParticleSystemComponentLuaProxy>(
        std::static_pointer_cast<GpuParticleSystemComponent>(shared_from_this()));
}

void GpuParticleSystemComponent::UpdateWorldMatrix(const glm::mat4& parentWorldMatrix)
{
    if (!mIsEnabled)
        return;

    if (const auto& ownerSp = GetOwner().lock()) {
        const auto& ownerTranslation = ownerSp->GetRootComponent()->GetTranslation();
        const auto& ownerScale = ownerSp->GetRootComponent()->GetScale();

        // Update current world matrix
        const glm::mat4 identityMatrix(1);
        m_worldMatrix = identityMatrix;
        m_worldMatrix *= glm::translate(identityMatrix, (mTransform->Translation + ownerTranslation));
        m_worldMatrix *= glm::scale(identityMatrix, mTransform->Scale);

        if (bIsSceneProxyReady.load(std::memory_order::seq_cst)) {
            // Update primitives proxy transform
            static const uint64_t functionId = Hash("GpuParticleSystemComponent:UpdatePrimitiveComponentTransform_GameThread");
            if (const auto& sceneSP = m_sceneWP.lock()) {
                if (const auto& sceneRendererSp = sceneSP->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
                    sceneRendererSp->UpdatePrimitiveComponentTransform_OnRenderThread(
                        mSceneProxyId, GetObjectId(), functionId, m_worldMatrix, glm::mat4(1), GetTransformedBoundingBox());
                }
            }
        }
        SetIsTransformationDirty(bIsSceneProxyReady.load(std::memory_order::seq_cst));
    }
}

void GpuParticleSystemComponent::SetIsParticlesDataDirty(const bool isDirty)
{
    mIsParticlesDataDirty = isDirty;
}

bool GpuParticleSystemComponent::IsParticlesDataDirty() const
{
    return mIsParticlesDataDirty;
}

void GpuParticleSystemComponent::SetIsParticleModulesProxiesDirty(const bool isDirty)
{
    mIsParticleModulesProxiesDirty = isDirty;
}

bool GpuParticleSystemComponent::IsParticleModulesProxiesDirty() const
{
    return mIsParticleModulesProxiesDirty;
}

void GpuParticleSystemComponent::SetIsEndlessRespawnEnabled(const bool value)
{
    if (mIsEndlessRespawnEnabled != value) {
        mIsEndlessRespawnEnabled = value;
        mIsEndlessRespawnEnabledDirty = true;
    }
}

void GpuParticleSystemComponent::SetIsEndlessRespawnEnabledDirty(const bool isDirty)
{
    mIsEndlessRespawnEnabledDirty = isDirty;
}

bool GpuParticleSystemComponent::IsEndlessRespawnEnabledDirty() const
{
    return mIsEndlessRespawnEnabledDirty;
}

void GpuParticleSystemComponent::SyncDataWithRenderThread(
    [[maybe_unused]] const size_t activeParticlesCount, const bool forceSyncData)
{
    static const uint64_t functionId = Hash("GpuParticleSystemComponent: SyncDataWithRenderThread");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetObjectId(),
            functionId,
            [weak = weak_from_this(),
             activeParticlesCount,
             mIsParticlesDataDirty = IsParticlesDataDirty(),
             mIsParticleModulesProxiesDirty = IsParticleModulesProxiesDirty(),
             mIsEndlessRespawnEnabledDirty = IsEndlessRespawnEnabledDirty(),
             sceneProxyId = mSceneProxyId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) mutable {
                if (const auto& componentPtr = weak.lock()) {
                    if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                        const auto particleComponentPtr = std::static_pointer_cast<GpuParticleSystemComponent>(componentPtr);
                        const auto& proxyPtr = std::static_pointer_cast<GpuParticleSystemSceneProxy>(
                            sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId));
                        if (proxyPtr) {
                            if (mIsParticlesDataDirty) {
                                const auto& particlesPool = particleComponentPtr->GetParticlesPool();
                                std::vector<glm::vec4> positionsData, velocitiesData, initialVelocitiesData, colorsData;
                                std::vector<glm::vec2> rotationAndSizeData;
                                std::vector<float> lifetimeData;
                                positionsData.reserve(particlesPool.size());
                                velocitiesData.reserve(particlesPool.size());
                                initialVelocitiesData.reserve(particlesPool.size());
                                colorsData.reserve(particlesPool.size());
                                rotationAndSizeData.reserve(particlesPool.size());
                                lifetimeData.reserve(particlesPool.size());
                                for (const auto& particle : particlesPool) {
                                    positionsData.emplace_back(particle.Position, 0.0f);
                                    velocitiesData.emplace_back(particle.Velocity, 0.0f);
                                    initialVelocitiesData.emplace_back(particle.InitialVelocity, 0.0f);
                                    colorsData.emplace_back(particle.Color, 1.0f);
                                    rotationAndSizeData.emplace_back(particle.Rotation, particle.Size);
                                    lifetimeData.emplace_back(particle.LifeTime);
                                }

                                proxyPtr->ResetParticlesData(
                                    reinterpret_cast<const void*>(positionsData.data()),
                                    positionsData.size() * sizeof(glm::vec4),
                                    reinterpret_cast<const void*>(velocitiesData.data()),
                                    velocitiesData.size() * sizeof(glm::vec4),
                                    reinterpret_cast<const void*>(initialVelocitiesData.data()),
                                    initialVelocitiesData.size() * sizeof(glm::vec4),
                                    reinterpret_cast<const void*>(colorsData.data()),
                                    colorsData.size() * sizeof(glm::vec4),
                                    reinterpret_cast<const void*>(rotationAndSizeData.data()),
                                    rotationAndSizeData.size() * sizeof(glm::vec2),
                                    reinterpret_cast<const void*>(lifetimeData.data()),
                                    lifetimeData.size() * sizeof(float));
                                particleComponentPtr->SetIsParticlesDataDirty(false);
                            }
                            if (mIsParticleModulesProxiesDirty) {
                                proxyPtr->ResetParticleModulesProxies(particleComponentPtr->GetParticleModulesProxies());
                                particleComponentPtr->SetIsParticleModulesProxiesDirty(false);
                            }
                            if (mIsEndlessRespawnEnabledDirty) {
                                proxyPtr->SetIsEndlessRespawnEnabled(particleComponentPtr->IsEndlessRespawnEnabled());
                                particleComponentPtr->SetIsEndlessRespawnEnabledDirty(false);
                            }
                        }
                    }
                }
            });
    }
}
} // namespace EngineCore