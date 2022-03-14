#pragma once

#include <memory>

#include "Core/CommonCore/Assertion.h"

namespace Graphics
{
   struct MaterialNode
   {
      enum class eMaterialNodeType
      {
         START,
         VALUE,
         UNARY_OP,
         BINARY_OP,
      };

      enum class eMaterialOperationType {
         NONE,
         UNARY_INCREMENT,
         UNARY_DECREMENT,
         UNARY_NO_OPERATION,
         BINARY_ADD,
         BINARY_MUL,
         BINARY_SUB,
         BINARY_DIV
      };

      MaterialNode() = default;
      virtual ~MaterialNode() = default;

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) = 0;

      virtual eMaterialNodeType GetMaterialNodeType() const = 0;

      virtual eMaterialOperationType GetMaterialOperationType() const = 0;

      virtual float TraverseGraph() = 0;
   };

}
