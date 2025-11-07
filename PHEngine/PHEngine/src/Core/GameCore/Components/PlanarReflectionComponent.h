#pragma once
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceController.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"
#include "SceneComponent.h"

using namespace Resources;
using namespace Graphics::Texture;

namespace Graphics {
class PlanarReflectionProxy;
}

namespace EngineCore {
class ACamera;
struct PlanarReflectionComponentData;

using Graphics::PlanarReflectionProxy;

class PlanarReflectionComponent : public SceneComponent, public IDeferredResourceCreator {
    int32_t mPlanarReflectionSceneProxyId{-1};

    glm::vec4 mReflectionPlane;

    std::weak_ptr<ACamera> mOwnerCameraWp;

    ::Graphics::ViewPortInfo mRenderTargetViewPortInfo;

    std::shared_ptr<DeferredResourceController<std::shared_ptr<ITexture>, eDeferredResourceType::TEXTURE>>
        mPlanarReflectionDeferredController;

    std::atomic<bool> bIsSceneProxyReady{false};

    bool bIsRenderDataDirty{false};

public:
    PlanarReflectionComponent(const std::shared_ptr<PlanarReflectionComponentData>& data);

    ~PlanarReflectionComponent() override;

    void SetIsSceneProxyReady(const bool isReady);

    bool IsSceneProxyReady() const;

    std::shared_ptr<PlanarReflectionProxy> CreatePlanarReflectionProxy();

    void Tick(const float deltaTimeSec) override;

    void OnPostInitialized() override;

    void PostLevelInit() override;

    ::Graphics::ViewPortInfo GetRenderTargetViewPortInfo() const;

    std::weak_ptr<ACamera> GetOwnerCameraWp() const;

    int32_t GetSceneProxyId() const;

    glm::vec4 GetReflectionPlane() const;

    void SetSceneProxyId(const int32_t sceneProxyId);

    eComponentType GetComponentType() const override;

    void SyncDataWithRenderThread();

    std::shared_ptr<IDeferredResource> GetDeferredResource() override;

    std::shared_ptr<DeferredResourceController<std::shared_ptr<ITexture>, eDeferredResourceType::TEXTURE>>
    GetPlanarReflectionDeferredController() const;

private:
    void UpdateReflectionPlane();
};
} // namespace EngineCore
