#pragma once

#include <memory>
#include <vector>

namespace Graphics
{
    namespace Proxy
    {
        class PrimitiveSceneProxy;
    }

    class PrimitiveSorter
    {
    public:
        PrimitiveSorter() = default;

        ~PrimitiveSorter() = default;

        void SortPrimitivesByOrder(std::vector<std::shared_ptr<::Graphics::Proxy::PrimitiveSceneProxy>> &primitiveProxies);
    };
}