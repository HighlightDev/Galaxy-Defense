#pragma once

#include "Core/CommonCore/Assertion.h"

#include <gl/glew.h>
#include <stdint.h>

#include <cstddef>
#include <type_traits>

namespace Graphics {
/* Depth / stencil state */
struct DepthState {
    friend class RenderState;

private:
    GLboolean _dtEnabled;
    GLboolean _dtwMask;
    GLenum _dtFunc;

    bool dtEnableDirty{true};
    bool dtwMaskDirty{true};
    bool dtFuncDirty{true};

    explicit DepthState();

    void BindDepthState();

public:
    DepthState& SetIsDepthTestEnabled(const GLboolean depthTestEnabled);

    DepthState& SetDepthTestWriteMask(const GLboolean depthTestWriteMask);

    DepthState& SetDepthTestFunc(const GLenum depthTestFunc);
};

struct StencilState {
    friend class RenderState;

private:
    GLboolean _stEnabled;
    GLenum _sfail;
    GLenum _dpfail;
    GLenum _dppass;
    GLenum _func;
    GLint _funcRef;
    GLuint _funcMask;
    GLuint _stencilMask;

    bool stEnableDirty{true};
    bool stOperationDirty{true};
    bool stFuncDirty{true};
    bool stMaskDirty{true};

    explicit StencilState();

    void BindStencilState();

public:
    StencilState& SetIsStencilTestEnabled(const GLboolean stencilTestEnabled);

    StencilState& SetStencilOperation(const GLenum sfail, const GLenum dpfail, const GLenum dppass);

    StencilState& SetStencilFunction(const GLenum func, const GLint funcRef, const GLuint funcMask);

    StencilState& SetStencilMask(const GLuint stencilMask);
};

/* Blending state */
struct BlendingState {
    friend class RenderState;

private:
    GLboolean _blendingEnabled;
    GLenum _sfactor;
    GLenum _dfactor;

    bool blendingEnableDirty{true};
    bool blendingFuncDirty{true};

public:
    explicit BlendingState();

    void BindBlendState();

    BlendingState& SetIsBlendingEnabled(const GLboolean blendingEnabled);

    BlendingState& SetBlendingFunction(const GLenum sfactor, const GLenum dfactor);
};

class RenderState {
public:
    static DepthState& GetDepthState()
    {
        static DepthState instance;
        return instance;
    }

    static StencilState& GetStencilState()
    {
        static StencilState instance;
        return instance;
    }

    static BlendingState& GetBlendingState()
    {
        static BlendingState instance;
        return instance;
    }

    void BindRenderState()
    {
        GetDepthState().BindDepthState();
        GetStencilState().BindStencilState();
        GetBlendingState().BindBlendState();
    }
};
} // namespace Graphics
