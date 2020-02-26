#pragma once

#include <vector>
#include <stdint.h>
#include <memory>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gl/glew.h>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingGBuffer.h"
#include "Core/GameCore/ShaderImplementation/TextureRendererShader.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"

using namespace Graphics::Texture;
using namespace Graphics::Proxy;
using namespace Graphics;

using namespace Game::ShaderImpl;

namespace Debug
{

   class TextureRenderer
   {
      std::shared_ptr<TextureRendererShader> m_shader;
      std::vector<std::shared_ptr<ITexture>> frameTextures;

   public:

#if DEBUG

      const size_t MAX_FRAME_COUNT = 3;

      size_t DebugRenderTargetIndex = 0;
#endif

      TextureRenderer();

      ~TextureRenderer();

#if DEBUG
      void PushDebugRenderTarget();
#endif

      void PushFrame(std::shared_ptr<ITexture> texture);

      void PopFrame();

      void RenderFrames(const std::unique_ptr<DeferredShadingGBuffer>& gbuffer);

      void RenderFullScreenInputTexture(std::shared_ptr<ITexture> renderTexture, glm::ivec2 screenRezolution);

      void RenderSeparatedScreen(std::shared_ptr<ITexture> separatedTexture, glm::ivec2 screenRezolution);

      void CleanUp();

   private:

      glm::mat4 GetScreenSpaceMatrix(size_t layoutIndex) const;

      void Render(std::shared_ptr<ITexture> renderTexture, size_t index);

   };

}

