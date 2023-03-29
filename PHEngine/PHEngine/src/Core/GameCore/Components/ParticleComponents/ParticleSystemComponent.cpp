#include "ParticleSystemComponent.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GraphicsCore/SceneProxy/ParticleSystemSceneProxy.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/CommonCore/Random.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Particles/Emitters/IEmitter.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <cmath>
#include <algorithm>
#include <iterator>

using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Graphics;
using namespace EngineMath;
using namespace TinyLogger;

namespace EngineCore
{
    ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponentData &meshComponentData,
                                                     const ParticleSystemRenderData &renderData)
        : PrimitiveComponent(meshComponentData.EngineObjectName,
                             meshComponentData.m_translation,
                             glm::vec3(),
                             glm::vec3(1.0f)),
          mParticlesPool(),
          mParticlesRawDataHandler(meshComponentData.m_particlesCount),
          mRenderData(renderData)
    {
        mParticlesPool.resize(meshComponentData.m_particlesCount);
        mSortOrderValue = std::numeric_limits<int32_t>::max(); // draw this primitive the last one
    }

    ParticleSystemComponent::~ParticleSystemComponent()
    {
    }

    void ParticleSystemComponent::PostLevelInit()
    {
    }

    eComponentType ParticleSystemComponent::GetComponentType() const
    {
        return PRIMITIVE_COMPONENT;
    }

    void ParticleSystemComponent::Tick(const float deltaTime)
    {
        static constexpr float particleMoveSpeed = 15.0f;

        for (auto &particle : mParticlesPool)
        {
            if (!particle.isActive)
                continue;

            for (const auto &module : mParticleModules)
            {
                module->Update(particle, deltaTime);
            }
        }

        size_t activeParticlesCount = 0;
        size_t particleTranslationByteOffset = 0;
        size_t particleRotationSizeByteOffset = 0;
        size_t particleColorByteOffset = 0;

        for (auto particleIt = mParticlesPool.begin(); particleIt != mParticlesPool.end(); ++particleIt)
        {
            if (!particleIt->isActive)
                continue;

            if ((particleIt->LifeRemaining - deltaTime) > 0.0f)
            {
                particleIt->Position += glm::normalize(particleIt->InitialVelocity + particleIt->Velocity) * deltaTime * particleMoveSpeed;
                particleIt->LifeRemaining -= deltaTime;

                mParticlesRawDataHandler.SubTranslationData(particleTranslationByteOffset, particleIt->Position);
                particleTranslationByteOffset += mParticlesRawDataHandler.GetTranslationVectorByteDataOffset();

                mParticlesRawDataHandler.SubRotationSizeData(particleRotationSizeByteOffset,
                                                             particleIt->Rotation,
                                                             particleIt->Size);
                particleRotationSizeByteOffset += mParticlesRawDataHandler.GetRotationSizeByteDataOffset();

                mParticlesRawDataHandler.SubColorData(particleColorByteOffset, particleIt->Color);
                particleColorByteOffset += mParticlesRawDataHandler.GetColorByteDataOffset();
                ++activeParticlesCount;
            }
            else
            {
                particleIt->isActive = false;
            }
        }

        mParticlesRawDataHandler.SetTranslationActiveDataChunkSize(particleTranslationByteOffset);
        mParticlesRawDataHandler.SetRotationSizeActiveDataChunkSize(particleRotationSizeByteOffset);
        mParticlesRawDataHandler.SetColorActiveDataChunkSize(particleColorByteOffset);

        if (activeParticlesCount || mPrevActiveParticles > 0)
        {
            SyncDataWithRenderThread(activeParticlesCount);
            mPrevActiveParticles = activeParticlesCount;
        }
    }

    void ParticleSystemComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
    {
    }

    std::shared_ptr<PrimitiveSceneProxy> ParticleSystemComponent::CreateSceneProxy() const
    {
        return std::make_shared<ParticleSystemSceneProxy>(this);
    }

    void ParticleSystemComponent::AddParticleModule(const std::shared_ptr<IParticleModule> &particleModule)
    {
        const auto &newModuleType = particleModule->GetParticleModuleType();
        auto foundSameModuleIt = std::find_if(mParticleModules.begin(), mParticleModules.end(),
                                              [=](const auto &particleModule)
                                              { return particleModule->GetParticleModuleType() == newModuleType; });
        assert(foundSameModuleIt == mParticleModules.end());
        mParticleModules.emplace_back(particleModule);
        std::sort(mParticleModules.begin(), mParticleModules.end(), [](const auto &leftModule, const auto &rightModule)
                  { return (uint8_t)leftModule->GetParticleModuleType() < (uint8_t)leftModule->GetParticleModuleType(); });
    }

    void ParticleSystemComponent::EmitParticles(const size_t particlesCount)
    {
        mParticleEmitter->EmitParticles(particlesCount);

        for (const auto &particleModule : mParticleModules)
        {
            particleModule->OnEmitParticles();
        }
    }

    void ParticleSystemComponent::UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix)
    {
        if (!mIsEnabled)
            return;

        if (const auto &ownerSp = GetOwner().lock())
        {
            const auto &ownerTranslation = ownerSp->GetRootComponent()->GetTranslation();
            const auto &ownerScale = ownerSp->GetRootComponent()->GetScale();

            // Update current relative matrix
            const glm::mat4 identityMatrix(1);
            m_relativeMatrix = glm::mat4(1);
            m_relativeMatrix *= glm::translate(glm::mat4(1), mTransform->Translation + ownerTranslation);
            m_relativeMatrix *= glm::scale(glm::mat4(1), mTransform->Scale + ownerScale);

            // Update primitives proxy transform
            static const uint64_t functionId = Hash("ParticleSystemComponent:UpdatePrimitiveComponentTransform_GameThread");

            if (const auto &sceneSP = m_sceneWP.lock())
            {
                if (const auto &sceneRendererSp = sceneSP->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                {
                    const auto updateSuccessfull = sceneRendererSp->UpdatePrimitiveComponentTransform_OnRenderThread(SceneProxyId,
                                                                                                             GetObjectId(),
                                                                                                             functionId,
                                                                                                             m_relativeMatrix,
                                                                                                             GetTransformedBoundingBox());

                    SetIsTransformationDirty(!updateSuccessfull);
                }
            }
        }
    }

    size_t ParticleSystemComponent::GetParticlesCount() const
    {
        return mParticlesPool.size();
    }

    void ParticleSystemComponent::SetParticleEmitter(const std::shared_ptr<IEmitter> &emitter)
    {
        assert(!mParticleEmitter);
        mParticleEmitter = emitter;
    }

    void ParticleSystemComponent::SyncDataWithRenderThread(const size_t activeParticlesCount)
    {
        static const uint64_t functionId = Hash("ParticleSystemComponent: SyncDataWithRenderThread");
        if (const auto &sceneSp = m_sceneWP.lock())
        {
            if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                                                                                    GetObjectId(),
                                                                                    functionId,
                                                                                    [=]() mutable
                                                                                    {
                                                                                        const auto &primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(SceneProxyId);
                                                                                        assert(primitiveProxySp);
                                                                                        const auto &proxyPtr =
                                                                                            std::static_pointer_cast<ParticleSystemSceneProxy>(primitiveProxySp);

                                                                                        if (activeParticlesCount > 0)
                                                                                        {
                                                                                            proxyPtr->CopyParticlesRawData(mParticlesRawDataHandler.GetTranslationData(),
                                                                                                                           mParticlesRawDataHandler.GetTranslationActiveDataChunkSize(),
                                                                                                                           mParticlesRawDataHandler.GetRotationSizeData(),
                                                                                                                           mParticlesRawDataHandler.GetRotationSizeActiveDataChunkSize(),
                                                                                                                           mParticlesRawDataHandler.GetColorData(),
                                                                                                                           mParticlesRawDataHandler.GetColorActiveDataChunkSize());
                                                                                        }

                                                                                        proxyPtr->SetActiveParticlesCount(activeParticlesCount);
                                                                                    });
            }
        }
    }
}