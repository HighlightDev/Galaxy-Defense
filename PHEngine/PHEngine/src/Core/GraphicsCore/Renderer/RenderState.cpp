#include "RenderState.h"

#include <functional>

namespace Graphics {
/********************************************
 *               DepthState
 ********************************************/

const std::unordered_map<bool, std::function<void(GLenum)>> glFuncMap
    = {{true, [](GLenum cap) { glEnable(cap); }}, {false, [](GLenum cap) { glDisable(cap); }}};

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
    if (dtEnableDirty) {
        glFuncMap.at(_dtEnabled)(GL_DEPTH_TEST);
        dtEnableDirty = false;
    }
    if (dtwMaskDirty) {
        glDepthMask(_dtwMask);
        dtwMaskDirty = false;
    }
    if (dtFuncDirty) {
        glDepthFunc(_dtFunc);
        dtFuncDirty = false;
    }
}

DepthState& DepthState::SetIsDepthTestEnabled(const GLboolean depthTestEnabled)
{
    if (_dtEnabled != depthTestEnabled) {
        dtEnableDirty = true;
        _dtEnabled = depthTestEnabled;
    }
    return *this;
}

DepthState& DepthState::SetDepthTestWriteMask(const GLboolean depthTestWriteMask)
{
    if (_dtwMask != depthTestWriteMask) {
        dtwMaskDirty = true;
        _dtwMask = depthTestWriteMask;
    }
    return *this;
}

DepthState& DepthState::SetDepthTestFunc(const GLenum depthTestFunc)
{
    ext_assert(depthTestFunc >= GL_NEVER && depthTestFunc <= GL_ALWAYS, "Invalid depth test function value");
    if (_dtFunc != depthTestFunc) {
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
    if (stEnableDirty) {
        glFuncMap.at(_stEnabled)(GL_STENCIL_TEST);
        stEnableDirty = false;
    }
    if (stOperationDirty) {
        glStencilOp(_sfail, _dpfail, _dppass);
        stOperationDirty = false;
    }
    if (stFuncDirty) {
        glStencilFunc(_func, _funcRef, _funcMask);
        stFuncDirty = false;
    }
    if (stMaskDirty) {
        glStencilMask(_stencilMask);
        stMaskDirty = false;
    }
}

StencilState& StencilState::SetIsStencilTestEnabled(const GLboolean stencilTestEnabled)
{
    if (_stEnabled != stencilTestEnabled) {
        _stEnabled = stencilTestEnabled;
        stEnableDirty = true;
    }
    return *this;
}

StencilState& StencilState::SetStencilOperation(const GLenum sfail, const GLenum dpfail, const GLenum dppass)
{
    if (_sfail != sfail || _dpfail != dpfail || _dppass != dppass) {
        _sfail = sfail;
        _dpfail = dpfail;
        _dppass = dppass;
        stOperationDirty = true;
    }
    return *this;
}

StencilState& StencilState::SetStencilFunction(const GLenum func, const GLint funcRef, const GLuint funcMask)
{
    if (_func != func || _funcRef != funcRef || _funcMask != funcMask) {
        _func = func;
        _funcRef = funcRef;
        _funcMask = funcMask;
        stFuncDirty = true;
    }
    return *this;
}

StencilState& StencilState::SetStencilMask(const GLuint stencilMask)
{
    if (_stencilMask != stencilMask) {
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
    _rgbSrcFactor = sfactor;
    _rgbDstFactor = dfactor;
    _alphaSrcFactor = sfactor;
    _alphaDstFactor = dfactor;
}

void BlendingState::BindBlendState()
{
    if (blendingEnableDirty) {
        glFuncMap.at(_blendingEnabled)(GL_BLEND);
        blendingEnableDirty = false;
    }
    if (blendingFuncDirty) {
        glBlendFuncSeparate(_rgbSrcFactor, _rgbDstFactor, _alphaSrcFactor, _alphaDstFactor);
        blendingFuncDirty = false;
    }
}

BlendingState& BlendingState::SetIsBlendingEnabled(const GLboolean blendingEnabled)
{
    if (_blendingEnabled != blendingEnabled) {
        _blendingEnabled = blendingEnabled;
        blendingEnableDirty = true;
    }
    return *this;
}

BlendingState& BlendingState::SetBlendingFunction(const GLenum rgbSrcFactor, const GLenum rgbDstFactor)
{
    if (_rgbSrcFactor != rgbSrcFactor || _rgbDstFactor != rgbDstFactor || _alphaSrcFactor != rgbSrcFactor
        || _alphaDstFactor != rgbDstFactor) {
        _rgbSrcFactor = rgbSrcFactor;
        _rgbDstFactor = rgbDstFactor;
        _alphaSrcFactor = rgbSrcFactor;
        _alphaDstFactor = rgbDstFactor;
        blendingFuncDirty = true;
    }
    return *this;
}

BlendingState& BlendingState::SetBlendingFunction(
    const GLenum rgbSrcFactor, const GLenum rgbDstFactor, const GLenum alphaSrcFactor, const GLenum alphaDstFactor)
{
    if (_rgbSrcFactor != rgbSrcFactor || _rgbDstFactor != rgbDstFactor || _alphaSrcFactor != alphaSrcFactor
        || _alphaDstFactor != alphaDstFactor) {
        _rgbSrcFactor = rgbSrcFactor;
        _rgbDstFactor = rgbDstFactor;
        _alphaSrcFactor = alphaSrcFactor;
        _alphaDstFactor = alphaDstFactor;
        blendingFuncDirty = true;
    }
    return *this;
}

CullingState::CullingState()
{
    GLint cullFaceMode, frontFace;
    glGetBooleanv(GL_CULL_FACE, &_cullingEnabled);
    glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);
    glGetIntegerv(GL_FRONT_FACE, &frontFace);
    _cullFaceMode = static_cast<GLenum>(cullFaceMode);
    _frontFace = static_cast<GLenum>(frontFace);
}

void CullingState::BindCullingState()
{
    if (cullingEnableDirty) {
        glFuncMap.at(_cullingEnabled)(GL_CULL_FACE);
        // cullingEnableDirty = false;
    }
    if (cullFaceModeDirty) {
        glCullFace(_cullFaceMode);
        // cullFaceModeDirty = false;
    }
    if (frontFaceDirty) {
        glFrontFace(_frontFace);
        // frontFaceDirty = false;
    }
}

CullingState& CullingState::SetIsCullingEnabled(const GLboolean cullingEnabled)
{
    if (_cullingEnabled != cullingEnabled) {
        _cullingEnabled = cullingEnabled;
        cullingEnableDirty = true;
    }
    return *this;
}

CullingState& CullingState::SetCullFaceMode(const GLenum cullFaceMode)
{
    if (_cullFaceMode != cullFaceMode) {
        _cullFaceMode = cullFaceMode;
        cullFaceModeDirty = true;
    }
    return *this;
}

CullingState& CullingState::SetFrontFace(const GLenum frontFace)
{
    if (_frontFace != frontFace) {
        _frontFace = frontFace;
        frontFaceDirty = true;
    }
    return *this;
}

ClipPlaneState::ClipPlaneState()
{
    for (uint32_t i = 0; i < 6; ++i) {
        glGetBooleanv(GL_CLIP_DISTANCE0 + i, &_clipPlaneEnabled[i]);
    }
}

void ClipPlaneState::BindClipPlaneState()
{
    for (uint32_t i = 0; i < 6; ++i) {
        if (clipPlaneEnabledDirty[i]) {
            std::invoke(glFuncMap.at(_clipPlaneEnabled[i]), GL_CLIP_DISTANCE0 + i);
            clipPlaneEnabledDirty[i] = false;
        }
    }
}

void ClipPlaneState::SetIsClipPlaneEnabled(const uint32_t clipPlaneIndex, const GLboolean enabled)
{
    ext_assert(clipPlaneIndex < 6, "Invalid clip plane index");
    if (_clipPlaneEnabled[clipPlaneIndex] != enabled) {
        _clipPlaneEnabled[clipPlaneIndex] = enabled;
        clipPlaneEnabledDirty[clipPlaneIndex] = true;
    }
}
} // namespace Graphics
