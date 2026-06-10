#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/BeamComponentBase.h"

#include <glm/mat4x4.hpp>

namespace EngineCore {

/**
 * Beam whose geometry is baked once in a canonical local space (a unit beam along +Z). The endpoints are
 * applied on the render thread via a world matrix, so moving the beam costs only a cheap matrix update
 * each tick — no geometry re-bake. Meant for beams that move every frame (e.g. electro / freezing rays).
 */
class DynamicBeamComponent : public BeamComponentBase {
    using Base = BeamComponentBase;

    // Endpoints moved → resend the world matrix on the next tick.
    bool mIsTransformDirty{false};

public:
    using BeamComponentBase::BeamComponentBase; // inherit constructors

    void OnRegistered() override;

protected:
    void BakeBeamMesh(
        const float animationTime, std::vector<BeamVertex>& outVertices, std::vector<uint32_t>& outIndices) const override;

    void OnEndpointsChanged() override;
    void SyncTransformIfDirty() override;

private:
    void SyncTransform();
    glm::mat4 ComputeBeamWorldMatrix() const;
};

} // namespace EngineCore
