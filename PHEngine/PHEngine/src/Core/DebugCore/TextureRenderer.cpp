#include "TextureRenderer.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/CommonCore/FolderManager.h"

#include <utility>

using namespace Resources;

namespace Debug
{

   TextureRenderer::TextureRenderer()
   {
      const auto& folderManager = Common::FolderManager::GetInstance();
      ShaderParams shaderParams("Texture Renderer Shader", folderManager->GetShadersPath() + "uiVS.glsl", folderManager->GetShadersPath() + "uiFS.glsl", "", "", "", "");
      m_shader = std::static_pointer_cast<TextureRendererShader>(ShaderPool::GetInstance()->template GetOrAllocateResource<TextureRendererShader>(shaderParams));
   }

   TextureRenderer::~TextureRenderer()
   {
      CleanUp();
   }

#if DEBUG
   void TextureRenderer::PushDebugRenderTarget()
   {
      size_t totalCount = RenderTargetPool::GetInstance()->GetResourcesCount();
      DebugRenderTargetIndex = DebugRenderTargetIndex > (totalCount - 1) ? 0 : DebugRenderTargetIndex;

      PushFrame(RenderTargetPool::GetInstance()->GetRenderTargetAt(DebugRenderTargetIndex));

      ++DebugRenderTargetIndex;
   }
#endif

   void TextureRenderer::PushFrame(std::shared_ptr<ITexture> texture)
   {
      if (frameTextures.size() >= MAX_FRAME_COUNT)
         PopFrame();

      frameTextures.push_back(texture);
   }

   void TextureRenderer::PopFrame()
   {
      if (frameTextures.size() > 0)
      {
         frameTextures.erase(frameTextures.begin());
      }
   }

   void TextureRenderer::RenderFrames(const std::unique_ptr<DeferredShadingGBuffer>& gbuffer)
   {
      for (size_t i = 0; i < frameTextures.size(); i++)
      {
         std::shared_ptr<ITexture> frameTexture = frameTextures[i];
         Render(frameTexture, i);
      }
   }

   glm::mat4 TextureRenderer::GetScreenSpaceMatrix(size_t layoutIndex) const
   {
      glm::mat4 resultMatrix = glm::mat4(1);

      glm::vec2 Origin = glm::vec2(-0.75f, -0.7f);
      glm::vec2 translation = glm::vec2(Origin.x, ((float)(layoutIndex) * 0.5f) + (0.15f * (float)(layoutIndex)) + Origin.y);

      resultMatrix = glm::translate(resultMatrix, glm::vec3(translation.x, translation.y, 0.0f));
      resultMatrix = glm::scale(resultMatrix, glm::vec3(0.2f, 0.25f, 1.0f));

      return resultMatrix;
   }

   void TextureRenderer::Render(std::shared_ptr<ITexture> renderTexture, size_t index)
   {
      int32_t texturePixelFormat = renderTexture->GetTextureParameters().TexPixelFormat;
      const bool bDepthTexture = texturePixelFormat == GL_DEPTH_COMPONENT;

      const glm::mat4& screenSpaceMatrix = GetScreenSpaceMatrix(index);
      m_shader->ExecuteShader();
      renderTexture->BindTexture(0);
      m_shader->SetIsDepthTexture(bDepthTexture);
      m_shader->SetIsSeparatedScreen(false);
      m_shader->SetUiTextureSampler(0);
      m_shader->SetScreenSpaceMatrix(screenSpaceMatrix);
      ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
      m_shader->StopShader();
   }

   void TextureRenderer::RenderFullScreenInputTexture(std::shared_ptr<ITexture> renderTexture, glm::ivec2 screenRezolution)
   {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, screenRezolution.x, screenRezolution.y);
      m_shader->ExecuteShader();
      renderTexture->BindTexture(0);
      m_shader->SetUiTextureSampler(0);
      m_shader->SetScreenSpaceMatrix(glm::mat4(1));
      m_shader->SetIsDepthTexture(false);
      m_shader->SetIsSeparatedScreen(false);
      ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
      m_shader->StopShader();
   }

   void TextureRenderer::RenderSeparatedScreen(std::shared_ptr<ITexture> separatedTexture, glm::ivec2 screenRezolution)
   {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, screenRezolution.x, screenRezolution.y);
      m_shader->ExecuteShader();
      separatedTexture->BindTexture(0);
      m_shader->SetUiTextureSampler(0);
      m_shader->SetIsDepthTexture(false);
      m_shader->SetIsSeparatedScreen(true);
      m_shader->SetScreenSpaceMatrix(glm::mat4(1));
      ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
      m_shader->StopShader();
   }

   void TextureRenderer::CleanUp()
   {
      ShaderPool::GetInstance()->TryToFreeMemory(m_shader);
   }

}