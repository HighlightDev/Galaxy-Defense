#include "ProjectedPointLightShadowInfo.h"

namespace Graphics {
ProjectedPointLightShadowInfo::ProjectedPointLightShadowInfo(const TextureAtlasSpaceRequest& shadowAtlasCellResource)
    : ProjectedShadowInfo(shadowAtlasCellResource)
{
    m_lightType = LightType::POINT_LIGHT;
}

ProjectedPointLightShadowInfo::~ProjectedPointLightShadowInfo()
{
    Event::TextureAtlasGeneratedGameThreadEvent::GetInstance()->RemoveListener(
        Event::TextureAtlasGeneratedGameThreadEvent::GetInstanceId());
}

void ProjectedPointLightShadowInfo::Initialize()
{
    Event::TextureAtlasGeneratedGameThreadEvent::GetInstance()->AddListener(
        std::dynamic_pointer_cast<ProjectedPointLightShadowInfo>(shared_from_this()));
}

std::shared_ptr<TextureCubeAtlasHandler> ProjectedPointLightShadowInfo::GetTextureCubeHandler() const
{
    return std::static_pointer_cast<TextureCubeAtlasHandler>(mShadowmapHandler);
}

void ProjectedPointLightShadowInfo::ProcessEvent(
    const TextureAtlasGeneratedGameThreadEvent* sender,
    const typename Event::TextureAtlasGeneratedGameThreadEvent::EventData_t& data)
{
    if (eTextureType::TEXTURE_CUBE == std::get<0>(data)) {
        mShadowmapHandler
            = TextureAtlasFactory::GetInstance()->GetTextureAtlasCellByRequestId(mShadowmapAtlasRequest.MyRequestId);
    }
}

void ProjectedPointLightShadowInfo::BindShadowFramebuffer(bool bBindFramebuffer, bool clearDepthBuffer) const
{
    ProjectedShadowInfo::BindShadowFramebuffer(bBindFramebuffer, clearDepthBuffer);

    auto rezolution = mShadowmapHandler->GetAtlasResource()->GetTextureRezolution();
    const GLbitfield clearDepthFlag = GL_DEPTH_BUFFER_BIT;
    m_shadowFramebuffer->RenderToTexture(bBindFramebuffer, 0, 0, rezolution.x, rezolution.y, clearDepthFlag);
}

ProjectedPointLightShadowInfo::six_mat4x4 ProjectedPointLightShadowInfo::GetShadowViewMatrices() const
{
    return m_shadowViewMatrix;
}

ProjectedPointLightShadowInfo::six_mat4x4 ProjectedPointLightShadowInfo::GetShadowProjectionMatrices() const
{
    return m_shadowProjectionMatrix;
}

void ProjectedPointLightShadowInfo::SetShadowViewMatrices(const six_mat4x4& shadowViewMatrices)
{
    m_shadowViewMatrix = shadowViewMatrices;
}

void ProjectedPointLightShadowInfo::SetShadowProjectionMatrix(const six_mat4x4& shadowProjectionMatrices)
{
    m_shadowProjectionMatrix = shadowProjectionMatrices;
}

ProjectedPointLightShadowInfo::six_mat4x4 ProjectedPointLightShadowInfo::GetShadowMatrix() const
{
    ProjectedPointLightShadowInfo::six_mat4x4 result;

    // 0
    glm::mat4 shadowMatrix(1);
    shadowMatrix *= m_shadowBiasMatrix;
    shadowMatrix *= m_shadowProjectionMatrix[0];
    shadowMatrix *= m_shadowViewMatrix[0];
    result[0] = shadowMatrix;
    // 1
    shadowMatrix = glm::mat4(1);
    shadowMatrix *= m_shadowBiasMatrix;
    shadowMatrix *= m_shadowProjectionMatrix[1];
    shadowMatrix *= m_shadowViewMatrix[1];
    result[1] = shadowMatrix;
    // 2
    shadowMatrix = glm::mat4(1);
    shadowMatrix *= m_shadowBiasMatrix;
    shadowMatrix *= m_shadowProjectionMatrix[2];
    shadowMatrix *= m_shadowViewMatrix[2];
    result[2] = shadowMatrix;
    // 3
    shadowMatrix = glm::mat4(1);
    shadowMatrix *= m_shadowBiasMatrix;
    shadowMatrix *= m_shadowProjectionMatrix[3];
    shadowMatrix *= m_shadowViewMatrix[3];
    std::get<3>(result) = shadowMatrix;
    // 4
    shadowMatrix = glm::mat4(1);
    shadowMatrix *= m_shadowBiasMatrix;
    shadowMatrix *= m_shadowProjectionMatrix[4];
    shadowMatrix *= m_shadowViewMatrix[4];
    std::get<4>(result) = shadowMatrix;
    // 5
    shadowMatrix = glm::mat4(1);
    shadowMatrix *= m_shadowBiasMatrix;
    shadowMatrix *= m_shadowProjectionMatrix[5];
    shadowMatrix *= m_shadowViewMatrix[5];
    std::get<5>(result) = shadowMatrix;

    return result;
}
} // namespace Graphics
