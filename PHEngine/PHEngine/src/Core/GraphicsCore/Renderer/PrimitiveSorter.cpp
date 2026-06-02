#include "PrimitiveSorter.h"

#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"

#include <glm/gtx/norm.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

using namespace Graphics::Proxy;

namespace Graphics {
void PrimitiveSorter::SortPrimitivesByOrder(std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies)
{
    std::sort(primitiveProxies.begin(), primitiveProxies.end(), [](const auto& proxyLeft, const auto& proxyRight) {
        return proxyLeft->GetPrimitiveSortOrder() < proxyRight->GetPrimitiveSortOrder();
    });
}

std::vector<std::shared_ptr<PrimitiveSceneProxy>>
PrimitiveSorter::SortPrimitivesByOrderAndShader(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies)
{
    std::map<int32_t, std::vector<std::shared_ptr<PrimitiveSceneProxy>>> orderGroups;
    std::set<int32_t> existingOrders;
    std::for_each(primitiveProxies.cbegin(), primitiveProxies.cend(), [&existingOrders](const auto& primitiveProxy) {
        if (!existingOrders.count(primitiveProxy->GetPrimitiveSortOrder())) {
            existingOrders.emplace(primitiveProxy->GetPrimitiveSortOrder());
        }
    });
    for (const auto& order : existingOrders) {
        for (const auto& primitiveProxy : primitiveProxies) {
            if (primitiveProxy->GetPrimitiveSortOrder() == order) {
                orderGroups[order].push_back(primitiveProxy);
            }
        }
    }
    std::vector<std::shared_ptr<PrimitiveSceneProxy>> finalSortedProxies;
    for (const auto& [order, proxies] : orderGroups) {
        const auto& sortedProxiesGroup = SortPrimitivesByShader(proxies);
        finalSortedProxies.insert(finalSortedProxies.end(), sortedProxiesGroup.begin(), sortedProxiesGroup.end());
    }

    return finalSortedProxies;
}

template<typename T>
typename std::enable_if<std::is_base_of<PrimitiveSceneProxy, T>::value, std::vector<std::shared_ptr<T>>>::type
PrimitiveSorter::SortPrimitivesByShader(const std::vector<std::shared_ptr<T>>& primitiveProxies)
{
    std::vector<std::shared_ptr<T>> sortedVector;
    sortedVector.reserve(primitiveProxies.size());
    std::unordered_map<std::string, std::vector<int32_t>> shaderNamesToPrimitiveIndicesMap;
    for (int i = 0; i < primitiveProxies.size(); ++i) {
        const auto& primitiveSp = primitiveProxies[i];
        const auto renderInfo = primitiveSp->GetRenderInfo();
        shaderNamesToPrimitiveIndicesMap[renderInfo.mShaderName].push_back(i);
    }

    for (const auto& [shaderName, primitiveIndices] : shaderNamesToPrimitiveIndicesMap) {
        for (const auto primitiveIndex : primitiveIndices) {
            sortedVector.emplace_back(primitiveProxies.at(primitiveIndex));
        }
    }

    return sortedVector;
}

template<typename T>
typename std::enable_if<std::is_base_of<PrimitiveSceneProxy, T>::value, std::vector<std::shared_ptr<T>>>::type
PrimitiveSorter::SortPrimitivesByShaderAndDistanceToCamera(
    const std::shared_ptr<CameraSceneProxy>& cameraProxy, const std::vector<std::shared_ptr<T>>& primitiveProxies)
{
    std::vector<std::shared_ptr<T>> sortedVector;
    sortedVector.reserve(primitiveProxies.size());
    std::unordered_map<std::string, std::vector<int32_t>> shaderNamesToPrimitiveIndicesMap;
    for (int i = 0; i < primitiveProxies.size(); ++i) {
        const auto& primitiveSp = primitiveProxies[i];
        const auto renderInfo = primitiveSp->GetRenderInfo();
        shaderNamesToPrimitiveIndicesMap[renderInfo.mShaderName].push_back(i);
    }

    for (const auto& [shaderName, primitiveIndices] : shaderNamesToPrimitiveIndicesMap) {
        if (primitiveIndices.size() > 1) {
            std::vector<std::shared_ptr<T>> primitivesGroup;
            primitivesGroup.reserve(primitiveIndices.size());
            for (const auto primitiveIndex : primitiveIndices) {
                primitivesGroup.emplace_back(primitiveProxies.at(primitiveIndex));
            }
            std::sort(
                primitivesGroup.begin(), primitivesGroup.end(), [&cameraProxy](const auto& proxyLeft, const auto& proxyRight) {
                    const float lengthToLeft2 = glm::length2(proxyLeft->GetOriginPosition() - cameraProxy->GetEyeVector());
                    const float lengthToRight2 = glm::length2(proxyRight->GetOriginPosition() - cameraProxy->GetEyeVector());
                    return lengthToLeft2 < lengthToRight2; // sort front to back to apply early depth test
                });
            sortedVector.insert(sortedVector.end(), primitivesGroup.begin(), primitivesGroup.end());
        } else {
            sortedVector.emplace_back(primitiveProxies.at(primitiveIndices.back()));
        }
    }

    return sortedVector;
}

template std::vector<std::shared_ptr<PrimitiveSceneProxy>>
PrimitiveSorter::SortPrimitivesByShader<PrimitiveSceneProxy>(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>&);
template std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>
PrimitiveSorter::SortPrimitivesByShader<SkeletalMeshSceneProxy>(const std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>&);

template std::vector<std::shared_ptr<PrimitiveSceneProxy>>
PrimitiveSorter::SortPrimitivesByShaderAndDistanceToCamera<PrimitiveSceneProxy>(
    const std::shared_ptr<CameraSceneProxy>& cameraProxy, const std::vector<std::shared_ptr<PrimitiveSceneProxy>>&);
template std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>
PrimitiveSorter::SortPrimitivesByShaderAndDistanceToCamera<SkeletalMeshSceneProxy>(
    const std::shared_ptr<CameraSceneProxy>& cameraProxy, const std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>&);
} // namespace Graphics