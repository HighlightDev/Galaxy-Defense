#include "MaterialUnaryOperationNode.h"

namespace Graphics {
MaterialUnaryOperationNode::MaterialUnaryOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType)
    : MaterialNode(materialPropertyType)
{
}

MaterialUnaryOperationNode::~MaterialUnaryOperationNode()
{
}

MaterialNode::eMaterialNodeType MaterialUnaryOperationNode::GetMaterialNodeType() const
{
    return MaterialNode::eMaterialNodeType::UNARY_OP;
}

void MaterialUnaryOperationNode::AttachInputNode(std::shared_ptr<MaterialNode> inputNode)
{
    mInputOperation = inputNode;
}

std::any MaterialUnaryOperationNode::TraverseGraph()
{
    assert(mInputOperation);
    return DoOperation(mInputOperation->TraverseGraph());
}
} // namespace Graphics
