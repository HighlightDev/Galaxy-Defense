#include "RenderState.h"

namespace Graphics
{
    /********************************************
     *               DepthState
     ********************************************/

    DepthState::DepthState()
    {
        glGetBooleanv(GL_DEPTH_TEST, &_dtEnabled);
        glGetBooleanv(GL_DEPTH_WRITEMASK, &_dtwMask);
        GLint dtFunc;
        glGetIntegerv(GL_DEPTH_FUNC, &dtFunc);
        _dtFunc = dtFunc;
    }

    void DepthState::BindDepthState()
    {
        if (dtEnableDirty)
        {
            if (_dtEnabled)
            {
                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }
            // dtEnableDirty = false;
        }
        if (dtwMaskDirty)
        {
            glDepthMask(_dtwMask);
            // dtwMaskDirty = false;
        }
        if (dtFuncDirty)
        {
            glDepthFunc(_dtFunc);
            // dtFuncDirty = false;
        }
    }

    DepthState &DepthState::SetIsDepthTestEnabled(const GLboolean depthTestEnabled)
    {
        if (_dtEnabled != depthTestEnabled)
        {
            dtEnableDirty = true;
            _dtEnabled = depthTestEnabled;
        }
        return *this;
    }

    DepthState &DepthState::SetDepthTestWriteMask(const GLboolean depthTestWriteMask)
    {
        if (_dtwMask != depthTestWriteMask)
        {
            dtwMaskDirty = true;
            _dtwMask = depthTestWriteMask;
        }
        return *this;
    }

    DepthState &DepthState::SetDepthTestFunc(const GLenum depthTestFunc)
    {
        assert(depthTestFunc >= GL_NEVER && depthTestFunc <= GL_ALWAYS);
        if (_dtFunc != depthTestFunc)
        {
            dtFuncDirty = true;
            _dtFunc = depthTestFunc;
        }
        return *this;
    }

    /********************************************
     *               StencilState
     ********************************************/

    StencilState::StencilState()
    {
        glGetBooleanv(GL_STENCIL_TEST, &_stEnabled);

        GLint sfail, dpFail, dpPass;
        glGetIntegerv(GL_STENCIL_FAIL, &sfail);
        glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &dpFail);
        glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &dpPass);
        _sfail = sfail;
        _dpfail = dpFail;
        _dppass = dpPass;

        GLint sfunc, funcMask, stencilMask;
        glGetIntegerv(GL_STENCIL_FUNC, &sfunc);
        glGetIntegerv(GL_STENCIL_REF, &_funcRef);
        glGetIntegerv(GL_STENCIL_VALUE_MASK, &funcMask);
        _func = sfunc;
        _funcMask = funcMask;

        glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilMask);
        _stencilMask = stencilMask;
    }

    void StencilState::BindStencilState()
    {
        if (stEnableDirty)
        {
            if (_stEnabled)
            {
                glEnable(GL_STENCIL_TEST);
            }
            else
            {
                glDisable(GL_STENCIL_TEST);
            }
            // stEnableDirty = false;
        }
        if (stOperationDirty)
        {
            glStencilOp(_sfail, _dpfail, _dppass);
            // stOperationDirty = false;
        }
        if (stOperationDirty)
        {
            glStencilFunc(_func, _funcRef, _funcMask);
            // stOperationDirty = false;
        }
        if (stMaskDirty)
        {
            glStencilMask(_stencilMask);
            // stMaskDirty = false;
        }
    }

    StencilState &StencilState::SetIsStencilTestEnabled(const GLboolean stencilTestEnabled)
    {
        if (_stEnabled != stencilTestEnabled)
        {
            _stEnabled = stencilTestEnabled;
            stEnableDirty = true;
        }
        return *this;
    }

    StencilState &StencilState::SetStencilOperation(const GLenum sfail,
                                                    const GLenum dpfail,
                                                    const GLenum dppass)
    {
        if (_sfail != sfail ||
            _dpfail != dpfail ||
            _dppass != dppass)
        {
            _sfail = sfail;
            _dpfail = dpfail;
            _dppass = dppass;
            stOperationDirty = true;
        }
        return *this;
    }

    StencilState &StencilState::SetStencilFunction(const GLenum func,
                                                   const GLint funcRef,
                                                   const GLuint funcMask)
    {
        if (_func != func ||
            _funcRef != funcRef ||
            _funcMask != funcMask)
        {
            _func = func;
            _funcRef = funcRef;
            _funcMask = funcMask;
            stOperationDirty = true;
        }
        return *this;
    }

    StencilState &StencilState::SetStencilMask(const GLuint stencilMask)
    {
        if (_stencilMask != stencilMask)
        {
            _stencilMask = stencilMask;
            stMaskDirty = true;
        }
        return *this;
    }

    /********************************************
     *               BlendingState
     ********************************************/

    BlendingState::BlendingState()
    {
        glGetBooleanv(GL_BLEND, &_blendingEnabled);
        GLint sfactor, dfactor;
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &sfactor);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &dfactor);
        _sfactor = sfactor;
        _dfactor = dfactor;
    }

    void BlendingState::BindBlendState()
    {
        if (blendingEnableDirty)
        {
            if (_blendingEnabled)
            {
                glEnable(GL_BLEND);
            }
            else
            {
                glDisable(GL_BLEND);
            }
            // blendingEnableDirty = false;
        }
        if (blendingFuncDirty)
        {
            glBlendFunc(_sfactor, _dfactor);
            // blendingFuncDirty = false;
        }
    }

    BlendingState &BlendingState::SetIsBlendingEnabled(const GLboolean blendingEnabled)
    {
        if (_blendingEnabled != blendingEnabled)
        {
            _blendingEnabled = blendingEnabled;
            blendingEnableDirty = true;
        }
        return *this;
    }

    BlendingState &BlendingState::SetBlendingFunction(const GLenum sfactor,
                                                      const GLenum dfactor)
    {
        if (_sfactor != sfactor ||
            _dfactor != dfactor)
        {
            _sfactor = sfactor;
            _dfactor = dfactor;
            blendingFuncDirty = true;
        }
        return *this;
    }
}
