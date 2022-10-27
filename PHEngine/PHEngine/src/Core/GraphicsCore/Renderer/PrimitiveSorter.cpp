#include "PrimitiveSorter.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

#include <algorithm>

using namespace Graphics::Proxy;

namespace Graphics
{
    void PrimitiveSorter::SortPrimitivesByOrder(std::vector<std::shared_ptr<PrimitiveSceneProxy>> &primitiveProxies)
    {
        std::sort(primitiveProxies.begin(), primitiveProxies.end(), [](const auto &forwardProxyLeft, const auto &forwardProxyRight)
                  { return static_cast<uint8_t>(forwardProxyLeft->GetPrimitiveSortOrder()) < static_cast<uint8_t>(forwardProxyRight->GetPrimitiveSortOrder()); });
    }
}