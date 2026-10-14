#include "DynamicBeamComponent.h"

#include "Core/GameCore/Components/ProceduralBeamGeometry.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/ElectricBeamSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

using namespace Graphics::Renderer;

namespace EngineCore {

void DynamicBeamComponent::OnRegistered()
{
    Base::OnRegistered(); // bakes the (canonical) frame set
    mIsTransformDirty = true;
}

void DynamicBeamComponent::BakeBeamMesh(
    const float animationTime, std::vector<BeamVertex>& outVertices, std::vector<uint32_t>& outIndices) const
{
    const float radius = mBeamThickness * 0.1f; // Convert thickness to radius

    // Canonical unit beam — independent of the endpoints, which the proxy applies via the world matrix.
    ProceduralBeamGeometry::GenerateCanonicalAnimatedElectricBeamGeometry(
        radius, mRadialSegments, mLengthSegments, mJitterAmount, animationTime, mAnimationSpeed, outVertices, outIndices);
}

void DynamicBeamComponent::OnEndpointsChanged()
{
    // Canonical geometry doesn't depend on the endpoints — just resend the placement matrix, no re-bake.
    mIsTransformDirty = true;
}

void DynamicBeamComponent::SyncTransformIfDirty()
{
    if (mIsTransformDirty) {
        SyncTransform();
        mIsTransformDirty = false;
    }
}

void DynamicBeamComponent::SyncTransform()
{
    static constexpr uint64_t functionId = Hash64_CT("DynamicBeamComponent::SyncTransform");
    const glm::mat4 worldMatrix = ComputeBeamWorldMatrix();
    if (const auto& sceneSp = m_sceneWP.lock()) {
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetObjectId(),
            functionId,
            [sceneProxyId = mSceneProxyId, worldMatrix](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                    if (const auto& beamProxySp = std::static_pointer_cast<Graphics::Proxy::ElectricBeamSceneProxy>(
                            sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        beamProxySp->SetBeamWorldMatrix(worldMatrix);
                    }
                }
            });
    }
}

glm::mat4 DynamicBeamComponent::ComputeBeamWorldMatrix() const
{
    const glm::vec3 delta = mEndWorldPosition - mStartWorldPosition;
    const float length = glm::length(delta);
    if (EngineMath::FloatsNearEqual(length, 0.0f)) {
        return glm::translate(glm::mat4(1.0f), mStartWorldPosition);
    }

    const glm::vec3 direction = delta / length;

    glm::vec3 tangent, bitangent;
    EngineMath::CreateOrthogonalBasisFromDirection(direction, tangent, bitangent);

    glm::mat4 worldMatrix(1.0f);
    worldMatrix[0] = glm::vec4(tangent, 0.0f); // local +X (radius plane) → world tangent
    worldMatrix[1] = glm::vec4(bitangent, 0.0f); // local +Y (radius plane) → world bitangent
    worldMatrix[2] = glm::vec4(direction * length, 0.0f); // local +Z (0..1) → beam axis scaled by length
    worldMatrix[3] = glm::vec4(mStartWorldPosition, 1.0f);
    return worldMatrix;
}

} // namespace EngineCore
