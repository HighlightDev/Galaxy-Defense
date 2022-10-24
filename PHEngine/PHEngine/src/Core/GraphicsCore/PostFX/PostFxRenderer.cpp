#include "PostFxRenderer.h"

#include "Bloom/BloomPostFxPass.h"

namespace Graphics
{

   PostFxRenderer::PostFxRenderer()
      : mPostFxStages()
   {
      Init();
   }

   PostFxRenderer::~PostFxRenderer()
   {
   }

   void PostFxRenderer::Init()
   {
      mPostFxStages.emplace({ePostFxStageIndex::STAGE_1, std::make_shared<BloomPostFxPass>()});
   }

   void PostFxRenderer::Execute(const std::shared_ptr<ITexture>& sceneColor)
   {

   }
}