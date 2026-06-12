#pragma once

#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "MaterialProperty.h"

using namespace Resources;

namespace Graphics {

struct TextureMaterialProperty : public MaterialProperty {
    using MaterialPropertyValueType = std::shared_ptr<ITexture>;

private:
    MaterialPropertyValueType m_value;

    bool m_isGrayscale{false};

public:
    TextureMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
        : MaterialProperty(propertyName)
        , m_value(propertyValue)
        , m_isGrayscale(propertyValue->GetTextureParameters().bIsGrayscale)
    {
    }

    ~TextureMaterialProperty() override
    {
        TexturePool::GetInstance()->TryToFreeMemory(m_value);
    }

    TextureMaterialProperty(const std::string& propertyName)
        : MaterialProperty(propertyName)
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY;
    }

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    void SetValueToUniform(ActiveBindedState& activeBindedState, Uniform uniform, const int32_t propertyIndex) const override
    {
        if (m_value) {
            const auto occupiedSlot = activeBindedState.OccupyTextureSlot(m_value->GetTextureDescriptor());
            if (!occupiedSlot.bWasAlreadyBound) {
                m_value->BindTexture(occupiedSlot.SlotIndex);
            }
            uniform.LoadUniform(occupiedSlot.SlotIndex);
        }
    }

    inline void SetValue(MaterialPropertyValueType value)
    {
        m_value = value;
        m_isGrayscale = value->GetTextureParameters().bIsGrayscale;
    }

    inline void SetValue(ITexture* value)
    {
        m_value = std::shared_ptr<ITexture>(value);
        m_isGrayscale = value->GetTextureParameters().bIsGrayscale;
    }

    inline std::shared_ptr<ITexture> GetValue() const
    {
        return m_value;
    }

    inline bool IsGrayscale() const
    {
        return m_isGrayscale;
    }
};
} // namespace Graphics