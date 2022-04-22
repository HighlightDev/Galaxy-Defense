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
#include <algorithm>
#include <iterator>
#include <TinyLogger/LogInterface.h>

using namespace Graphics::Proxy;
using namespace Graphics;
using namespace EngineMath;
using namespace TinyLogger;

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
          mParticlesRawDataHandler(200),
          mRenderData(renderData)
    {
        mParticlesPool.resize(200);
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
                particleIt->Position = particleIt->Position + (particleIt->Velocity * 100.0f * deltaTime);
                particleIt->Velocity += (EngineMath::G * -AXIS_UP) * deltaTime * 2.0f;
                particleIt->LifeRemaining -= deltaTime;

                const float invLife = particleIt->LifeTime - particleIt->LifeRemaining;

                mParticlesRawDataHandler.SubTranslationData(particleTranslationByteOffset, particleIt->Position);
                particleTranslationByteOffset += mParticlesRawDataHandler.GetTranslationVectorByteDataOffset();

                mParticlesRawDataHandler.SubRotationSizeData(particleRotationSizeByteOffset,
                                                             particleIt->Rotation,
                                                             EngineMath::LerpFloat(invLife,
                                                                                   0.0f,
                                                                                   particleIt->LifeTime,
                                                                                   particleIt->SizeBegin,
                                                                                   particleIt->SizeEnd));
                particleRotationSizeByteOffset += mParticlesRawDataHandler.GetRotationSizeByteDataOffset();

                mParticlesRawDataHandler.SubColorData(particleColorByteOffset,
                                                      EngineMath::LerpVec4(invLife,
                                                                           0.0f,
                                                                           particleIt->LifeTime,
                                                                           particleIt->ColorBegin,
                                                                           particleIt->ColorEnd));
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
            const float random_theta_rad = Random::Float() * EngineMath::PI * 2;
            const float random_phi_rad = Random::Float() * EngineMath::PI;

            Particle &p = mParticlesPool[i];

            p.Position = glm::vec3(
                random_radius * std::cos(random_theta_rad) * std::sin(random_phi_rad),
                random_radius * std::sin(random_theta_rad) * std::sin(random_theta_rad),
                random_radius * std::cos(random_phi_rad));
            p.Rotation = Random::Float() * EngineMath::PI * 2;

            const auto randomNormalizedValue = []()
            {
                return (Random::Float() * 2.0f) - 1.0f;
            };

            p.Velocity = glm::vec3(randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f);

            p.ColorBegin = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            p.ColorEnd = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            p.SizeBegin = 0.5f;
            p.SizeEnd = 0.1f;
            p.LifeTime = 1.0f;
            p.LifeRemaining = p.LifeTime;

            p.isActive = true;
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

            SetIsTransformationDirty(false);

            // Update primitives proxy transform
            static const uint64_t functionId = Hash("ParticleSystemComponent:UpdatePrimitiveComponentTransform_GameThread");

            if (const auto &sceneSP = m_sceneWP.lock())
            {
                sceneSP->UpdatePrimitiveComponentTransform_OnRenderThread(SceneProxyId,
                                                                          GetObjectId(),
                                                                          functionId,
                                                                          m_relativeMatrix,
                                                                          GetTransformedBoundingBox());
            }
        }
    }

    void ParticleSystemComponent::SyncDataWithRenderThread(const size_t activeParticlesCount)
    {
        static const uint64_t functionId = Hash("ParticleSystemComponent: SyncDataWithRenderThread");
        if (const auto &sceneSP = m_sceneWP.lock())
        {
            if (const auto &sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
            {
                sceneSP->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
                                               GetObjectId(),
                                               functionId,
                                               [=]() mutable
                                               {
                                                   const auto& primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(SceneProxyId);
                                                   assert(primitiveProxySp);
                                                   ParticleSystemSceneProxy *const proxyPtr =
                                                       static_cast<ParticleSystemSceneProxy *>(primitiveProxySp.get());

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