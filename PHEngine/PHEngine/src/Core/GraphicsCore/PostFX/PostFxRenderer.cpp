#include "PostFxRenderer.h"

#include "Bloom/BloomPostFxPass.h"

namespace Graphics
{

   PostFxRenderer::PostFxRenderer(const ViewPortInfo &viewPortInfo)
       : mEnabledFxBits(0),
         mPostFxStages(),
         mViewPortInfo(viewPortInfo)
   {
      Init();
   }

   PostFxRenderer::~PostFxRenderer()
   {
      for (const auto &postFxInstancePair : mPostFxStages)
      {
         postFxInstancePair.second->CleanUp();
      }
   }

   void PostFxRenderer::Init()
   {
      mPostFxStages.emplace(ePostFxStageIndex::STAGE_1, std::make_shared<BloomPostFxPass>(mViewPortInfo));
      mEnabledFxBits.set(static_cast<uint8_t>(ePostFxStageIndex::STAGE_1), true);
   }

   void PostFxRenderer::Execute(const std::shared_ptr<ITexture> &sceneColor)
   {
      if (mEnabledFxBits.test(static_cast<uint8_t>(ePostFxStageIndex::STAGE_1)))
      {
         mPostFxStages.at(ePostFxStageIndex::STAGE_1)->ExecutePostFx(sceneColor);
      }
   }
}