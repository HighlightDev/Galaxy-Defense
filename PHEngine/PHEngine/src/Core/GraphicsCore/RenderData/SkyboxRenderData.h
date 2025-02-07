#pragma once

#include "Core/GraphicsCore/Material/MaterialProxy.h"

#include <memory>

namespace Graphics {
namespace Data {
struct SkyboxRenderData {
    std::shared_ptr<MaterialProxy> mMaterialProxy;

    SkyboxRenderData(std::shared_ptr<MaterialProxy> materialProxy)
        : mMaterialProxy(materialProxy)
    {
    }
};
} // namespace Data
} // namespace Graphics
