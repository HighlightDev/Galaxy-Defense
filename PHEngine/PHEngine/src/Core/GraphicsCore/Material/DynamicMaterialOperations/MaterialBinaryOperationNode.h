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

     virtual eMaterialNodeType GetMaterialNodeType()  const override;

     virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override;

     virtual float TraverseGraph() override;

     virtual float DoOperation(const float& left, const float& right) = 0;
   };
}

