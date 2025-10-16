#pragma once

#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "PrimitiveComponent.h"

using namespace Graphics;
using namespace Graphics::Data;

namespace EngineCore {
namespace Scripts {
class LuaWrapper;
}

struct MeshComponentData;
class SkeletalMeshComponent : public PrimitiveComponent {
    using Base = PrimitiveComponent;

    bool bIsRenderDataDirty{false};

protected:
    MeshRenderData m_renderData;

    std::string mLuaScriptAbsPath;

    std::unique_ptr<::EngineCore::Scripts::LuaWrapper> mLuaInstance;

    float mUpdateDataResetTimeCounter;

    float update_data_reset_time;

    float mTimeIncreaseMultiply;

public:
    std::string LuaScriptName;

    /* src is the main animation time counter*/
    std::shared_ptr<EngineObjectProperty<float>> SrcAnimationTime;

    /* dst time is used when blending of animations is being calculated*/
    std::shared_ptr<EngineObjectProperty<float>> DstAnimationTime;

    /* src is the main animation name*/
    std::shared_ptr<EngineObjectProperty<std::string>> SrcAnimationName;

    /* dst animation name is used when blending of animations is being occurred*/
    std::shared_ptr<EngineObjectProperty<std::string>> DstAnimationName;

    std::shared_ptr<EngineObjectProperty<bool>> bTransitionEnabled;

    std::shared_ptr<EngineObjectProperty<float>> TransitionValue;

public:
    SkeletalMeshComponent(const std::shared_ptr<MeshComponentData>& meshComponentData, const MeshRenderData& renderData);

    ~SkeletalMeshComponent() override;

    void OnSceneOwnerInitialized() override;

    void SetIsEnabled(const bool bEnabled) override;

    void SetIsVisible(bool isVisible) override;

    eComponentType GetComponentType() const override;

    void Tick(const float deltaTime) override;

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    inline const MeshRenderData& GetRenderData() const
    {
        return m_renderData;
    }

    std::shared_ptr<IMaterial> GetMaterial() const;

    void SetMeshModelPath(const std::string& modelPath);

protected:
    void SyncDataWithRenderThread();
};

} // namespace EngineCore
