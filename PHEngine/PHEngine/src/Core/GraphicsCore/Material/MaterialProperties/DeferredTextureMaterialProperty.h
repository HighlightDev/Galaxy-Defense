#pragma once

#include "Core/GameCore/LoggerExtension.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResource.h"
#include "MaterialProperty.h"

using namespace Resources;
using namespace EngineCore;

namespace Graphics {

struct DeferredTextureMaterialProperty : public MaterialProperty {
    using MaterialPropertyValueType = DeferredResource<std::shared_ptr<ITexture>, eDeferredResourceType::TEXTURE>;

private:
    std::shared_ptr<MaterialPropertyValueType> m_value;

public:
    DeferredTextureMaterialProperty(std::shared_ptr<MaterialPropertyValueType> propertyValue, const std::string& propertyName)
        : MaterialProperty(propertyName)
        , m_value(propertyValue)
    {
    }

    DeferredTextureMaterialProperty(const std::string& propertyName)
        : MaterialProperty(propertyName)
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::DEFERRED_TEXTURE_PROPERTY;
    }

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    void SetValueToUniform(ActiveBindedState& activeBindedState, Uniform uniform, const int32_t propertyIndex) const override
    {
        if (m_value) {
            std::shared_ptr<ITexture> outResource = nullptr;
            const bool bHasResource = m_value->TryGetResource(outResource);
            if (bHasResource) {
                const auto occupiedSlot = activeBindedState.OccupyTextureSlot(outResource->GetTextureDescriptor());
                if (!occupiedSlot.bWasAlreadyBound) {
                    outResource->BindTexture(occupiedSlot.SlotIndex);
                }
                uniform.LoadUniform(occupiedSlot.SlotIndex);
            } else {
                LogInfo(
                    "DeferredTextureMaterialProperty::SetValueToUniform: property { ", GetPropertyName(), " } is not ready yet.");
            }
        }
    }

    void SetValue(std::shared_ptr<MaterialPropertyValueType> value)
    {
        m_value = value;
    }

    typename MaterialPropertyValueType::arg_t GetValue() const
    {
        std::shared_ptr<ITexture> outResource = nullptr;
        if (m_value) {
            const bool bHasResource = m_value->TryGetResource(outResource);
        }
        return outResource;
    }
};
} // namespace Graphics