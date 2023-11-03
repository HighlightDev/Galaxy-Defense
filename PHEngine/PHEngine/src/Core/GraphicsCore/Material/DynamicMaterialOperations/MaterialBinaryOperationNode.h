#pragma once
#include "MaterialNode.h"

namespace Graphics
{

   struct MaterialBinaryOperationNode
       : public MaterialNode
   {
   private:
      std::shared_ptr<MaterialNode> mInputOperationA;
      std::shared_ptr<MaterialNode> mInputOperationB;

   public:
      MaterialBinaryOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType);

      virtual ~MaterialBinaryOperationNode();

      eMaterialNodeType GetMaterialNodeType() const override;

      void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override;

      std::any TraverseGraph() override;

      virtual std::any DoOperation(const std::any &left, const std::any &right) = 0;
   };
}
