#pragma once

#include "Core/GraphicsCore/PostFX/IPostFxPass.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/FramebufferImplementation/BloomFramebuffer.h"
#include "Core/GameCore/ShaderImplementation/BloomFxShader.h"

using namespace EngineCore::FramebufferImpl;
using namespace EngineCore::ShaderImpl;

namespace Graphics
{
   class BloomPostFxPass : public IPostFxPass
   {
   private:
      std::unique_ptr<BloomFramebuffer> mBloomFramebuffer;
      std::shared_ptr<BloomFxShader> mBloomFxShader;
      uint32_t mBlurPassCount;

      std::shared_ptr<ITexture> mBloomFxResult;

      ViewPortInfo mViewPortInfo;

   public:
      BloomPostFxPass(const ViewPortInfo &viewPortInfo);

      virtual ~BloomPostFxPass();

      void ExecutePostFx(const std::shared_ptr<ITexture> &sceneColorTexture, const std::shared_ptr<FramebufferBundle>& previousStepFramebuffer) override;

      virtual std::shared_ptr<ITexture> GetPostFxResult() const;

      void CleanUp() override;

      void ResizeRenderTargets(const ViewPortInfo &viewPortInfo) override;

   private:
      void Init();
   };
}
