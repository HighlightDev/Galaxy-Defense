#pragma once
#include "MaterialNode.h"

namespace Graphics
{
   struct MaterialUnaryOperationNode
      : public MaterialNode
   {
      std::shared_ptr<MaterialNode> mInputOperation;

   public:

      MaterialUnaryOperationNode();

      virtual ~MaterialUnaryOperationNode();

      virtual eMaterialNodeType GetMaterialNodeType() const override;

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode);

      virtual float TraverseGraph() override;

      virtual float DoOperation(const float& value) = 0;
   };
}

