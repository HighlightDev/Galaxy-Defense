#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace Graphics::OpenGL;

namespace EngineCore
{
    namespace ShaderImpl
    {
        class FontRenderingShader : public Shader
        {

        private:
            Uniform u_fontAtlas, u_position, u_color, u_shadowWidth, u_shadowOffset;

        public:
            FontRenderingShader();

            virtual ~FontRenderingShader();

            FontRenderingShader(const ShaderParams &params);

            void SetFontAtlasSlot(const int32_t slot);

            void SetPosition(const glm::vec2& position);

            void SetColor(const glm::vec3& color);

            void SetShadowWidth(const float shadowWidth);

            void SetShadowOffset(const glm::vec2& shadowOffset);

        protected:

            virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

            virtual void SetShaderPredefine() override;
        };

    }
}
