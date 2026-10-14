#pragma once

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

#include <memory>

using namespace Graphics::Mesh;
using namespace Graphics::Texture;

namespace Graphics {
class MaterialProxy;
}

namespace Graphics {
namespace Data {
struct MeshRenderData {
    std::string mModelPath;
    std::shared_ptr<::Graphics::MaterialProxy> mMaterialProxy;
    bool mIsDeferredShaded;

    MeshRenderData(
        const std::string& modelPath, std::shared_ptr<::Graphics::MaterialProxy> materialProxy, const bool isDeferredShaded)
        : mModelPath(modelPath)
        , mMaterialProxy(materialProxy)
        , mIsDeferredShaded(isDeferredShaded)
    {
    }
};
} // namespace Data
} // namespace Graphics
