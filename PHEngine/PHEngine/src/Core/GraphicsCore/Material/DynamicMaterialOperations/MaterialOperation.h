#pragma once
#include <memory>
#include <type_traits>

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
         BINARY_ADD,
         BINARY_MUL,
         BINARY_SUB,
         BINARY_DIV
      };

      virtual eMaterialNodeType GetMaterialNodeType() = 0;
      virtual eMaterialOperationType GetMaterialOperationType() = 0;
   };

   struct MaterialStartNode
      : public MaterialNode
   {
      virtual eMaterialNodeType GetMaterialNodeType() override
      {
         return MaterialNode::eMaterialNodeType::START;
      }

      virtual eMaterialOperationType GetMaterialOperationType() override {
         return eMaterialOperationType::NONE;
      }

      float getIteratedValue(std::shared_ptr<MaterialNode> node);

      float GetValue();


      std::shared_ptr<MaterialNode> InputOperation;
   };

   struct MaterialValueNode
      : public MaterialNode
   {
      float Value;

      explicit MaterialValueNode(const float value) : Value(value) {

      }

      virtual eMaterialNodeType GetMaterialNodeType() override
      {
         return MaterialNode::eMaterialNodeType::VALUE;
      }

      virtual eMaterialOperationType GetMaterialOperationType() override {
         return eMaterialOperationType::NONE;
      }
   };

   struct MaterialUnaryOperationNode
      : public MaterialNode
   {
      virtual eMaterialNodeType GetMaterialNodeType() {
         return MaterialNode::eMaterialNodeType::UNARY_OP;
      }

      std::shared_ptr<MaterialNode> InputOperation;

      virtual float doOperation(const float& value) = 0;
   };

   struct MaterialUnaryIncrementNode
      : public MaterialUnaryOperationNode {

      virtual eMaterialOperationType GetMaterialOperationType() override {
         return eMaterialOperationType::UNARY_INCREMENT;
      }

      virtual float doOperation(const float& value) override {
         return value + 1;
      }
   };

   struct MaterialUnaryDecrementNode
      : public MaterialUnaryOperationNode {

      virtual eMaterialOperationType GetMaterialOperationType() override {
         return eMaterialOperationType::UNARY_DECREMENT;
      }

      virtual float doOperation(const float& value) {
         return value - 1;
      }
   };

   struct MaterialBinaryOperationNode
      : public MaterialNode
   {
      virtual eMaterialNodeType GetMaterialNodeType() {
         return MaterialNode::eMaterialNodeType::BINARY_OP;
      }

      std::shared_ptr<MaterialNode> InputOperation1;
      std::shared_ptr<MaterialNode> InputOperation2;

      virtual float doOperation(const float& left, const float& right) = 0;
   };

   struct MaterialBinaryAddOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() override {
         return MaterialNode::eMaterialOperationType::BINARY_ADD;
      }

      virtual float doOperation(const float& left, const float& right) override {
         return left + right;
      }
   };

   struct MaterialBinarySubOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() override {
         return MaterialNode::eMaterialOperationType::BINARY_SUB;
      }

      virtual float doOperation(const float& left, const float& right) override {
         return left - right;
      }
   };

   struct MaterialBinaryMulOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() override {
         return MaterialNode::eMaterialOperationType::BINARY_MUL;
      }

      virtual float doOperation(const float& left, const float& right) override {
         return left * right;
      }
   };

   struct MaterialBinaryDivOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() override {
         return MaterialNode::eMaterialOperationType::BINARY_DIV;
      }

      virtual float doOperation(const float& left, const float& right) override {
         assert(right != 0.0f);
         return left / right;
      }
   };
}

