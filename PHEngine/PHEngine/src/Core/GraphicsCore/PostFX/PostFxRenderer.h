#pragma once

#include "IPostFxPass.h"

#include <unordered_map>
#include <memory>

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

      std::unordered_map<ePostFxStageIndex, std::shared_ptr<IPostFxPass>> mPostFxStages;

   public:
      PostFxRenderer();
      ~PostFxRenderer();

      void Execute(const std::shared_ptr<ITexture>& sceneColor);

   private:

      void Init();
   };

}

