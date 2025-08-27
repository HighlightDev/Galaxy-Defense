#pragma once

#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "MaterialProperty.h"

using namespace Resources;

namespace Graphics {

struct TextureMaterialProperty : public MaterialProperty {
    using MaterialPropertyValueType = std::shared_ptr<ITexture>;

private:
    MaterialPropertyValueType m_value;

public:
    TextureMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
        : MaterialProperty(propertyName)
        , m_value(propertyValue)
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
            int32_t slot = activeBindedState.OccupyTextureSlot(m_value->GetTextureDescriptor());
            m_value->BindTexture(slot);
            uniform.LoadUniform(slot);
        }
    }

    inline void SetValue(MaterialPropertyValueType value)
    {
        m_value = value;
    }

    inline void SetValue(ITexture* value)
    {
        m_value = std::shared_ptr<ITexture>(value);
    }

    inline std::shared_ptr<ITexture> GetValue() const
    {
        return m_value;
    }
};
} // namespace Graphics