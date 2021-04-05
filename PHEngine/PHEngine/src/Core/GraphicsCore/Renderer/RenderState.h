#pragma once

#include <stdint.h>
#include <gl/glew.h>

namespace Graphics
{
   template <
      bool depthTestEnabled = true,
      int32_t depthFunc = GL_LEQUAL,
      int32_t stencilFunc = GL_ALWAYS,
      int32_t stencilRef = 0x0,
      int32_t stencilMask = 0x1>
      struct DepthStencilState
   {
      static void BindDepthStencilState()
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
   struct DepthStencilState<false, depthFunc, stencilFunc, stencilRef, stencilMask>
   {
      static void BindDepthStencilState()
      {
         glDisable(GL_DEPTH_TEST);
      }
   };

   template <bool bEnableBlending = true,
      int32_t srcFactor = GL_SRC_ALPHA,
      int32_t dstFactor = GL_ONE_MINUS_SRC_ALPHA>
   struct BlendingState
   {
      static void BindBlendState()
      {
         glEnable(GL_BLEND);
         glBlendFunc(srcFactor, dstFactor);
      }
   };

   template <int32_t srcFactor,
      int32_t dstFactor>
      struct BlendingState<false, srcFactor, dstFactor>
   {
      static void BindBlendState()
      {
         glDisable(GL_BLEND);
      }
   };

   // todo: implement for all cases (depth + stencil + blending)
   template <typename DepthStencilStateType, typename BlendingStateType>
   class RenderState
   {
      using depthStencilState_t = DepthStencilStateType;
      using blendState_t = BlendingStateType;

   public:

      void BindRenderState()
      {
         depthStencilState_t::BindDepthStencilState();
         blendState_t::BindBlendState();
      }
   };
}
