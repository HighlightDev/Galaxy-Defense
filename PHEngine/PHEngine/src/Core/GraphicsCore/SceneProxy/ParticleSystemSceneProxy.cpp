#include "ParticleSystemSceneProxy.h"

#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"

namespace Graphics
{

    namespace Proxy
    {
        ParticleSystemSceneProxy::ParticleSystemSceneProxy(const ParticleSystemComponent *component)
            : PrimitiveSceneProxy(component->IsEnabled(),
                                  component->IsVisible(),
                                  component->GetRelativeMatrix(),
                                  nullptr, /*component->GetRenderData().m_skin,*/
                                  nullptr,
                                  nullptr,
                                  nullptr),
              mParticles()
        {
            mShader = std::make_shared<ParticleShader>();
        }

        ParticleSystemSceneProxy::~ParticleSystemSceneProxy()
        {
        }

        void ParticleSystemSceneProxy::Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            if (!mParticles.size())
                return;
            // enable blending
            mShader->ExecuteShader();
            for (const auto &particle : mParticles)
            {
                glm::mat4 translation = glm::translate(glm::mat4(1), particle.Position);
                glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(particle.Size, particle.Size, 1.0f));
                glm::mat4 rotation  = glm::rotate(glm::mat4(1), particle.Rotation, glm::vec3(0, 0, 1));
                mShader->SetTransformMatrices(scale * translation * rotation, viewMatrix, projectionMatrix);
                // skin render
            }
            mShader->StopShader();
            // unbind shader
        }

        bool ParticleSystemSceneProxy::IsDeferred() const
        {
            return false;
        }

        eMeshFacing ParticleSystemSceneProxy::GetMeshFrontFace() const
        {
            return eMeshFacing::COUNTER_CLOCK_WISE;
        }

        bool ParticleSystemSceneProxy::IsFrustumCullTestNeeded() const
        {
            return false;
        }

        void ParticleSystemSceneProxy::SetParticleProxyProperties(std::vector<ParticleProxyProperties> &&properties)
        {
            mParticles = std::move(properties);
        }
    }
}
