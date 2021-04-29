#include "MaterialBinaryOperationNode.h"

namespace Graphics
{

   MaterialBinaryOperationNode::MaterialBinaryOperationNode()
      : mInputOperationA(nullptr)
      , mInputOperationB(nullptr)
   {
   }

   MaterialBinaryOperationNode::~MaterialBinaryOperationNode()
   {
   }

   MaterialNode::eMaterialNodeType MaterialBinaryOperationNode::GetMaterialNodeType()  const {
      return MaterialNode::eMaterialNodeType::BINARY_OP;
   }

   void MaterialBinaryOperationNode::AttachInputNode(std::shared_ptr<MaterialNode> inputNode)
   {
      if (!mInputOperationA)
      {
         mInputOperationA = inputNode;
      }
      else if (!mInputOperationB)
      {
         mInputOperationB = inputNode;
      }
   }

   float MaterialBinaryOperationNode::TraverseGraph()
   {
      assert(mInputOperationA);
      assert(mInputOperationB);
      float resultNodeA = mInputOperationA->TraverseGraph();
      float resultNodeB = mInputOperationB->TraverseGraph();
      return DoOperation(resultNodeA, resultNodeB);
   }

}
