#pragma once

#include "Core/CommonCore/Assertion.h"

#include <any>
#include <memory>

namespace Graphics {
struct MaterialNode {
    enum class eMaterialPropertyType { UNDEFINED, FLOAT, VEC2, IVEC2, VEC3 };

    enum class eMaterialNodeType {
        START,
        VALUE,
        UNARY_OP,
        BINARY_OP,
    };

    enum class eMaterialOperationType {
        NONE,
        UNARY_INCREMENT,
        UNARY_DECREMENT,
        UNARY_NO_OPERATION,
        BINARY_ADD,
        BINARY_MUL,
        BINARY_SUB,
        BINARY_DIV
    };

    MaterialNode(const eMaterialPropertyType materialPropertyType)
        : mMaterialPropertyType(materialPropertyType)
    {
    }

    virtual ~MaterialNode() = default;

    virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) = 0;

    virtual eMaterialNodeType GetMaterialNodeType() const = 0;

    virtual eMaterialOperationType GetMaterialOperationType() const = 0;

    virtual std::any TraverseGraph() = 0;

    eMaterialPropertyType GetMaterialPropertyType() const
    {
        return mMaterialPropertyType;
    }

protected:
    void SetPropertyType(const MaterialNode::eMaterialPropertyType propertyType)
    {
        mMaterialPropertyType = propertyType;
    }

private:
    eMaterialPropertyType mMaterialPropertyType;
};

} // namespace Graphics
