#pragma once
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectPropertyBindings/FloatPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/PropertyBinding.h"
#include "Core/GraphicsCore/Material/MaterialProperties/InstancedFloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/ResourceManagerCore/MaterialInstanceDataProviders/MaterialInstanceDataProvider.h"
#include "DynamicMaterialProperty.h"

#include <algorithm>
#include <any>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#undef max
#undef min

using namespace EngineCore;
using namespace Resources;

namespace Graphics {

struct DynamicInstancedFloatMaterialProperty : public DynamicMaterialProperty {
private:
    std::vector<std::pair<std::shared_ptr<PropertyBinding>, std::weak_ptr<MaterialInstanceDataProvider>>> mInstancedBindings;

public:
    explicit DynamicInstancedFloatMaterialProperty(const std::string& propertyName)
        : DynamicMaterialProperty(propertyName)
    {
    }

    eDynamicMaterialPropertyType GetPropertyType() const override
    {
        return DynamicMaterialProperty::eDynamicMaterialPropertyType::InstancedFloatProperty;
    }

    void UpdateStaticPropertyWithDynamicValue(const std::shared_ptr<MaterialProperty>& staticProperty) override
    {
        ext_assert(
            MaterialProperty::eMaterialPropertyType::FLOAT_INSTANCED_PROPERTY == staticProperty->GetPropertyType(),
            "DynamicInstancedFloatMaterialProperty::UpdateStaticPropertyWithDynamicValue: staticProperty type is not "
            "FLOAT_INSTANCED_PROPERTY");
        const auto& staticInstancedFloatMaterialProperty
            = std::static_pointer_cast<InstancedFloatMaterialProperty>(staticProperty);
        staticInstancedFloatMaterialProperty->SetValue(GetValue());
    }

    void AddInstancedBinding(
        const std::shared_ptr<PropertyBinding>& propertyBinding,
        const std::shared_ptr<MaterialInstanceDataProvider>& instanceDataProvider)
    {
        mInstancedBindings.emplace_back(std::make_pair(propertyBinding, instanceDataProvider));
    }

    std::vector<float> GetValue()
    {
        std::vector<float> result;
        result.reserve(mInstancedBindings.size());
        std::sort(mInstancedBindings.begin(), mInstancedBindings.end(), [](const auto& pairLeft, const auto& pairRight) {
            const auto& leftProviderSp = pairLeft.second.lock();
            const auto& rightProviderSp = pairRight.second.lock();
            if (leftProviderSp && rightProviderSp) {
                return leftProviderSp->GetRenderInstanceId() < rightProviderSp->GetRenderInstanceId();
            }
            return false;
        });
        for (const auto& [bindingSp, instanceDataWp] : mInstancedBindings) {
            const auto& instanceDataSp = instanceDataWp.lock();
            if (instanceDataSp && instanceDataSp->IsInstanceActive() && instanceDataSp->GetRenderInstanceId() >= 0) {
                const auto& floatBinding = std::static_pointer_cast<FloatPropertyBinding>(bindingSp);
                result.emplace_back(floatBinding->GetValue());
            }
        }
        return result;
    }
};
} // namespace Graphics
