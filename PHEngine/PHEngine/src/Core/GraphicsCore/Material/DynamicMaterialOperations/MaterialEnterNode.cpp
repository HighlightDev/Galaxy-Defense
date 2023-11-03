#include "MaterialEnterNode.h"

namespace Graphics
{

   MaterialEnterNode::MaterialEnterNode()
       : MaterialNode(MaterialNode::eMaterialPropertyType::UNDEFINED),
         mInputOperation(nullptr)
   {
   }

   MaterialEnterNode::~MaterialEnterNode()
   {
   }

   MaterialNode::eMaterialNodeType MaterialEnterNode::GetMaterialNodeType() const
   {
      return MaterialNode::eMaterialNodeType::START;
   }

   MaterialNode::eMaterialOperationType MaterialEnterNode::GetMaterialOperationType() const
   {
      return eMaterialOperationType::NONE;
   }

   void MaterialEnterNode::AttachInputNode(std::shared_ptr<MaterialNode> inputNode)
   {
      mInputOperation = inputNode;
   }

   std::any MaterialEnterNode::GetValue()
   {
      return TraverseGraph();
   }

   std::any MaterialEnterNode::TraverseGraph()
   {
      assert(mInputOperation);
      return mInputOperation->TraverseGraph();
   }

   void MaterialEnterNode::SetPropertyType(const MaterialNode::eMaterialPropertyType propertyType)
   {
      MaterialNode::SetPropertyType(propertyType);
   }
}
