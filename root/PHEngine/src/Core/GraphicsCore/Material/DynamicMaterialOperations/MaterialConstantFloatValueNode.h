#pragma once
#include "MaterialValueNode.h"

namespace Graphics {
struct MaterialConstantFloatValueNode : public MaterialValueNode {
private:
    float mValue;

public:
    explicit MaterialConstantFloatValueNode(const float value);

    eValueType GetValueType() const override;

    std::any TraverseGraph() override;
};
} // namespace Graphics
