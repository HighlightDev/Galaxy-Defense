#include "PrimitiveSorter.h"

#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

#include <algorithm>

using namespace Graphics::Proxy;

namespace Graphics {
void PrimitiveSorter::SortPrimitivesByOrder(std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitiveProxies)
{
    std::sort(primitiveProxies.begin(), primitiveProxies.end(), [](const auto& forwardProxyLeft, const auto& forwardProxyRight) {
        return forwardProxyLeft->GetPrimitiveSortOrder() < forwardProxyRight->GetPrimitiveSortOrder();
    });
}
} // namespace Graphics