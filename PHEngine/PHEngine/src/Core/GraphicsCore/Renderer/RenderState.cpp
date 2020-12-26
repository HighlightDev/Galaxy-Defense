#include "RenderState.h"

namespace Graphics
{
   RenderState::RenderState(std::shared_ptr<IDepthStencilState> depthState, std::shared_ptr<IBlendingState> blendState)
      : mDepthState(depthState)
      , mBledingState(blendState)
   {
   }

   void RenderState::BindRenderState()
   {
      if (mDepthState)
         mDepthState->BindDepthStencilState();

      if (mBledingState)
         mBledingState->BindBlendState();
   }
}