#include "PostFxRenderer.h"

#include "Bloom/BloomPostFxPass.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderParams.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/GraphicsCore/Renderer/ResolvedSceneFramebuffer.h"

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
         mPostFxStages.emplace(ePostFxStageType::BLOOM_STAGE, std::make_shared<BloomPostFxPass>(mViewPortInfo));
         mEnabledFxBits.set(static_cast<uint8_t>(ePostFxStageType::BLOOM_STAGE), true);
      }
   }

   void PostFxRenderer::Execute(const std::shared_ptr<ResolvedSceneFramebuffer> &resolveSceneColorFramebuffer)
   {
      if (mEnabledFxBits.test(static_cast<uint8_t>(ePostFxStageType::BLOOM_STAGE)))
      {
         const auto &postFx = std::static_pointer_cast<BloomPostFxPass>(mPostFxStages.at(ePostFxStageType::BLOOM_STAGE));
         postFx->ExecutePostFx(resolveSceneColorFramebuffer->GetResolvedSceneColorTexture(), resolveSceneColorFramebuffer);
         mFxColorResolver->Execute(resolveSceneColorFramebuffer->GetResolvedSceneColorTexture(), postFx);
      }

      if (!mEnabledFxBits.any())
      {
         mFxColorResolver->Execute(resolveSceneColorFramebuffer->GetResolvedSceneColorTexture(), nullptr);
      }
   }
}