#include "ParticleSystemSceneProxy.h"

#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/CommonCore/Assertion.h"

#include <stdlib.h>
#include <TinyLogger/LogInterface.h>

using namespace EngineMath;
using namespace Resources;
using namespace TinyLogger;

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
              mParticles(),
              mParticlesRawDataHandler(1000 * sizeof(float) * 3)
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
            const auto particleCount = mParticles.size();
            if (!particleCount)
                return;

            PrepareParticlesInstancedBuffer();

            mShader->ExecuteShader();
            mShader->SetColor(mParticles.back().Color);
            mShader->SetParticleSize(mParticles.back().Size);
            mShader->SetTransformMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);

            Logger::Out("ParticleSystemSceneProxy::Render => particleCount: ", particleCount);
            m_skin->GetBuffer()->RenderInstanced(GL_POINTS, particleCount);
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

        void ParticleSystemSceneProxy::SetParticleProxyProperties(std::vector<ParticleProxyData> &&properties)
        {
            mParticles = std::move(properties);
        }

        void ParticleSystemSceneProxy::CopyParticlesRawData(const void *particlesRawData, const size_t byteChunkSize)
        {
            if (byteChunkSize > 0)
            {
                mParticlesRawDataHandler.CopyToMeActiveTranslationData(particlesRawData, 0, byteChunkSize);
                mParticlesRawDataHandler.SetActiveDataChunkSize(byteChunkSize);
            }
        }

        void ParticleSystemSceneProxy::PrepareParticlesInstancedBuffer()
        {
            auto *const particlesTransformVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::CUSTOM_0);

            assert(particlesTransformVBO);

            const size_t subBufferSize = mParticlesRawDataHandler.GetActiveDataChunkSize();
            // mParticles.size() * 3 * sizeof(float);

            // float *transformBuffer = (float *)malloc(subBufferSize);

            // size_t bufferIndex = 0;
            // for (const auto &particle : mParticles)
            // {
            //     transformBuffer[bufferIndex++] = particle.Position[0];
            //     transformBuffer[bufferIndex++] = particle.Position[1];
            //     transformBuffer[bufferIndex++] = particle.Position[2];
            // }

            Logger::Out("ParticleSystemSceneProxy::PrepareParticlesInstancedBuffer => subBufferSize: ", subBufferSize);
            particlesTransformVBO->BindVBO();
            particlesTransformVBO->BufferSubData(0, subBufferSize, mParticlesRawDataHandler.GetTranslationData());
            particlesTransformVBO->UnbindVBO();

            // free(transformBuffer);
        }
    }
}
