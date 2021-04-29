#include "MaterialEnterNode.h"

namespace Graphics {

   MaterialEnterNode::MaterialEnterNode()
      : mInputOperation(nullptr)
   {
   }

   MaterialEnterNode::~MaterialEnterNode()
   {
   }

   MaterialNode::eMaterialNodeType MaterialEnterNode::GetMaterialNodeType() const
   {
      return MaterialNode::eMaterialNodeType::START;
   }

   MaterialNode::eMaterialOperationType MaterialEnterNode::GetMaterialOperationType() const {
      return eMaterialOperationType::NONE;
   }

   void MaterialEnterNode::AttachInputNode(std::shared_ptr<MaterialNode> inputNode) {
      mInputOperation = inputNode;
   }

   float MaterialEnterNode::GetValue()
   {
      return TraverseGraph();
   }

   float MaterialEnterNode::TraverseGraph()
   {
      assert(mInputOperation);
      return mInputOperation->TraverseGraph();
   }
}
