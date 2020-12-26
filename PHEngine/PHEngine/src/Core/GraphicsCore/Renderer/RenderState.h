#pragma once

#include <stdint.h>
#include <gl/glew.h>
#include <memory>

namespace Graphics
{
   struct IDepthStencilState
   {
      virtual void BindDepthStencilState() = 0;
   };

   struct IBlendingState
   {
      virtual void BindBlendState() = 0;
   };

   template <
      bool depthTestEnabled = true,
      int32_t depthFunc = GL_LEQUAL,
      int32_t stencilFunc = GL_ALWAYS,
      int32_t stencilRef = 0x0,
      int32_t stencilMask = 0x1>
      struct DepthStencilState : public IDepthStencilState
   {
      virtual void BindDepthStencilState() override
      {
         glEnable(GL_DEPTH_TEST);
         glDepthFunc(depthFunc);
         //glEnable(GL_STENCIL_TEST);
         //glStencilFunc(stencilFunc, stencilRef, stencilMask);
      }
   };

   template <int32_t depthFunc,
      int32_t stencilFunc,
      int32_t stencilRef,
      int32_t stencilMask>
   struct DepthStencilState<false, depthFunc, stencilFunc, stencilRef, stencilMask> : public IDepthStencilState
   {
      virtual void BindDepthStencilState() override
      {
         glDisable(GL_DEPTH_TEST);
      }
   };

   template <bool bEnableBlending = true,
      int32_t srcFactor = GL_SRC_ALPHA,
      int32_t dstFactor = GL_ONE_MINUS_SRC_ALPHA>
   struct BlendingState : public IBlendingState
   {
      virtual void BindBlendState() override
      {
         glEnable(GL_BLEND);
         glBlendFunc(srcFactor, dstFactor);
      }
   };

   template <int32_t srcFactor,
      int32_t dstFactor>
      struct BlendingState<false, srcFactor, dstFactor> : public IBlendingState
   {
      virtual void BindBlendState() override
      {
         glDisable(GL_BLEND);
      }
   };

   class RenderState
   {
      std::shared_ptr<IDepthStencilState> mDepthState;

      std::shared_ptr<IBlendingState> mBledingState;

   public:

      RenderState(std::shared_ptr<IDepthStencilState> depthState, std::shared_ptr<IBlendingState> blendState);

      void BindRenderState();
   };
}
