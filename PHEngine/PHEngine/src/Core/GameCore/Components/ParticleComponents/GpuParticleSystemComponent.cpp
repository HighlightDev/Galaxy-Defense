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
        meshComponentData->m_particlesCount)
    , mRenderData(renderData)
{
}

GpuParticleSystemComponent::~GpuParticleSystemComponent()
{
}

void GpuParticleSystemComponent::Tick(const float deltaTimeSec)
{
    if (bIsSceneProxyReady.load(std::memory_order::seq_cst) && IsParticlesDataDirty()) {
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
    isParticlesDataDirty = isDirty;
}

bool GpuParticleSystemComponent::IsParticlesDataDirty() const
{
    return isParticlesDataDirty;
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
             isParticlesDataDirty = IsParticlesDataDirty(),
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
                            if (isParticlesDataDirty) {
                                const auto& particlesPool = particleComponentPtr->GetParticlesPool();
                                std::vector<glm::vec4> positionsData;
                                std::vector<glm::vec4> colorsData;
                                positionsData.reserve(particlesPool.size());
                                colorsData.reserve(particlesPool.size());
                                std::transform(
                                    particlesPool.begin(),
                                    particlesPool.end(),
                                    std::back_inserter(positionsData),
                                    [](const Particle& particle) { return glm::vec4(particle.Position, 0.0f); });
                                std::transform(
                                    particlesPool.begin(),
                                    particlesPool.end(),
                                    std::back_inserter(colorsData),
                                    [](const Particle& particle) { return glm::vec4(particle.Color, 1.0f); });

                                proxyPtr->ResetParticlesData(
                                    reinterpret_cast<const void*>(positionsData.data()),
                                    positionsData.size() * sizeof(glm::vec4),
                                    reinterpret_cast<const void*>(colorsData.data()),
                                    colorsData.size() * sizeof(glm::vec4));
                                particleComponentPtr->SetIsParticlesDataDirty(false);
                            }
                        }
                    }
                }
            });
    }
}
} // namespace EngineCore