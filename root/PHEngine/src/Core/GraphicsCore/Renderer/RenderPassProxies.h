#pragma once

#include "RenderPassType.h"

#include <glm/vec3.hpp>
#include <stdint.h>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Graphics::Proxy {
class PrimitiveSceneProxy;
class SkeletalMeshSceneProxy;
class LightSceneProxy;
} // namespace Graphics::Proxy

namespace Graphics {
class PlanarReflectionProxy;
} // namespace Graphics

namespace Graphics::Renderer {

using namespace Proxy;

class IRenderPassProxiesProvider {
public:
    virtual ~IRenderPassProxiesProvider() = default;

    virtual eRenderPassType GetRenderPassType() const = 0;

    virtual void CleanUp() = 0;
};

class ShadowDepthPassProxiesProvider : public IRenderPassProxiesProvider {
    std::unordered_map<
        uint32_t,
        std::pair<std::vector<std::shared_ptr<PrimitiveSceneProxy>>, std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>>>
        mSortedPrimitivesByShadows;

public:
    explicit ShadowDepthPassProxiesProvider();

    eRenderPassType GetRenderPassType() const override;

    void Reset(
        const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allPrimitiveProxies,
        const std::vector<std::shared_ptr<LightSceneProxy>>& allLightProxies);

    const std::pair<std::vector<std::shared_ptr<PrimitiveSceneProxy>>, std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>>&
    GetPrimitivesForShadowByDescriptorId(const uint32_t shadowTextureDescId) const;

    void CleanUp() override;
};

class PlanarReflectionPassProxiesProvider : public IRenderPassProxiesProvider {
    std::unordered_map<int32_t, std::vector<std::shared_ptr<PrimitiveSceneProxy>>> mSortedPrimitivesByPlane;

public:
    explicit PlanarReflectionPassProxiesProvider();

    eRenderPassType GetRenderPassType() const override;

    void Reset(
        const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allProxies,
        const std::vector<std::shared_ptr<PlanarReflectionProxy>>& allPlanarReflectionProxies);

    const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& GetPrimitivesForPlane(const int32_t planarReflectionProxyId) const;

    void CleanUp() override;
};

class OutlinePassProxiesProvider : public IRenderPassProxiesProvider {
    std::vector<std::shared_ptr<PrimitiveSceneProxy>> mSortedPrimitives;

public:
    explicit OutlinePassProxiesProvider();

    eRenderPassType GetRenderPassType() const override;

    void Reset(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allProxies, const glm::vec3& cameraPosition);

    const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& GetPrimitives() const;

    void CleanUp() override;
};

class DepthPrePassProxiesProvider : public IRenderPassProxiesProvider {
    std::pair<std::vector<std::shared_ptr<PrimitiveSceneProxy>>, std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>>
        mSortedPrimitives;

public:
    explicit DepthPrePassProxiesProvider();

    eRenderPassType GetRenderPassType() const override;

    void Reset(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allProxies, const glm::vec3& cameraPosition);

    const std::pair<std::vector<std::shared_ptr<PrimitiveSceneProxy>>, std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>>&
    GetPrimitives() const;

    void CleanUp() override;
};

class DeferredBasePassProxiesProvider : public IRenderPassProxiesProvider {
    std::vector<std::shared_ptr<PrimitiveSceneProxy>> mSortedPrimitives;

public:
    explicit DeferredBasePassProxiesProvider();

    eRenderPassType GetRenderPassType() const override;

    void Reset(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allProxies);

    const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& GetPrimitives() const;

    void CleanUp() override;
};

class ForwardPassProxiesProvider : public IRenderPassProxiesProvider {
    std::vector<std::shared_ptr<PrimitiveSceneProxy>> mSortedPrimitives;

public:
    explicit ForwardPassProxiesProvider();

    eRenderPassType GetRenderPassType() const override;

    void Reset(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allProxies);

    const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& GetPrimitives() const;

    void CleanUp() override;
};
} // namespace Graphics::Renderer
