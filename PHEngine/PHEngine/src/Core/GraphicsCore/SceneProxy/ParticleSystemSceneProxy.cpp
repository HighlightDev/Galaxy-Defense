#include "ParticleSystemSceneProxy.h"

#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

using namespace EngineMath;
using namespace Resources;

namespace Graphics
{

    namespace Proxy
    {
        ParticleSystemSceneProxy::ParticleSystemSceneProxy(const ParticleSystemComponent *component)
            : PrimitiveSceneProxy(component->IsEnabled(),
                                  component->IsVisible(),
                                  component->GetRelativeMatrix(),
                                  component->GetRenderData().m_skin,
                                  nullptr,
                                  nullptr,
                                  nullptr),
              mParticles()
        {
            mShader = ShaderPool::GetInstance()->GetOrAllocateResource<ParticleShader>(ShaderParams{
                .ShaderName = "ParticleShader",
                .VertexShaderFile = FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleVS.glsl",
                .FragmentShaderFile = FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleFS.glsl",
                .GeometryShaderFile = FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleGS.glsl"});
        }

        ParticleSystemSceneProxy::~ParticleSystemSceneProxy()
        {
        }

        void ParticleSystemSceneProxy::Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            if (!mParticles.size())
                return;

            mShader->ExecuteShader();
            for (const auto &particle : mParticles)
            {
                if (!particle.isActive)
                    continue;

                const glm::mat4 identityMatrix(1);
                glm::mat4 transformMatrix = identityMatrix;
                transformMatrix *= m_relativeMatrix;
                transformMatrix *= glm::translate(glm::mat4(1), particle.Position);
                transformMatrix *= glm::rotate(glm::mat4(1), DEG_TO_RAD(particle.Rotation), glm::vec3(0, 0, 1));

                mShader->SetColor(particle.Color);
                mShader->SetParticleSize(particle.Size);
                mShader->SetTransformMatrices(transformMatrix, viewMatrix, projectionMatrix);

                m_skin->GetBuffer()->RenderVAO(GL_POINTS);
            }
            mShader->StopShader();
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
