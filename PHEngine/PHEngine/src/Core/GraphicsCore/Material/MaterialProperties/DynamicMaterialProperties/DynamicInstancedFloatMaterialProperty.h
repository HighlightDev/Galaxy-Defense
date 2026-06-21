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

    bool bValueIncremental{false};

    bool mRangeIsSet{false};
    glm::vec2 mRangeMinMax;

    std::vector<float> mCachedValues;

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
        // The uniform array is read in the shader as property[gl_InstanceID], and gl_InstanceID is the render instance
        // id (the instance's position in the batch's valid-instance list, always a contiguous 0..activeCount-1). So
        // write each active binding's value straight to its render instance id, size the array exactly to the active
        // count, and zero everything else. Rebuilding from scratch every frame (rather than overwriting a persistent,
        // never-shrinking vector by a compacted counter) guarantees no stale slot survives a change in the active set
        // and that the value at slot k always belongs to the instance rendered at gl_InstanceID == k.
        int32_t activeInstanceCount = 0;
        for (const auto& [bindingSp, instanceDataWp] : mInstancedBindings) {
            const auto& instanceDataSp = instanceDataWp.lock();
            if (instanceDataSp && instanceDataSp->IsInstanceActive() && instanceDataSp->GetRenderInstanceId() >= 0) {
                activeInstanceCount = std::max(activeInstanceCount, instanceDataSp->GetRenderInstanceId() + 1);
            }
        }

        mCachedValues.assign(static_cast<size_t>(activeInstanceCount), 0.0f);

        for (const auto& [bindingSp, instanceDataWp] : mInstancedBindings) {
            const auto& instanceDataSp = instanceDataWp.lock();
            if (!instanceDataSp || !instanceDataSp->IsInstanceActive()) {
                continue;
            }
            const int32_t renderInstanceId = instanceDataSp->GetRenderInstanceId();
            if (renderInstanceId < 0 || renderInstanceId >= activeInstanceCount) {
                continue;
            }
            const auto& floatBinding = std::static_pointer_cast<FloatPropertyBinding>(bindingSp);
            if (bValueIncremental) {
                mCachedValues[renderInstanceId] += floatBinding->GetValue();
            } else {
                mCachedValues[renderInstanceId] = floatBinding->GetValue();
            }
        }
        return mCachedValues;
    }

    void SetRange(const glm::vec2& range)
    {
        mRangeIsSet = true;
        mRangeMinMax = range;
    }

    glm::vec2 GetRange() const
    {
        return mRangeMinMax;
    }

    void SetIsValueIncremental(bool isIncremental)
    {
        bValueIncremental = isIncremental;
    }

    bool IsValueIncremental() const
    {
        return bValueIncremental;
    }
};
} // namespace Graphics
