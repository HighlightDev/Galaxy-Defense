#include "PrimitiveSorter.h"

#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

using namespace Graphics::Proxy;

namespace Graphics {
void PrimitiveSorter::SortPrimitivesByOrder(std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies)
{
    std::sort(primitiveProxies.begin(), primitiveProxies.end(), [](const auto& forwardProxyLeft, const auto& forwardProxyRight) {
        return forwardProxyLeft->GetPrimitiveSortOrder() < forwardProxyRight->GetPrimitiveSortOrder();
    });
}

template<typename T>
typename std::enable_if<std::is_base_of<PrimitiveSceneProxy, T>::value, std::vector<std::shared_ptr<T>>>::type
PrimitiveSorter::SortPrimitivesByShaderAndMaterial(const std::vector<std::shared_ptr<T>>& primitiveProxies)
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

template std::vector<std::shared_ptr<PrimitiveSceneProxy>>
PrimitiveSorter::SortPrimitivesByShaderAndMaterial<PrimitiveSceneProxy>(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>&);
template std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>
PrimitiveSorter::SortPrimitivesByShaderAndMaterial<SkeletalMeshSceneProxy>(
    const std::vector<std::shared_ptr<SkeletalMeshSceneProxy>>&);
} // namespace Graphics