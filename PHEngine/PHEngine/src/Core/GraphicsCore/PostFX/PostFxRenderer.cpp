#include "PostFxRenderer.h"

#include "Bloom/BloomPostFxPass.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderParams.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

#include <type_traits>

using namespace Resources;
using namespace EngineUtility;
using namespace IO;

namespace Graphics
{

   PostFxRenderer::PostFxRenderer(const ViewPortInfo &viewPortInfo)
       : mEnabledFxBits(0),
         mViewPortInfo(viewPortInfo),
         mPostFxStages(),
         mFxColorResolver(std::make_unique<FxColorResolver>(mViewPortInfo))
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
      const auto &cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
      if (cfg.IsBloomEnabled)
      {
         mPostFxStages.emplace(ePostFxStageIndex::STAGE_1, std::make_shared<BloomPostFxPass>(mViewPortInfo));
         mEnabledFxBits.set(static_cast<uint8_t>(ePostFxStageIndex::STAGE_1), true);
      }
   }

   void PostFxRenderer::Execute(const std::shared_ptr<ITexture> &sceneColor)
   {
      if (mEnabledFxBits.test(static_cast<uint8_t>(ePostFxStageIndex::STAGE_1)))
      {
         const auto &postFx = mPostFxStages.at(ePostFxStageIndex::STAGE_1);
         postFx->ExecutePostFx(sceneColor);
         mFxColorResolver->Execute(sceneColor, postFx);
      }

      if (!mEnabledFxBits.any())
      {
         mFxColorResolver->Execute(sceneColor, nullptr);
      }
   }
}