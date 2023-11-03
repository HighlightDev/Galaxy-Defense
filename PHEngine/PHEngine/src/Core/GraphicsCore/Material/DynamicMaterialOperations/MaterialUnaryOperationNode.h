#pragma once
#include "MaterialNode.h"

namespace Graphics
{
   struct MaterialUnaryOperationNode
      : public MaterialNode
   {
      std::shared_ptr<MaterialNode> mInputOperation;

   public:

      explicit MaterialUnaryOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType);

      ~MaterialUnaryOperationNode() override;

      eMaterialNodeType GetMaterialNodeType() const override;

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode);

      std::any TraverseGraph() override;

      virtual std::any DoOperation(const std::any& value) = 0;
   };
}

