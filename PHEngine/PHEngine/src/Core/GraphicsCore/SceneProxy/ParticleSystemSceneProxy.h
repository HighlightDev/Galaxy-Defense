#pragma once

#include "PrimitiveSceneProxy.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GameCore/Particles/ParticlesRawDataHandler.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/InstancedStaticMeshVertexFactory.h"

#include <vector>

using namespace IO;
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

            size_t mActiveParticlesCount;

            using Base = PrimitiveSceneProxy;
            using ParticleShader_t = VertexFactoryCompositeShader<InstancedStaticMeshVertexFactory, SimpleShader>;

        public:
            ParticleSystemSceneProxy(const ::EngineCore::ParticleSystemComponent *component);

            ~ParticleSystemSceneProxy();

            virtual void Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

            virtual bool IsDeferred() const override;

            virtual eMeshFacing GetMeshFrontFace() const override;

            virtual bool IsFrustumCullTestNeeded() const override;

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
