#pragma once
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialEnterNode.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec2MaterialProperty.h"
#include "DynamicMaterialProperty.h"

#include <glm/vec2.hpp>

#include <algorithm>
#include <any>
#include <limits>

#undef max
#undef min

namespace Graphics {

struct DynamicIVec2MaterialProperty : public DynamicMaterialProperty {
    using MaterialPropertyValueType = std::shared_ptr<MaterialEnterNode>;

private:
    MaterialPropertyValueType mDynamicOperationStartNode;

    bool bValueIncremental;

    bool mRangeIsSet{false};
    glm::ivec2 mRangeMinMax;

    glm::ivec2 mCachedValue;

public:
    DynamicIVec2MaterialProperty(MaterialPropertyValueType startNode, const std::string& propertyName)
        : DynamicMaterialProperty(propertyName)
        , mDynamicOperationStartNode(startNode)
        , bValueIncremental(false)
        , mRangeMinMax(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max())
        , mCachedValue()
    {
    }

    DynamicIVec2MaterialProperty(const std::string& propertyName)
        : DynamicMaterialProperty(propertyName)
        , mDynamicOperationStartNode()
        , bValueIncremental(false)
        , mRangeMinMax(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max())
        , mCachedValue()
    {
    }

    eDynamicMaterialPropertyType GetPropertyType() const override
    {
        return DynamicMaterialProperty::eDynamicMaterialPropertyType::Vec2Property;
    }

    void UpdateStaticPropertyWithDynamicValue(const std::shared_ptr<MaterialProperty>& staticProperty) override
    {
        ext_assert(
            MaterialProperty::eMaterialPropertyType::VEC2_PROPERTY == staticProperty->GetPropertyType(),
            "DynamicIVec2MaterialProperty::UpdateStaticPropertyWithDynamicValue: staticProperty type is not VEC2_PROPERTY");
        const auto& staticVec2MaterialProperty = std::static_pointer_cast<Vec2MaterialProperty>(staticProperty);
        staticVec2MaterialProperty->SetValue(GetValue());
    }

    void SetValue(MaterialPropertyValueType startNode)
    {
        mDynamicOperationStartNode = startNode;
    }

    glm::ivec2 GetValue()
    {
        ext_assert(
            mDynamicOperationStartNode->GetMaterialPropertyType() == MaterialNode::eMaterialPropertyType::IVEC2,
            "DynamicIVec2MaterialProperty::GetValue: Dynamic operation start node property type is not IVEC2");
        auto value = std::any_cast<glm::ivec2>(mDynamicOperationStartNode->GetValue());

        /*If incremental - add new value to previous value*/
        if (bValueIncremental) {
            value += mCachedValue;
        }

        if (mRangeIsSet) {
            /*Range*/
            if (value.x < mRangeMinMax.x) {
                value.x = mRangeMinMax.y - (mRangeMinMax.x - std::abs(value.x));
            } else if (value.x > mRangeMinMax.y) {
                value.x %= mRangeMinMax.y;
            }

            if (value.y < mRangeMinMax.x) {
                value.y = mRangeMinMax.y - (mRangeMinMax.x - std::abs(value.y));
            } else if (value.y > mRangeMinMax.y) {
                value.y %= mRangeMinMax.y;
            }
        }

        mCachedValue = value;

        return value;
    }

    void SetRange(const glm::ivec2& range)
    {
        mRangeIsSet = true;
        mRangeMinMax = range;
    }

    glm::ivec2 GetRange() const
    {
        return mRangeMinMax;
    }

    void SetIsValueIncremental(const bool isIncremental)
    {
        bValueIncremental = isIncremental;
    }

    bool IsValueIncremental() const
    {
        return bValueIncremental;
    }
};
} // namespace Graphics
