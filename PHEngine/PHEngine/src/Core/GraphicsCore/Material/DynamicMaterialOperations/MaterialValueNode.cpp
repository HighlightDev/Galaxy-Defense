#include "MaterialValueNode.h"

namespace Graphics
{
   MaterialValueNode::MaterialValueNode()
   {
   }

   MaterialValueNode::~MaterialValueNode()
   {
   }

   MaterialNode::eMaterialNodeType MaterialValueNode::GetMaterialNodeType() const
   {
      return MaterialNode::eMaterialNodeType::VALUE;
   }

   MaterialNode::eMaterialOperationType MaterialValueNode::GetMaterialOperationType() const
   {
      return MaterialNode::eMaterialOperationType::NONE;
   }

   void MaterialValueNode::AttachInputNode(std::shared_ptr<MaterialNode> inputNode) 
   {
   }
}
