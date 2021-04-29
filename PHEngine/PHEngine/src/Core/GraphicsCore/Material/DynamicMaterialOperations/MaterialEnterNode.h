#pragma once
#include "MaterialNode.h"

namespace Graphics {

   struct MaterialEnterNode :
      public MaterialNode
   {
   private:

      std::shared_ptr<MaterialNode> mInputOperation;

   public:

      MaterialEnterNode();

      ~MaterialEnterNode();

      virtual eMaterialNodeType GetMaterialNodeType() const;

      virtual eMaterialOperationType GetMaterialOperationType() const override;

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override;

      float GetValue();

   protected:

      virtual float TraverseGraph() override;
   };
}
