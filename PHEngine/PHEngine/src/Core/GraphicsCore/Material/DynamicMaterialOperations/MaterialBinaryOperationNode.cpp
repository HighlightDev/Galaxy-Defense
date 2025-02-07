#include "MaterialBinaryOperationNode.h"

namespace Graphics {

MaterialBinaryOperationNode::MaterialBinaryOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType)
    : MaterialNode(materialPropertyType)
    , mInputOperationA(nullptr)
    , mInputOperationB(nullptr)
{
}

MaterialBinaryOperationNode::~MaterialBinaryOperationNode()
{
}

MaterialNode::eMaterialNodeType MaterialBinaryOperationNode::GetMaterialNodeType() const
{
    return MaterialNode::eMaterialNodeType::BINARY_OP;
}

void MaterialBinaryOperationNode::AttachInputNode(std::shared_ptr<MaterialNode> inputNode)
{
    if (!mInputOperationA) {
        mInputOperationA = inputNode;
    } else if (!mInputOperationB) {
        mInputOperationB = inputNode;
    }
}

std::any MaterialBinaryOperationNode::TraverseGraph()
{
    assert(mInputOperationA);
    assert(mInputOperationB);
    return DoOperation(mInputOperationA->TraverseGraph(), mInputOperationB->TraverseGraph());
}

} // namespace Graphics
