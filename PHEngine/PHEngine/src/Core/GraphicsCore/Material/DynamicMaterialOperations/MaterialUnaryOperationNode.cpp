#include "MaterialUnaryOperationNode.h"

namespace Graphics
{
   MaterialUnaryOperationNode::MaterialUnaryOperationNode()
   {
   }

   MaterialUnaryOperationNode::~MaterialUnaryOperationNode()
   {
   }

   MaterialNode::eMaterialNodeType MaterialUnaryOperationNode::GetMaterialNodeType() const {
      return MaterialNode::eMaterialNodeType::UNARY_OP;
   }

   void MaterialUnaryOperationNode::AttachInputNode(std::shared_ptr<MaterialNode> inputNode) 
   {
      mInputOperation = inputNode;
   }

   float MaterialUnaryOperationNode::TraverseGraph()
   {
      assert(mInputOperation);
      float resultNode = mInputOperation->TraverseGraph();
      return DoOperation(resultNode);
   }
}
