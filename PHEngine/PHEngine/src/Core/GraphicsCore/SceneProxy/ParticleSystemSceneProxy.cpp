#include "ParticleSystemSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/ParticlesPool.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/ParticlePoolParameters.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/UtilityCore/EngineMath.h"

#include <stdlib.h>

using namespace EngineMath;
using namespace Resources;
using namespace TinyLogger;
using namespace Graphics::Renderer;
using namespace Graphics::OpenGL;
using namespace EngineCore;

namespace Graphics {
namespace Proxy {
ParticleSystemSceneProxy::ParticleSystemSceneProxy(const ParticleSystemComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , mParticlesRawDataHandler(component->GetParticlesCount())
    , mRenderData(component->GetRenderData())
    , mActiveParticlesCount(0)
{
}

void ParticleSystemSceneProxy::PostConstructorInitialize()
{
    static constexpr uint64_t functionId = Hash64_CT("ParticleSystemSceneProxy::PostConstructorInitialize");

    const ShaderParams particlesShaderParams(
        "ParticleShader",
        FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleFS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + SLASH + "particleGS.glsl");

    CompositeShaderParams particlesCompositeShaderParams("ParticleVertexFactory_SimpleShader", particlesShaderParams);

    m_shader = CreateMaterialShader<ParticleVertexFactory, SimpleShader>(
        "ParticleVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, particlesShaderParams, mMaterialProxy);

    mRenderData.mParticleMeshParams.mVertexAttributes = GetShader()->GetVertexAttributes();

    m_skin = ParticlesPool::GetInstance()->GetOrAllocateResource(mRenderData.mParticleMeshParams);

    if (const auto& deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock()) {
        if (const auto& sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock()) {
            const auto boundingBox = m_skin->GetBoundingBox();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mSceneProxyId, functionId, [this, boundingBox, sceneSp]() {
                    const auto& engineObject = sceneSp->GetEngineObjectById(GetGameObjectId());
                    assert(engineObject);
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    assert(primitiveComponent);
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

ParticleSystemSceneProxy::~ParticleSystemSceneProxy()
{
}

std::shared_ptr<typename ParticleSystemSceneProxy::ParticleShader_t> ParticleSystemSceneProxy::GetShader() const
{
    return std::static_pointer_cast<ParticleSystemSceneProxy::ParticleShader_t>(m_shader);
}

void ParticleSystemSceneProxy::CleanUp()
{
    PrimitiveSceneProxy::CleanUp();
}

void ParticleSystemSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (!mActiveParticlesCount)
        return;

    const auto& shader = GetShader();

    PrepareParticlesInstancedBuffer();
    shader->ExecuteShader();
    shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
    shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
    m_skin->GetBuffer()->RenderInstanced(GL_POINTS, mActiveParticlesCount);
    shader->StopShader();
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

void ParticleSystemSceneProxy::SetActiveParticlesCount(const size_t activeParticlesCount)
{
    mActiveParticlesCount = activeParticlesCount;
}

void ParticleSystemSceneProxy::CopyParticlesRawData(
    const void* translationBuffer,
    const size_t translationByteChunkSize,
    const void* rotationSizeBuffer,
    const size_t rotationByteChunkSize,
    const void* colorBuffer,
    const size_t colorByteChunkSize)
{
    if (translationByteChunkSize > 0) {
        mParticlesRawDataHandler.CopyToMeActiveTranslationData(translationBuffer, 0, translationByteChunkSize);
        mParticlesRawDataHandler.SetTranslationActiveDataChunkSize(translationByteChunkSize);
    }

    if (rotationByteChunkSize > 0) {
        mParticlesRawDataHandler.CopyToMeActiveRotationSizeData(rotationSizeBuffer, 0, rotationByteChunkSize);
        mParticlesRawDataHandler.SetRotationSizeActiveDataChunkSize(rotationByteChunkSize);
    }

    if (colorByteChunkSize > 0) {
        mParticlesRawDataHandler.CopyToMeActiveColorData(colorBuffer, 0, colorByteChunkSize);
        mParticlesRawDataHandler.SetColorActiveDataChunkSize(colorByteChunkSize);
    }

    bIsParticlesTransformDirty = true;
}

void ParticleSystemSceneProxy::PrepareParticlesInstancedBuffer()
{
    if (!bIsParticlesTransformDirty)
        return;
    auto* const particlesTransformVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("ParticleRelativeOffset");
    auto* const particlesRotationSizeVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("ParticleRotationAndSize");
    auto* const particlesColorVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("ParticleColor");

    assert(particlesTransformVBO && particlesRotationSizeVBO && particlesColorVBO);

    const size_t translationSubBufferSize = mParticlesRawDataHandler.GetTranslationActiveDataChunkSize();
    const size_t rotationSizeSubBufferSize = mParticlesRawDataHandler.GetRotationSizeActiveDataChunkSize();
    const size_t colorBufferSize = mParticlesRawDataHandler.GetColorActiveDataChunkSize();
    particlesTransformVBO->BindVBO();
    particlesTransformVBO->BufferSubData(0, translationSubBufferSize, mParticlesRawDataHandler.GetTranslationData());

    particlesRotationSizeVBO->BindVBO();
    particlesTransformVBO->BufferSubData(0, rotationSizeSubBufferSize, mParticlesRawDataHandler.GetRotationSizeData());

    particlesColorVBO->BindVBO();
    particlesColorVBO->BufferSubData(0, colorBufferSize, mParticlesRawDataHandler.GetColorData());

    particlesTransformVBO->UnbindVBO();

    bIsParticlesTransformDirty = false;
}
} // namespace Proxy
} // namespace Graphics
