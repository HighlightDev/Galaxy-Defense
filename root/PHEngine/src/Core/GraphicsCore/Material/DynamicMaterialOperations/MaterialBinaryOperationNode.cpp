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
    ext_assert(mInputOperationA, "MaterialBinaryOperationNode::TraverseGraph: Input operation A is not attached");
    ext_assert(mInputOperationB, "MaterialBinaryOperationNode::TraverseGraph: Input operation B is not attached");
    return DoOperation(mInputOperationA->TraverseGraph(), mInputOperationB->TraverseGraph());
}

} // namespace Graphics
