#include "RenderState.h"

namespace Graphics
{
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
        if (_dtEnabled)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
        glDepthFunc(_dtFunc);
        glDepthMask(_dtwMask);
    }

    DepthState &DepthState::SetIsDepthTestEnabled(const GLboolean depthTestEnabled)
    {
        if (_dtEnabled != depthTestEnabled)
        {
            _dtEnabled = depthTestEnabled;
        }
        return *this;
    }

    DepthState &DepthState::SetDepthTestWriteMask(const GLboolean depthTestWriteMask)
    {
        if (_dtwMask != depthTestWriteMask)
        {
            _dtwMask = depthTestWriteMask;
        }
        return *this;
    }

    DepthState &DepthState::SetDepthTestFunc(const GLenum depthTestFunc)
    {
        assert(depthTestFunc >= GL_NEVER && depthTestFunc <= GL_ALWAYS);
        if (_dtFunc != depthTestFunc)
        {
            _dtFunc = depthTestFunc;
        }
        return *this;
    }
}
