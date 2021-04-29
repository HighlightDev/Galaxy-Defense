#pragma once
#include "MaterialNode.h"

namespace Graphics 
{
   struct MaterialValueNode :
      public MaterialNode
   {
   public:
      MaterialValueNode();

      virtual ~MaterialValueNode();

      enum class eValueType
      {
         FLOAT_CONSTANT,
         PROPERTY
      };

      virtual eMaterialNodeType GetMaterialNodeType() const override;

      virtual eMaterialOperationType GetMaterialOperationType() const;

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override;

      virtual eValueType GetValueType() const = 0;
   };
}

