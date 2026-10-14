#pragma once

#include "Core/CommonCore/Assertion.h"

#include <gl/glew.h>
#include <stdint.h>

#include <array>
#include <cstddef>
#include <type_traits>

namespace Graphics {
/* Depth / stencil state */
class DepthState {
    friend class RenderState;

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

    GLboolean GetIsDepthTestEnabled() const
    {
        return _dtEnabled;
    }

    GLboolean GetDepthTestWriteMask() const
    {
        return _dtwMask;
    }

    GLenum GetDepthTestFunc() const
    {
        return _dtFunc;
    }
};

class StencilState {
    friend class RenderState;

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

    GLboolean GetIsStencilTestEnabled() const
    {
        return _stEnabled;
    }

    GLenum GetStencilSFail() const
    {
        return _sfail;
    }

    GLenum GetStencilDPFail() const
    {
        return _dpfail;
    }

    GLenum GetStencilDPPass() const
    {
        return _dppass;
    }

    GLenum GetStencilFunc() const
    {
        return _func;
    }

    GLint GetStencilFuncRef() const
    {
        return _funcRef;
    }

    GLuint GetStencilFuncMask() const
    {
        return _funcMask;
    }

    GLuint GetStencilMask() const
    {
        return _stencilMask;
    }
};

/* Blending state */
class BlendingState {
    friend class RenderState;

    GLboolean _blendingEnabled;
    GLenum _rgbSrcFactor;
    GLenum _rgbDstFactor;
    GLenum _alphaSrcFactor;
    GLenum _alphaDstFactor;

    bool blendingEnableDirty{true};
    bool blendingFuncDirty{true};

public:
    explicit BlendingState();

    void BindBlendState();

    BlendingState& SetIsBlendingEnabled(const GLboolean blendingEnabled);

    BlendingState& SetBlendingFunction(const GLenum rgbSrcFactor, const GLenum rgbDstFactor);

    BlendingState& SetBlendingFunction(
        const GLenum rgbSrcFactor, const GLenum rgbDstFactor, const GLenum alphaSrcFactor, const GLenum alphaDstFactor);

    GLboolean GetIsBlendingEnabled() const
    {
        return _blendingEnabled;
    }

    GLenum GetRgbSrcFactor() const
    {
        return _rgbSrcFactor;
    }

    GLenum GetRgbDstFactor() const
    {
        return _rgbDstFactor;
    }

    GLenum GetAlphaSrcFactor() const
    {
        return _alphaSrcFactor;
    }

    GLenum GetAlphaDstFactor() const
    {
        return _alphaDstFactor;
    }
};

class CullingState {
    friend class RenderState;

    GLboolean _cullingEnabled;
    GLenum _cullFaceMode;
    GLenum _frontFace;

    bool cullingEnableDirty{true};
    bool cullFaceModeDirty{true};
    bool frontFaceDirty{true};

public:
    explicit CullingState();

    void BindCullingState();

    CullingState& SetIsCullingEnabled(const GLboolean cullingEnabled);

    CullingState& SetCullFaceMode(const GLenum cullFaceMode);

    CullingState& SetFrontFace(const GLenum frontFace);

    GLboolean GetIsCullingEnabled() const
    {
        return _cullingEnabled;
    }

    GLenum GetCullFaceMode() const
    {
        return _cullFaceMode;
    }

    GLenum GetFrontFace() const
    {
        return _frontFace;
    }
};

class ClipPlaneState {
    friend class RenderState;

    GLboolean _clipPlaneEnabled[6]{};

    bool clipPlaneEnabledDirty[6]{true, true, true, true, true, true};

public:
    explicit ClipPlaneState();

    void BindClipPlaneState();

    void SetIsClipPlaneEnabled(const uint32_t clipPlaneIndex, const GLboolean enabled);

    GLboolean GetIsClipPlaneEnabled(const uint32_t clipPlaneIndex) const
    {
        ext_assert(clipPlaneIndex < 6, "Invalid clip plane index");
        return _clipPlaneEnabled[clipPlaneIndex];
    }
};

class ColorState {
    friend class RenderState;

    std::array<GLboolean, 4> mChannelsMask;

    bool mChannelsMaskDirty{true};

public:
    explicit ColorState();

    void BindColorState();

    void SetColorMask(const GLboolean red, const GLboolean green, const GLboolean blue, const GLboolean alpha);

    const std::array<GLboolean, 4>& GetChannelsMask() const;
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

    static CullingState& GetCullingState()
    {
        static CullingState instance;
        return instance;
    }

    static ClipPlaneState& GetClipPlaneState()
    {
        static ClipPlaneState instance;
        return instance;
    }

    static ColorState& GetColorState()
    {
        static ColorState instance;
        return instance;
    }

    void BindRenderState()
    {
        GetDepthState().BindDepthState();
        GetStencilState().BindStencilState();
        GetBlendingState().BindBlendState();
        GetCullingState().BindCullingState();
        GetClipPlaneState().BindClipPlaneState();
        GetColorState().BindColorState();
    }
};
} // namespace Graphics
