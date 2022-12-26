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

      ~MaterialValuePropertyNode()  override;

      explicit MaterialValuePropertyNode(std::shared_ptr<MaterialProperty> valueProperty);

      eValueType GetValueType() const override;

      std::shared_ptr<MaterialProperty> GetValueProperty() const;

      float TraverseGraph() override;
   };
}

