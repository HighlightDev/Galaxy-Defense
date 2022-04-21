#pragma once

#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Particles/ParticlesRawDataHandler.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/IoCore/FolderManager.h"

#include <vector>

using namespace EngineCore;
using namespace IO;

namespace EngineCore
{
    class ParticleSystemComponent;
}

namespace Graphics
{
    struct ParticleShader : public Shader
    {

    private:
        Uniform u_worldMatrix, u_viewMatrix, u_projectionMatrix;
        Uniform u_color;

    public:
        ParticleShader(const ShaderParams &shaderParams)
            : Shader(shaderParams)
        {
            ShaderInit();
        }

        virtual ~ParticleShader()
        {
        }

        void SetTransformMatrices(const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            u_worldMatrix.LoadUniform(worldMatrix);
            u_viewMatrix.LoadUniform(viewMatrix);
            u_projectionMatrix.LoadUniform(projectionMatrix);
        }

        void SetColor(const glm::vec4 &color)
        {
            u_color.LoadUniform(color);
        }

    protected:
        virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override
        {
            Shader::AccessAllUniformLocations(shaderProgramID);

            u_worldMatrix = GetUniform("worldMatrix", shaderProgramID);
            u_viewMatrix = GetUniform("viewMatrix", shaderProgramID);
            u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramID);
            u_color = GetUniform("color", shaderProgramID);
        }

        virtual void SetShaderPredefine() override
        {
        }
    };

    namespace Proxy
    {
        class ParticleSystemSceneProxy : public PrimitiveSceneProxy
        {
            ParticlesRawDataHandler mParticlesRawDataHandler;
            size_t mActiveParticlesCount;

            std::shared_ptr<ParticleShader> mShader;

            using Base = PrimitiveSceneProxy;

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
