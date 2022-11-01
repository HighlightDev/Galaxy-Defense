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
      std::unique_ptr<BloomFxShader> mBloomFxShader;

      ViewPortInfo mViewPortInfo;

   public:
      BloomPostFxPass(const ViewPortInfo &viewPortInfo);
      virtual ~BloomPostFxPass();

      virtual std::shared_ptr<ITexture> ExecutePostFx(const std::shared_ptr<ITexture> &sceneColorTexture) override;

      virtual void CleanUp() override;
   };
}
