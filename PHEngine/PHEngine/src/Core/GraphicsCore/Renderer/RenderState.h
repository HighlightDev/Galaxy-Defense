#pragma once

#include <cstddef>
#include <stdint.h>
#include <gl/glew.h>
#include <type_traits>

namespace Graphics
{
   /* Depth / stencil state */
   template <
       bool depthTestEnabled,
       int32_t depthFunc,
       bool stencilTestEnabled,
       int32_t sfail,
       int32_t dpfail,
       int32_t dppass,
       int32_t func,
       int32_t funcRef,
       int32_t funcMask,
       int32_t stencilMask>
   struct DepthStencilState;

   template <int32_t depthFunc, int32_t sfail, int32_t dpfail, int32_t dppass, int32_t func, int32_t funcRef, int32_t funcMask, int32_t stencilMask>
   struct DepthStencilState<false, depthFunc, false, sfail, dpfail, dppass, func, funcRef, funcMask, stencilMask>
   {
      static void BindDepthStencilState()
      {
         glDisable(GL_DEPTH_TEST);

         glDisable(GL_STENCIL_TEST);
      }
   };

   template <int32_t depthFunc, int32_t sfail, int32_t dpfail, int32_t dppass, int32_t func, int32_t funcRef, int32_t funcMask, int32_t stencilMask>
   struct DepthStencilState<true, depthFunc, false, sfail, dpfail, dppass, func, funcRef, funcMask, stencilMask>
   {
      static void BindDepthStencilState()
      {
         glEnable(GL_DEPTH_TEST);
         glDepthFunc(depthFunc);

         glDisable(GL_STENCIL_TEST);
      }
   };

   template <int32_t depthFunc, int32_t sfail, int32_t dpfail, int32_t dppass, int32_t func, int32_t funcRef, int32_t funcMask, int32_t stencilMask>
   struct DepthStencilState<false, depthFunc, true, sfail, dpfail, dppass, func, funcRef, funcMask, stencilMask>
   {
      static void BindDepthStencilState()
      {
         glDisable(GL_DEPTH_TEST);

         glEnable(GL_STENCIL_TEST);
         glStencilOp(sfail, dpfail, dppass);
         glStencilFunc(func, funcRef, funcMask);
         glStencilMask(stencilMask);
      }
   };

   template <int32_t depthFunc, int32_t sfail, int32_t dpfail, int32_t dppass, int32_t func, int32_t funcRef, int32_t funcMask, int32_t stencilMask>
   struct DepthStencilState<true, depthFunc, true, sfail, dpfail, dppass, func, funcRef, funcMask, stencilMask>
   {
      static void BindDepthStencilState()
      {
         glEnable(GL_DEPTH_TEST);
         glDepthFunc(depthFunc);

         glEnable(GL_STENCIL_TEST);
         glStencilOp(sfail, dpfail, dppass);
         glStencilFunc(func, funcRef, funcMask);
         glStencilMask(stencilMask);
      }
   };

   /* Blending state */
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

   template <>
   struct BlendingState<false>
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
