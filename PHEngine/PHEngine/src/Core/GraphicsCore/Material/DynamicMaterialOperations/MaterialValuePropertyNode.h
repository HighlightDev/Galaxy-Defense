#pragma once
#include "MaterialValueNode.h"

namespace Graphics
{
   struct MaterialProperty;

   struct MaterialValuePropertyNode :
      public MaterialValueNode
   {
   private:

      std::shared_ptr<MaterialProperty> mValueProperty;

   public:
      MaterialValuePropertyNode();

      virtual ~MaterialValuePropertyNode();

      explicit MaterialValuePropertyNode(std::shared_ptr<MaterialProperty> valueProperty);

      virtual eValueType GetValueType() const override;

      std::shared_ptr<MaterialProperty> GetValueProperty() const;

      virtual float TraverseGraph() override;
   };
}

