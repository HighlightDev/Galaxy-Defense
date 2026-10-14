#pragma once

#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace Graphics {

class CameraSceneProxy;

using namespace Proxy;
class PrimitiveSorter {
public:
    enum class ePrimitiveSortComparatorType { LESS, GREATER };

    PrimitiveSorter() = default;

    ~PrimitiveSorter() = default;

    void SortPrimitivesByOrder(std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies);

    std::vector<std::shared_ptr<PrimitiveSceneProxy>>
    SortPrimitivesByOrderAndShader(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies);

    template<typename T>
    typename std::enable_if<std::is_base_of<PrimitiveSceneProxy, T>::value, std::vector<std::shared_ptr<T>>>::type
    SortPrimitivesByShader(const std::vector<std::shared_ptr<T>>& primitiveProxies);

    template<typename T>
    typename std::enable_if<std::is_base_of<PrimitiveSceneProxy, T>::value, std::vector<std::shared_ptr<T>>>::type
    SortPrimitivesByShaderAndDistanceToCamera(
        const glm::vec3& cameraPosition, const std::vector<std::shared_ptr<T>>& primitiveProxies);

    template<typename T>
    typename std::enable_if<std::is_base_of<PrimitiveSceneProxy, T>::value, std::vector<std::shared_ptr<T>>>::type
    SortPrimitivesByDistanceToCamera(
        const PrimitiveSorter::ePrimitiveSortComparatorType sortCmpType,
        const glm::vec3& cameraPosition,
        const std::vector<std::shared_ptr<T>>& primitiveProxies);
};
} // namespace Graphics