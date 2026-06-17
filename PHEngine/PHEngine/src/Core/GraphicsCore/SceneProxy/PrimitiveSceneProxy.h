#pragma once

#include "Core/GameCore/EngineObject.h"
#include "Core/GraphicsCore/Material/MaterialProxy.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/RenderInfo.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GraphicsCore/Renderer/ActiveBindedState.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SceneProxyBase.h"
#include "Core/GraphicsCore/SceneViewInfo/AProxyVisibilityController.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/CompositeShaderPool.h"

#include <memory>
#include <type_traits>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace Graphics::Texture;
using namespace EngineCore;
using namespace Resources;
using namespace Graphics;
using namespace IO;

namespace Graphics {
namespace Renderer {
class SceneRenderer;
}
} // namespace Graphics

namespace EngineCore {
class PrimitiveComponent;
}

namespace Graphics {
namespace Proxy {
enum class ePrimitiveProxyType : int8_t { PRIMITIVE_PROXY, STATIC_MESH_PROXY, SKELETAL_MESH_PROXY, INDIRECT_RENDERED_PROXY };

enum class eMeshFacing {
    CLOCK_WISE,
    COUNTER_CLOCK_WISE,
};

class PrimitiveSceneProxy : public SceneProxyBase, public AProxyVisibilityController {

    bool bTransformInitialized;

    std::weak_ptr<::Graphics::Renderer::SceneRenderer> mDeferredShadingSceneRenderer;

    int32_t mSortOrderValue{0};

protected:
    glm::mat4 m_worldMatrix;

    glm::mat4 m_outlineMatrix;

    std::shared_ptr<Skin> m_skin;

    std::shared_ptr<IShader> m_shader;

    std::shared_ptr<IShader> m_planarReflectionShader;

    std::shared_ptr<IShader> m_outlineShader;

    std::shared_ptr<MaterialProxy> mMaterialProxy;

    bool mCanBloomBeApplied{false};

    bool mDepthWriteMaskEnabled{true};

    bool mIsOutlineApplied{false};

    float mOutlineThickness{0.0f};

    glm::vec3 mOriginPosition;

public:
    PrimitiveSceneProxy(const ::EngineCore::PrimitiveComponent* component, const std::shared_ptr<MaterialProxy>& materialProxy);

    void CleanUp() override;

    void SetWorldMatrix(const glm::mat4& worldMatrix);

    void SetDeferredShadingSceneRenderer(const std::weak_ptr<::Graphics::Renderer::SceneRenderer>& deferredShadingSceneRenderer);

    const std::weak_ptr<::Graphics::Renderer::SceneRenderer>& GetDeferredShadingSceneRendererWp() const;

    virtual glm::mat4 GetMatrix() const;

    virtual void PostConstructorInitialize();

    virtual std::shared_ptr<Skin> GetSkin() const;

    virtual ePrimitiveProxyType GetPrimitiveProxyType() const;

    bool IsFrustumCullTestNeeded() const override;

    virtual void Render(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState) = 0;

    virtual void RenderPlanarReflection(
        const glm::vec4& plane,
        const glm::mat4& mirrorMatrix,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState)
    {
    }

    virtual void RenderOutline(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState)
    {
    }

    virtual void RenderOutlineStencil(
        const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        ActiveBindedState& activeBindedState)
    {
    }

    virtual bool IsDeferred() const = 0;

    virtual eMeshFacing GetMeshFrontFace() const;

    virtual bool IsTransformIntialized() const;

    // [primitive with sort order value < 0 are the most early drawn primitives]
    int32_t GetPrimitiveSortOrder() const;

    void SetSortOrderValue(const int32_t sortOrderValue);

    int32_t GetSortOrderValue() const;

    void SetCanBloomBeApplied(const bool value);

    bool CanBloomBeApplied() const;

    bool IsDepthWriteMaskEnabled() const;

    void SetDepthWriteMaskEnabled(const bool isEnabled);

    bool GetIsOutlineApplied() const;

    void SetIsOutlineApplied(const bool value);

    virtual RenderInfo GetRenderInfo() const = 0;

    void SetOriginPosition(const glm::vec3& origin);

    glm::vec3 GetOriginPosition() const;

    void SetOutlineMatrix(const glm::mat4& outlineMatrix);

public:
    template<typename VertexFactoryType, typename BaseShaderType>
    static std::enable_if_t<
        std::is_base_of_v<VertexFactoryShader, VertexFactoryType> && std::is_base_of_v<Shader, BaseShaderType>,
        typename CompositeShaderPool::sharedValue_t>
    CreateMaterialShader(
        const std::string& compositeShaderName, const ShaderParams& shaderParams, std::shared_ptr<MaterialProxy> materialProxy)
    {
        CompositeMaterialShaderParams compositeParams(compositeShaderName, shaderParams, materialProxy);

        return CompositeShaderPool::GetInstance()
            ->template GetOrAllocateResource<VertexFactoryMaterialCompositeShader<VertexFactoryType, BaseShaderType>>(
                compositeParams);
    }
};

} // namespace Proxy
} // namespace Graphics
