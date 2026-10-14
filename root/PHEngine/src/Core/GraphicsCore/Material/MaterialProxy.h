#pragma once

#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/GraphicsCore/SceneProxy/SceneProxyBase.h"

#include <memory>
#include <string>
#include <vector>

using namespace Graphics::Texture;

namespace Graphics {
class MaterialProxy : public SceneProxyBase {
public:
    const std::string MaterialName;
    const std::string MaterialShaderName;
    const std::string MaterialShaderRelativePath;

protected:
    std::vector<std::shared_ptr<MaterialProperty>> mProperties;

public:
    MaterialProxy(const class IMaterial* material);

    ~MaterialProxy();

    void CleanUp() override;

    const std::vector<std::shared_ptr<MaterialProperty>>& GetProperties() const;

    std::vector<std::string> GetUniformNames() const;

    std::vector<std::string> GetUniformArrayNames() const;

    void UpdateProperty(std::shared_ptr<MaterialProperty> property);

    void UpdateProperties(std::vector<std::shared_ptr<MaterialProperty>>&& updatedProperties);

    std::shared_ptr<MaterialProperty> GetPropertyByName(const std::string& propertyName) const;
};

} // namespace Graphics
