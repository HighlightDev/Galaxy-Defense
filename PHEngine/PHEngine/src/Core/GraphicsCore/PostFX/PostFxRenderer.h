#pragma once

#include "IPostFxPass.h"
#include "FxColorResolver/FxColorResolver.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <unordered_map>
#include <memory>
#include <bitset>

namespace Graphics
{
   enum class ePostFxStageIndex
   {
      STAGE_1,
      STAGE_2,
      STAGE_3,
      STAGE_4
   };

   class PostFxRenderer
   {
   private:
      std::bitset<4> mEnabledFxBits;

      ViewPortInfo mViewPortInfo;

      std::unordered_map<ePostFxStageIndex, std::shared_ptr<IPostFxPass>> mPostFxStages;

      std::unique_ptr<FxColorResolver> mFxColorResolver;

   public:
      PostFxRenderer(const ViewPortInfo &viewPortInfo);
      ~PostFxRenderer();

      void Execute(const std::shared_ptr<ITexture> &sceneColor);

   private:
      void Init();
   };

}
