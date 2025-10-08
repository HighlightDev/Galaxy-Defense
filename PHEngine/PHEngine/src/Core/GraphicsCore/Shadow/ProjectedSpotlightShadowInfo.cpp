#include "ProjectedSpotlightShadowInfo.h"

namespace Graphics {
ProjectedSpotlightShadowInfo::ProjectedSpotlightShadowInfo(const TextureAtlasSpaceRequest& shadowAtlasCellResource)
    : ProjectedShadowInfo(shadowAtlasCellResource)
{
    m_lightType = LightType::SPOT_LIGHT;
}

ProjectedSpotlightShadowInfo::~ProjectedSpotlightShadowInfo()
{
    Event::TextureAtlasGeneratedGameThreadEvent::GetInstance()->RemoveListener(GetInstanceId());
}

void ProjectedSpotlightShadowInfo::Initialize()
{
    Event::TextureAtlasGeneratedGameThreadEvent::GetInstance()->AddListener(
        std::dynamic_pointer_cast<ProjectedSpotlightShadowInfo>(shared_from_this()));
}

std::shared_ptr<Texture2dAtlasHandler> ProjectedSpotlightShadowInfo::GetTexture2dHandler() const
{
    return std::static_pointer_cast<Texture2dAtlasHandler>(mShadowmapHandler);
}

void ProjectedSpotlightShadowInfo::ProcessEvent(
    const TextureAtlasGeneratedGameThreadEvent* sender,
    const typename Event::TextureAtlasGeneratedGameThreadEvent::EventData_t& data)
{
    if (eTextureType::TEXTURE_2D == std::get<0>(data)) {
        mShadowmapHandler
            = TextureAtlasFactory::GetInstance()->GetTextureAtlasCellByRequestId(mShadowmapAtlasRequest.MyRequestId);
        if (mOnShadowMapUpdatedCallback) {
            mOnShadowMapUpdatedCallback();
        }
    }
}

void ProjectedSpotlightShadowInfo::BindShadowFramebuffer(bool bBindFramebuffer, bool clearDepthBuffer) const
{
    ProjectedShadowInfo::BindShadowFramebuffer(bBindFramebuffer, clearDepthBuffer);

    auto texAtlas = GetTexture2dHandler();
    auto atlas_cell = texAtlas->GetAtlasCell();
    const GLbitfield clearDepthFlag = clearDepthBuffer ? GL_DEPTH_BUFFER_BIT : 0;
    m_shadowFramebuffer->RenderToTexture(
        bBindFramebuffer, atlas_cell.X, atlas_cell.Y, atlas_cell.Width, atlas_cell.Height, clearDepthFlag);
}

glm::mat4x4 ProjectedSpotlightShadowInfo::GetShadowViewMatrix() const
{
    return mShadowViewMatrix;
}

glm::mat4x4 ProjectedSpotlightShadowInfo::GetShadowProjectionMatrix() const
{
    return mShadowProjectionMatrix;
}

void ProjectedSpotlightShadowInfo::SetShadowViewMatrix(const glm::mat4x4& shadowViewMatrix)
{
    mShadowViewMatrix = shadowViewMatrix;
}

void ProjectedSpotlightShadowInfo::SetShadowProjectionMatrix(const glm::mat4x4& shadowProjectionMatrix)
{
    mShadowProjectionMatrix = shadowProjectionMatrix;
}

glm::mat4x4 ProjectedSpotlightShadowInfo::GetShadowMatrix() const
{
    glm::mat4 shadowMatrix(1);
    shadowMatrix *= m_shadowBiasMatrix;
    shadowMatrix *= mShadowProjectionMatrix;
    shadowMatrix *= mShadowViewMatrix;
    return shadowMatrix;
}

glm::vec4 ProjectedSpotlightShadowInfo::GetTextureAtlasOffset() const
{
    glm::vec4 result;

    if (mShadowmapHandler) {
        result = GetTexture2dHandler()->GetTextureAtlasOffset();
    }

    return result;
}
} // namespace Graphics
