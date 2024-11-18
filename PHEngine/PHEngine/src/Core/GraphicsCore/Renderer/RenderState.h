#pragma once

#include <cstddef>
#include <stdint.h>
#include <gl/glew.h>
#include <type_traits>

#include "Core/CommonCore/Assertion.h"

namespace Graphics
{
   /* Depth / stencil state */
   struct DepthState
   {
      template <typename StencilStateType, typename BlendingStateType>
      friend class RenderState;

   private:
      GLboolean _dtEnabled;
      GLboolean _dtwMask;
      GLenum _dtFunc;

      explicit DepthState();

      void BindDepthState();

   public:
      DepthState &SetIsDepthTestEnabled(const GLboolean depthTestEnabled);

      DepthState &SetDepthTestWriteMask(const GLboolean depthTestWriteMask);

      DepthState &SetDepthTestFunc(const GLenum depthTestFunc);
   };

   template <bool stencilTestEnabled,
             int32_t sfail,
             int32_t dpfail,
             int32_t dppass,
             int32_t func,
             int32_t funcRef,
             int32_t funcMask,
             int32_t stencilMask>
   struct StencilState;

   template <int32_t sfail,
             int32_t dpfail,
             int32_t dppass,
             int32_t func,
             int32_t funcRef,
             int32_t funcMask,
             int32_t stencilMask>
   struct StencilState<false, sfail, dpfail, dppass, func, funcRef, funcMask, stencilMask>
   {
      static void BindStencilState()
      {
         glDisable(GL_STENCIL_TEST);
         glStencilOp(sfail, dpfail, dppass);
         glStencilFunc(func, funcRef, funcMask);
         glStencilMask(stencilMask);
      }
   };

   template <int32_t sfail,
             int32_t dpfail,
             int32_t dppass,
             int32_t func,
             int32_t funcRef,
             int32_t funcMask,
             int32_t stencilMask>
   struct StencilState<true, sfail, dpfail, dppass, func, funcRef, funcMask, stencilMask>
   {
      static void BindStencilState()
      {
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
   template <typename StencilStateType, typename BlendingStateType>
   class RenderState
   {
      using blendState_t = BlendingStateType;
      using stencilState_t = StencilStateType;

   public:
      static DepthState &GetDepthState()
      {
         static DepthState instance;
         return instance;
      }

      void BindRenderState()
      {
         GetDepthState().BindDepthState();
         stencilState_t::BindStencilState();
         blendState_t::BindBlendState();
      }
   };
}
