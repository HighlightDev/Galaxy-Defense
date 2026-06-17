#include "CpuParticleSystemComponent.h"

#include "Core/CommonCore/Random.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Particles/Emitters/IEmitter.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/ParticleSystemComponentLuaProxy.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/CpuParticleSystemSceneProxy.h"
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
CpuParticleSystemComponent::CpuParticleSystemComponent(
    const std::shared_ptr<ParticleSystemComponentData>& meshComponentData, const CpuParticleSystemRenderData& renderData)
    : ParticleSystemBaseComponent(
        meshComponentData->EngineObjectName,
        meshComponentData->m_translation,
        glm::vec3(),
        meshComponentData->m_scale,
        meshComponentData->m_particlesCount,
        meshComponentData->m_isEndlessRespawn)
    , mParticlesRawDataHandler(meshComponentData->m_particlesCount)
    , mRenderData(renderData)
{
}

CpuParticleSystemComponent::~CpuParticleSystemComponent()
{
}

void CpuParticleSystemComponent::Tick(const float deltaTimeSec)
{
    if (!mIsEmitting)
        return;

    for (auto& particle : mParticlesPool) {
        if (!particle.isActive && !mIsEndlessRespawnEnabled)
            continue;

        for (const auto& module : mParticleModules) {
            module->Update(particle, deltaTimeSec);
        }
    }

    size_t activeParticlesCount = 0;
    size_t particleTranslationByteOffset = 0;
    size_t particleRotationSizeByteOffset = 0;
    size_t particleColorByteOffset = 0;

    for (auto particleIt = mParticlesPool.begin(); particleIt != mParticlesPool.end(); ++particleIt) {
        if (not particleIt->isActive && mIsEndlessRespawnEnabled) {
            mParticleEmitter->EmitSingleParticle(*particleIt);

            for (const auto& particleModule : mParticleModules) {
                particleModule->EmitSingleParticle(*particleIt);
            }

            // Run module updates so Velocity is initialized from the new InitialVelocity
            // before we use it for position update below.
            for (const auto& particleModule : mParticleModules) {
                particleModule->Update(*particleIt, deltaTimeSec);
            }
        } else if (not particleIt->isActive && !mIsEndlessRespawnEnabled) {
            continue;
        }

        particleIt->Position += particleIt->Velocity * deltaTimeSec;

        mParticlesRawDataHandler.SubTranslationData(particleTranslationByteOffset, particleIt->Position);
        particleTranslationByteOffset += mParticlesRawDataHandler.GetTranslationVectorByteDataOffset();

        mParticlesRawDataHandler.SubRotationSizeData(particleRotationSizeByteOffset, particleIt->Rotation, particleIt->Size);
        particleRotationSizeByteOffset += mParticlesRawDataHandler.GetRotationSizeByteDataOffset();

        mParticlesRawDataHandler.SubColorData(particleColorByteOffset, particleIt->Color);
        particleColorByteOffset += mParticlesRawDataHandler.GetColorByteDataOffset();
        ++activeParticlesCount;
    }

    mParticlesRawDataHandler.SetTranslationActiveDataChunkSize(particleTranslationByteOffset);
    mParticlesRawDataHandler.SetRotationSizeActiveDataChunkSize(particleRotationSizeByteOffset);
    mParticlesRawDataHandler.SetColorActiveDataChunkSize(particleColorByteOffset);

    if (bIsSceneProxyReady.load(std::memory_order::seq_cst) && (activeParticlesCount || mPrevActiveParticles > 0)) {
        SyncDataWithRenderThread(activeParticlesCount);
        mPrevActiveParticles = activeParticlesCount;
    }
}

std::shared_ptr<PrimitiveSceneProxy> CpuParticleSystemComponent::CreateSceneProxy() const
{
    return std::make_shared<CpuParticleSystemSceneProxy>(this);
}

std::shared_ptr<Scripts::LuaProxy> CpuParticleSystemComponent::ReplicateLuaProxy()
{
    return std::make_shared<ParticleSystemComponentLuaProxy>(
        std::static_pointer_cast<CpuParticleSystemComponent>(shared_from_this()));
}

void CpuParticleSystemComponent::ResetParticles()
{
    ParticleSystemBaseComponent::ResetParticles();

    mParticlesRawDataHandler.ResetTranslationData();
    mParticlesRawDataHandler.ResetRotationSizeData();
    mParticlesRawDataHandler.ResetColorData();
}

void CpuParticleSystemComponent::UpdateWorldMatrix(const glm::mat4& parentWorldMatrix)
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
            static const uint64_t functionId = Hash("CpuParticleSystemComponent:UpdatePrimitiveComponentTransform_GameThread");
            if (const auto& sceneSP = m_sceneWP.lock()) {
                if (const auto& sceneRendererSp = sceneSP->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
                    sceneRendererSp->UpdatePrimitiveComponentTransform_OnRenderThread(
                        mSceneProxyId, GetObjectId(), functionId, m_worldMatrix, glm::mat4(1), GetTransformedBoundingBox());
                }
            }
        }
        SetIsTransformationDirty(!bIsSceneProxyReady.load(std::memory_order::seq_cst));
    }
}

ParticlesRawDataHandler& CpuParticleSystemComponent::GetParticlesRawDataHandler()
{
    return mParticlesRawDataHandler;
}

void CpuParticleSystemComponent::SyncDataWithRenderThread(const size_t activeParticlesCount, const bool forceSyncData)
{
    static const uint64_t functionId = Hash("CpuParticleSystemComponent: SyncDataWithRenderThread");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetObjectId(),
            functionId,
            [weak = weak_from_this(), activeParticlesCount, sceneProxyId = mSceneProxyId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) mutable {
                if (const auto& componentPtr = weak.lock()) {
                    if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                        const auto particleComponentPtr = std::static_pointer_cast<CpuParticleSystemComponent>(componentPtr);
                        const auto& proxyPtr = std::static_pointer_cast<CpuParticleSystemSceneProxy>(
                            sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId));
                        if (proxyPtr) {
                            if (activeParticlesCount > 0) {
                                proxyPtr->CopyParticlesRawData(
                                    particleComponentPtr->GetParticlesRawDataHandler().GetTranslationData(),
                                    particleComponentPtr->GetParticlesRawDataHandler().GetTranslationActiveDataChunkSize(),
                                    particleComponentPtr->GetParticlesRawDataHandler().GetRotationSizeData(),
                                    particleComponentPtr->GetParticlesRawDataHandler().GetRotationSizeActiveDataChunkSize(),
                                    particleComponentPtr->GetParticlesRawDataHandler().GetColorData(),
                                    particleComponentPtr->GetParticlesRawDataHandler().GetColorActiveDataChunkSize());
                            }

                            proxyPtr->SetActiveParticlesCount(activeParticlesCount);
                        }
                    }
                }
            });
    }
}
} // namespace EngineCore