#include "RenderPassProxies.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "PrimitiveSorter.h"

#include <algorithm>
#include <iterator>

using namespace Graphics;
using namespace Graphics::Proxy;

namespace Graphics::Renderer {

/*  ShadowDepthPassProxiesProvider */
ShadowDepthPassProxiesProvider::ShadowDepthPassProxiesProvider()
    : IRenderPassProxiesProvider()
{
}

eRenderPassType ShadowDepthPassProxiesProvider::GetRenderPassType() const
{
    return eRenderPassType::SHADOW_DEPTH_PASS;
}

void ShadowDepthPassProxiesProvider::Reset(
    const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allSceneProxies,
    const std::vector<std::shared_ptr<LightSceneProxy>>& allLightProxies)
{
    mSortedPrimitivesByShadows.clear();

    std::vector<std::shared_ptr<PrimitiveSceneProxy>> deferredNonSkeletalMeshProxies;
    deferredNonSkeletalMeshProxies.reserve(allSceneProxies.size());
    std::vector<std::shared_ptr<SkeletalMeshSceneProxy>> deferredSkeletalMeshProxies;
    deferredSkeletalMeshProxies.reserve(allSceneProxies.size());

    for (const auto& proxy : allSceneProxies) {
        if (proxy->IsDeferred() && ePrimitiveProxyType::INDIRECT_RENDERED_PROXY != proxy->GetPrimitiveProxyType()) {
            if (ePrimitiveProxyType::SKELETAL_MESH_PROXY == proxy->GetPrimitiveProxyType()) {
                deferredSkeletalMeshProxies.emplace_back(std::static_pointer_cast<SkeletalMeshSceneProxy>(proxy));
            } else {
                deferredNonSkeletalMeshProxies.emplace_back(proxy);
            }
        }
    }

    PrimitiveSorter sorter;
    for (const auto& lightProxy : allLightProxies) {
        const auto shadowInfo = lightProxy->GetShadowInfo();
        if (shadowInfo != nullptr) {
            const uint32_t shadowTextureDescId = shadowInfo->GetAtlasResource()->GetTextureDescriptor();
            // A single shadow atlas can be shared by several lights; sort the candidates once per descriptor.
            if (mSortedPrimitivesByShadows.count(shadowTextureDescId) > 0) {
                continue;
            }
            mSortedPrimitivesByShadows.emplace(
                shadowTextureDescId,
                std::make_pair(
                    sorter.SortPrimitivesByDistanceToCamera(
                        PrimitiveSorter::ePrimitiveSortComparatorType::LESS,
                        lightProxy->GetShadowCastPosition(),
                        deferredNonSkeletalMeshProxies),
                    sorter.SortPrimitivesByDistanceToCamera(
                        PrimitiveSorter::ePrimitiveSortComparatorType::LESS,
                        lightProxy->GetShadowCastPosition(),
                        deferredSkeletalMeshProxies)));
        }
    }
}

const std::pair<std::vector<std::shared_ptr<PrimitiveSceneProxy>>, std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>>&
ShadowDepthPassProxiesProvider::GetPrimitivesForShadowByDescriptorId(const uint32_t shadowTextureDescId) const
{
    if (mSortedPrimitivesByShadows.count(shadowTextureDescId) > 0) {
        return mSortedPrimitivesByShadows.at(shadowTextureDescId);
    }
    ext_assert(false, "Missing cache for shadow texture descriptor id: " + std::to_string(shadowTextureDescId));
    static std::pair<std::vector<std::shared_ptr<PrimitiveSceneProxy>>, std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>>
        defaultResult = {};
    return defaultResult;
}

void ShadowDepthPassProxiesProvider::CleanUp()
{
     mSortedPrimitivesByShadows.clear();
}

/*  PlanarReflectionPassProxiesProvider */
PlanarReflectionPassProxiesProvider::PlanarReflectionPassProxiesProvider()
    : IRenderPassProxiesProvider()
{
}

eRenderPassType PlanarReflectionPassProxiesProvider::GetRenderPassType() const
{
    return eRenderPassType::PLANAR_REFLECTION_PASS;
}

void PlanarReflectionPassProxiesProvider::Reset(
    const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allPrimitiveProxies,
    const std::vector<std::shared_ptr<PlanarReflectionProxy>>& allPlanarReflectionProxies)
{
    mSortedPrimitivesByPlane.clear();
    PrimitiveSorter sorter;
    for (const auto& planarReflectionProxy : allPlanarReflectionProxies) {
        auto& proxies = mSortedPrimitivesByPlane[planarReflectionProxy->GetSceneProxyId()];
        proxies.reserve(allPrimitiveProxies.size());
        std::copy_if(
            allPrimitiveProxies.cbegin(), allPrimitiveProxies.cend(), std::back_inserter(proxies), [](const auto& proxy) {
                return proxy->GetPrimitiveProxyType() != ePrimitiveProxyType::INDIRECT_RENDERED_PROXY;
            });
        // Note: the in-place sort overload cannot deduce its template argument, so the call would silently resolve to the
        // copy-returning overload. Assign the sorted copy back explicitly.
        proxies = sorter.SortPrimitivesByDistanceToCamera(
            PrimitiveSorter::ePrimitiveSortComparatorType::LESS, planarReflectionProxy->GetReflectionPlaneOrigin(), proxies);
    }
}

const std::vector<std::shared_ptr<PrimitiveSceneProxy>>&
PlanarReflectionPassProxiesProvider::GetPrimitivesForPlane(const int32_t planarReflectionProxyId) const
{
    if (mSortedPrimitivesByPlane.count(planarReflectionProxyId) > 0) {
        return mSortedPrimitivesByPlane.at(planarReflectionProxyId);
    }
    static std::vector<std::shared_ptr<PrimitiveSceneProxy>> defaultResult = {};
    return defaultResult;
}

void PlanarReflectionPassProxiesProvider::CleanUp()
{
     mSortedPrimitivesByPlane.clear();
}

/*  OutlinePassProxiesProvider */
OutlinePassProxiesProvider::OutlinePassProxiesProvider()
    : IRenderPassProxiesProvider()
{
}

eRenderPassType OutlinePassProxiesProvider::GetRenderPassType() const
{
    return eRenderPassType::OUTLINE_PASS;
}

void OutlinePassProxiesProvider::Reset(
    const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allProxies, const glm::vec3& cameraPosition)
{
    mSortedPrimitives.clear();
    mSortedPrimitives.reserve(allProxies.size());

    std::copy_if(allProxies.cbegin(), allProxies.cend(), std::back_inserter(mSortedPrimitives), [](const auto& proxy) {
        return proxy->IsDeferred() && ePrimitiveProxyType::INDIRECT_RENDERED_PROXY != proxy->GetPrimitiveProxyType();
    });

    PrimitiveSorter sorter;
    mSortedPrimitives = sorter.SortPrimitivesByDistanceToCamera(
        PrimitiveSorter::ePrimitiveSortComparatorType::LESS, cameraPosition, mSortedPrimitives);
}

const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& OutlinePassProxiesProvider::GetPrimitives() const
{
    return mSortedPrimitives;
}

void OutlinePassProxiesProvider::CleanUp()
{
     mSortedPrimitives.clear();
}

/*  DepthPrePassProxiesProvider */
DepthPrePassProxiesProvider::DepthPrePassProxiesProvider()
    : IRenderPassProxiesProvider()
{
}

eRenderPassType DepthPrePassProxiesProvider::GetRenderPassType() const
{
    return eRenderPassType::DEPTH_PRE_PASS;
}

void DepthPrePassProxiesProvider::Reset(
    const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allProxies, const glm::vec3& cameraPosition)
{
    mSortedPrimitives.first.clear();
    mSortedPrimitives.second.clear();

    mSortedPrimitives.first.reserve(allProxies.size());
    mSortedPrimitives.second.reserve(allProxies.size());

    for (const auto& proxy : allProxies) {
        if (proxy->IsDeferred() && ePrimitiveProxyType::INDIRECT_RENDERED_PROXY != proxy->GetPrimitiveProxyType()) {
            if (ePrimitiveProxyType::SKELETAL_MESH_PROXY == proxy->GetPrimitiveProxyType()) {
                mSortedPrimitives.second.emplace_back(std::static_pointer_cast<SkeletalMeshSceneProxy>(proxy));
            } else {
                mSortedPrimitives.first.emplace_back(proxy);
            }
        }
    }

    PrimitiveSorter sorter;
    mSortedPrimitives.first = sorter.SortPrimitivesByDistanceToCamera(
        PrimitiveSorter::ePrimitiveSortComparatorType::LESS, cameraPosition, mSortedPrimitives.first);
    mSortedPrimitives.second = sorter.SortPrimitivesByDistanceToCamera(
        PrimitiveSorter::ePrimitiveSortComparatorType::LESS, cameraPosition, mSortedPrimitives.second);
}

const std::pair<std::vector<std::shared_ptr<PrimitiveSceneProxy>>, std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>>&
DepthPrePassProxiesProvider::GetPrimitives() const
{
    return mSortedPrimitives;
}

void DepthPrePassProxiesProvider::CleanUp()
{
     mSortedPrimitives.first.clear();
     mSortedPrimitives.second.clear();
}

/*  DeferredBasePassProxiesProvider */
DeferredBasePassProxiesProvider::DeferredBasePassProxiesProvider()
{
}

eRenderPassType DeferredBasePassProxiesProvider::GetRenderPassType() const
{
    return eRenderPassType::DEFERRED_BASE_PASS;
}

void DeferredBasePassProxiesProvider::Reset(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allProxies)
{
    mSortedPrimitives.clear();
    mSortedPrimitives.reserve(allProxies.size());
    std::copy_if(allProxies.cbegin(), allProxies.cend(), std::back_inserter(mSortedPrimitives), [](const auto& proxy) {
        return proxy->IsDeferred() && ePrimitiveProxyType::INDIRECT_RENDERED_PROXY != proxy->GetPrimitiveProxyType();
    });
}

const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& DeferredBasePassProxiesProvider::GetPrimitives() const
{
    return mSortedPrimitives;
}

void DeferredBasePassProxiesProvider::CleanUp()
{
     mSortedPrimitives.clear();
}

/*  ForwardPassProxiesProvider */
ForwardPassProxiesProvider::ForwardPassProxiesProvider()
    : IRenderPassProxiesProvider()
{
}

eRenderPassType ForwardPassProxiesProvider::GetRenderPassType() const
{
    return eRenderPassType::FORWARD_BASE_PASS;
}

void ForwardPassProxiesProvider::Reset(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& allProxies)
{
    mSortedPrimitives.clear();
    mSortedPrimitives.reserve(allProxies.size());
    std::copy_if(allProxies.cbegin(), allProxies.cend(), std::back_inserter(mSortedPrimitives), [](const auto& proxy) {
        return !proxy->IsDeferred() && ePrimitiveProxyType::INDIRECT_RENDERED_PROXY != proxy->GetPrimitiveProxyType();
    });

    PrimitiveSorter sorter;
    mSortedPrimitives = sorter.SortPrimitivesByOrderAndShader(mSortedPrimitives);
}

const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& ForwardPassProxiesProvider::GetPrimitives() const
{
    return mSortedPrimitives;
}

void ForwardPassProxiesProvider::CleanUp()
{
     mSortedPrimitives.clear();
}
} // namespace Graphics::Renderer
