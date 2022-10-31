#pragma once

#include "Core/GraphicsCore/PostFX/IPostFxPass.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "BloomFramebuffer.h"

namespace Graphics
{
   class BloomPostFxPass : public IPostFxPass
   {
   private:
      std::unique_ptr<BloomFramebuffer> mBloomFramebuffer;

      ViewPortInfo mViewPortInfo;

   public:
      BloomPostFxPass(const ViewPortInfo &viewPortInfo);
      virtual ~BloomPostFxPass();

      virtual std::shared_ptr<ITexture> ExecutePostFx(const std::shared_ptr<ITexture> &sceneColorTexture) override;

      virtual void CleanUp() override;
   };
}
