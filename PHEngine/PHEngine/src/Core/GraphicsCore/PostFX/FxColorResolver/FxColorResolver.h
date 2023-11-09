#pragma once

#include "Core/GraphicsCore/PostFX/IPostFxPass.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/ShaderImplementation/ResolveFxColorShader.h"

using namespace EngineCore::ShaderImpl;

namespace Graphics
{
   class FxColorResolver
   {
   private:
      std::shared_ptr<ResolveFxColorShader> mResolveFxColorShader;

      ViewPortInfo mViewPortInfo;

   public:
      FxColorResolver(const ViewPortInfo &viewPortInfo);
      virtual ~FxColorResolver();

      virtual void Execute(const std::shared_ptr<ITexture>& sceneColorTexture, const std::shared_ptr<IPostFxPass>& prevPostFxPass);

      virtual void CleanUp();

      void ResizeViewPortInfo(const ViewPortInfo &viewPortInfo);

   private:
      void Init();

      void ExecutePostFxDisabled(const std::shared_ptr<ITexture>& sceneColorTexture);
      void ExecutePostFxEnabled(const std::shared_ptr<ITexture>& sceneColorTexture, const std::shared_ptr<IPostFxPass>& prevPostFxPass);
   };
}