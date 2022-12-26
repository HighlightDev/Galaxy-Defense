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
      MaterialBinaryOperationNode();

     virtual ~MaterialBinaryOperationNode();

     eMaterialNodeType GetMaterialNodeType()  const override;

     void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override;

     float TraverseGraph() override;

     virtual float DoOperation(const float& left, const float& right) = 0;
   };
}

