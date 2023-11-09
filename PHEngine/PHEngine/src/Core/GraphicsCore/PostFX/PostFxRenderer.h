#pragma once

#include "IPostFxPass.h"
#include "FxColorResolver/FxColorResolver.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <unordered_map>
#include <memory>
#include <bitset>

namespace Graphics
{
   enum class ePostFxStageType
   {
      BLOOM_STAGE
   };

   class ResolvedSceneFramebuffer;

   class PostFxRenderer
   {
   private:
      std::bitset<4> mEnabledFxBits;

      ViewPortInfo mViewPortInfo;

      std::unordered_map<ePostFxStageType, std::shared_ptr<IPostFxPass>> mPostFxStages;

      std::unique_ptr<FxColorResolver> mFxColorResolver;

   public:
      PostFxRenderer(const ViewPortInfo &viewPortInfo);
      ~PostFxRenderer();

      void Execute(const std::shared_ptr<ResolvedSceneFramebuffer> &resolveSceneColorFramebuffer);

      void ResizeRenderTargets(const ViewPortInfo &viewPortInfo);

   private:
      void Init();
   };

}
