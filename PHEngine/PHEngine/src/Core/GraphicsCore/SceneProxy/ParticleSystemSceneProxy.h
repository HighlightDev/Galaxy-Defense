#pragma once

#include "PrimitiveSceneProxy.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GameCore/Particles/ParticlesRawDataHandler.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/ParticleVertexFactory.h"
#include "Core/GameCore/Particles/ParticlePoolParameters.h"
#include "Core/GraphicsCore/RenderData/ParticleSystemRenderData.h"

#include <vector>

using namespace IO;
using namespace Graphics::Data;
using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace EngineCore
{
    class ParticleSystemComponent;
}

namespace Graphics
{
    namespace Proxy
    {
        class ParticleSystemSceneProxy : public PrimitiveSceneProxy
        {
            ParticlesRawDataHandler mParticlesRawDataHandler;

            ParticleSystemRenderData mRenderData;

            size_t mActiveParticlesCount;

            bool bIsParticlesTransformDirty{false};

            using Base = PrimitiveSceneProxy;
            using ParticleShader_t = VertexFactoryMaterialCompositeShader<ParticleVertexFactory, SimpleShader>;

        public:
            ParticleSystemSceneProxy(const ::EngineCore::ParticleSystemComponent *component);

            ~ParticleSystemSceneProxy() override;

            void CleanUp() override;

            void Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

            void PostConstructorInitialize() override;

            bool IsDeferred() const override;

            eMeshFacing GetMeshFrontFace() const override;

            bool IsFrustumCullTestNeeded() const override;

            void SetActiveParticlesCount(const size_t activeParticlesCount);

            void CopyParticlesRawData(const void *translationBuffer,
                                      const size_t translationByteChunkSize,
                                      const void *rotationSizeBuffer,
                                      const size_t rotationByteChunkSize,
                                      const void *colorBuffer,
                                      const size_t colorByteChunkSize);

        private:
            void PrepareParticlesInstancedBuffer();
        };
    }
}
