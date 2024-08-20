#include "InstancedGeometryBatch.h"
#include "Core/GraphicsCore/SceneProxy/InstancedStaticMeshSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderUtilityFunctions.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/MeshPoolParameters.h"
#include "Core/ResourceManagerCore/Pool/InstancedMeshPool.h"

using namespace Graphics;
using namespace Graphics::Proxy;
using namespace Resources;

namespace Graphics::GeometryBatching
{
    InstancedGeometryBatch::InstancedGeometryBatch(const std::shared_ptr<InstancedStaticMeshSceneProxy> &initialSceneProxy)
        : m_renderData(initialSceneProxy->GetRenderData()),
          mBatchKey(initialSceneProxy->GetBatchKey())
    {
        mInstancedStaticMeshSceneProxies.emplace_back(initialSceneProxy);
    }

    void InstancedGeometryBatch::Initialize()
    {
        const auto shaderIdName = m_renderData.mIsDeferredShaded ? "DeferredNonSkeletalBase Shader" : "ForwardNonSkeletalBase Shader";
        const auto fragmentShaderName = m_renderData.mIsDeferredShaded ? "deferredFS.glsl" : "forwardFS.glsl";
        const ShaderParams shaderParams(
            shaderIdName,
            FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "simpleVS.glsl",
            FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + fragmentShaderName);

        mShader = std::static_pointer_cast<typename InstancedGeometryBatch::ShaderType>(PrimitiveSceneProxy::CreateMaterialShader<InstancedStaticMeshVertexFactory,
                                                                                                                                  SimpleShader>("InstancedStaticMeshVertexFactory_SimpleShader_" + m_renderData.mMaterialProxy->MaterialName,
                                                                                                                                                shaderParams, m_renderData.mMaterialProxy));

        MeshPoolParameters poolParameters;
        poolParameters.mModelPath = m_renderData.mModelPath;
        poolParameters.mVertexAttributes = GetShader()->GetVertexAttributes();

        m_skin = InstancedMeshPool::GetInstance()->GetOrAllocateResource(poolParameters);
    }

    std::string InstancedGeometryBatch::GetBatchKey() const
    {
        return mBatchKey;
    }

    void InstancedGeometryBatch::Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy,
                                        const glm::mat4 &viewMatrix,
                                        const glm::mat4 &projectionMatrix)
    {
        auto worldMatrices = CollectAllWorldMatrices();
        const auto &shader = GetShader();

        shader->ExecuteShader();
        shader->GetVertexFactoryShader()->SetMatrices(worldMatrices, viewMatrix, projectionMatrix);
        shader->GetMaterialShader()->LoadUniformValues(m_renderData.mMaterialProxy);
        m_skin->GetBuffer()->RenderInstanced(GL_TRIANGLES, worldMatrices.size());
        shader->StopShader();
    }

    void InstancedGeometryBatch::AddInstancedStaticMeshSceneProxy(const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy> &sceneProxy)
    {
        const auto alreadyContains = std::any_of(mInstancedStaticMeshSceneProxies.cbegin(), mInstancedStaticMeshSceneProxies.cend(), [sceneProxy](const auto &proxyWp)
                                                 {
            if (const auto& proxySp = proxyWp.lock()) {
                return proxySp->GetSceneProxyId() == sceneProxy->GetSceneProxyId();
            }
            return false; });

        if (!alreadyContains)
        {
            mInstancedStaticMeshSceneProxies.emplace_back(sceneProxy);
        }
    }

    void InstancedGeometryBatch::RemoveInstancedStaticMeshSceneProxy(const std::shared_ptr<::Graphics::Proxy::InstancedStaticMeshSceneProxy> &sceneProxy)
    {
        auto removeIt = std::remove_if(mInstancedStaticMeshSceneProxies.begin(), mInstancedStaticMeshSceneProxies.end(), [sceneProxy](const auto &proxyWp)
                                       {
            if (const auto& proxySp = proxyWp.lock()) {
                return proxySp->GetSceneProxyId() == sceneProxy->GetSceneProxyId();
            }
            return false; });
        mInstancedStaticMeshSceneProxies.erase(removeIt);
    }

    std::shared_ptr<InstancedGeometryBatch::ShaderType> InstancedGeometryBatch::GetShader() const
    {
        return mShader;
    }

    std::vector<glm::mat4> InstancedGeometryBatch::CollectAllWorldMatrices()
    {
        std::vector<glm::mat4> result;
        result.reserve(mInstancedStaticMeshSceneProxies.size());

        for (const auto &wpProxy : mInstancedStaticMeshSceneProxies)
        {
            if (const auto &spProxy = wpProxy.lock())
            {
                if (spProxy->IsEnabled() && spProxy->IsVisible() && spProxy->IsTransformIntialized())
                {
                    result.emplace_back(spProxy->GetMatrix());
                }
            }
        }

        return result;
    }
}
