#pragma once
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "PrimitiveComponent.h"

#include <glm/mat4x4.hpp>

using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore {
namespace Scripts {
class LuaProxy;
} // namespace Scripts
struct MeshComponentData;

class StaticMeshComponent : public PrimitiveComponent {
protected:
    using Base = PrimitiveComponent;

    MeshRenderData m_renderData;

public:
    StaticMeshComponent(const std::shared_ptr<MeshComponentData>& meshComponentData, const MeshRenderData renderData);

    ~StaticMeshComponent() override;

    eComponentType GetComponentType() const override;

    void SetIsEnabled(const bool bEnabled) override;

    void SetIsVisible(bool isVisible) override;

    void Tick(const float deltaTimeSec) override;

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    std::shared_ptr<Scripts::LuaProxy> ReplicateLuaProxy() override;

    inline const MeshRenderData& GetRenderData() const
    {
        return m_renderData;
    }

    std::shared_ptr<IMaterial> GetMaterial() const;

    void SetMeshModelPath(const std::string& modelPath);
};

} // namespace EngineCore
