#pragma once
#include "Core/GameCore/ShaderImplementation/BillboardShader.h"
#include "Core/GraphicsCore/RenderData/BillboardRenderData.h"
#include "PrimitiveComponent.h"

#include <glm/mat4x4.hpp>

#include <functional>
#include <memory>

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Data;

namespace EngineCore {
struct BillboardComponentData;

class BillboardComponent : public PrimitiveComponent {
protected:
    using Base = PrimitiveComponent;

    float mBillboardExtent;

    bool mApplyScreenAspectRatio;

    float mRotationRadians;

    bool mIsFlipped{false};

private:
    BillboardRenderData m_renderData;

    bool bIsRenderDataDirty{false};

    std::function<glm::mat4(const glm::mat4&)> mViewMatrixTransformer;

    std::function<glm::mat4(const glm::mat4&)> mProjectionMatrixTransformer;

public:
    BillboardComponent(const std::shared_ptr<BillboardComponentData>& data, const BillboardRenderData& renderData);

    ~BillboardComponent() override;

    eComponentType GetComponentType() const override;

    void UnpausableTick(const float deltaTimeSec) override;

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    std::shared_ptr<Scripts::LuaProxy> ReplicateLuaProxy() override;

    void SetBillboardExtent(const float extent);

    float GetBillboardExtent() const;

    void SetIsEnabled(const bool bEnabled) override;

    void SetIsVisible(bool isVisible) override;

    std::shared_ptr<IMaterial> GetMaterial() const;

    inline const BillboardRenderData& GetRenderData() const
    {
        return m_renderData;
    }

    std::function<glm::mat4(const glm::mat4&)> GetViewMatrixTransformer() const;

    std::function<glm::mat4(const glm::mat4&)> GetProjectionMatrixTransformer() const;

    void SetApplyScreenAspectRatio(const bool apply);

    bool GetApplyScreenAspectRatio() const;

    void SetRotationRadians(const float rotationRadians);

    float GetRotationRadians() const;

    void SetIsFlipped(const bool isFlipped);

    bool GetIsFlipped() const;

private:
    void SyncRenderData();
};

} // namespace EngineCore
