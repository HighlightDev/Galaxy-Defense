#pragma once

#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Particles/ParticleProxyProperties.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

#include <vector>

using namespace EngineCore;

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

    public:
        ParticleShader()
            : Shader(ShaderParams{
                  .ShaderName = "ParticleShader",
                  .VertexShaderFile = "",
                  .FragmentShaderFile = ""})
        {
            ShaderInit();
        }

        virtual ~ParticleShader()
        {
        }

        void SetTransformMatrices(const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
        {
            u_worldMatrix.LoadUniform(worldMatrix);
            u_worldMatrix.LoadUniform(viewMatrix);
            u_worldMatrix.LoadUniform(projectionMatrix);
        }

    protected:
        virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override
        {
            Shader::AccessAllUniformLocations(shaderProgramID);

            u_worldMatrix = GetUniform("worldMatrix", shaderProgramID);
            u_viewMatrix = GetUniform("viewMatrix", shaderProgramID);
            u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramID);
        }

        virtual void SetShaderPredefine() override
        {
        }
    };

    namespace Proxy
    {
        class ParticleSystemSceneProxy : public PrimitiveSceneProxy
        {

            // std::shared_ptr<BillboardShader> m_billboardShader;

            std::vector<ParticleProxyProperties> mParticles;

            std::shared_ptr<ParticleShader> mShader;

            using Base = PrimitiveSceneProxy;

        public:
            ParticleSystemSceneProxy(const ::EngineCore::ParticleSystemComponent *component);

            ~ParticleSystemSceneProxy();

            virtual void Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

            virtual bool IsDeferred() const override;

            virtual eMeshFacing GetMeshFrontFace() const override;

            virtual bool IsFrustumCullTestNeeded() const override;

            void SetParticleProxyProperties(std::vector<ParticleProxyProperties> &&properties);
        };
    }
}
