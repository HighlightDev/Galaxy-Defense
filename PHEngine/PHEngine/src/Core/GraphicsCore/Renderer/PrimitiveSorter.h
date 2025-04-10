#pragma once

#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace Graphics {

using namespace Proxy;
class PrimitiveSorter {
public:
    PrimitiveSorter() = default;

    ~PrimitiveSorter() = default;

    void SortPrimitivesByOrder(std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies);

    template<typename T>
    typename std::enable_if<std::is_base_of<PrimitiveSceneProxy, T>::value, std::vector<std::shared_ptr<T>>>::type
    SortPrimitivesByShaderAndMaterial(const std::vector<std::shared_ptr<T>>& primitiveProxies);
};
} // namespace Graphics