#pragma once
#include <memory>
#include <type_traits>

#include "Core/CommonCore/Assertion.h"

namespace Graphics
{
   //todo: Refactor this
   struct MaterialProperty;

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

      template <typename TypeToCastTo>
      static std::shared_ptr<TypeToCastTo> CastTo(std::shared_ptr<MaterialNode> node)
      {
         assert(node);
         return std::static_pointer_cast<TypeToCastTo>(node);
      }

      virtual eMaterialNodeType GetMaterialNodeType() const = 0;
      virtual eMaterialOperationType GetMaterialOperationType() const = 0;

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) = 0;
   };

   struct MaterialStartNode
      : public MaterialNode
   {
      virtual eMaterialNodeType GetMaterialNodeType() const override
      {
         return MaterialNode::eMaterialNodeType::START;
      }

      virtual eMaterialOperationType GetMaterialOperationType() const override {
         return eMaterialOperationType::NONE;
      }

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override {
         InputOperation = inputNode;
      }

      float getIteratedValue(std::shared_ptr<MaterialNode> node);

      float GetValue();

      std::shared_ptr<MaterialNode> InputOperation;
   };

   struct MaterialValueNode
      : public MaterialNode
   {
      enum class eValueType
      {
         FLOAT_CONSTANT,
         PROPERTY
      };

      virtual eMaterialNodeType GetMaterialNodeType() const override
      {
         return MaterialNode::eMaterialNodeType::VALUE;
      }

      virtual eMaterialOperationType GetMaterialOperationType() const override {
         return eMaterialOperationType::NONE;
      }

      virtual eValueType GetValueType() = 0;

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override { }
   };

   struct MaterialConstantFloatValueNode
      : public MaterialValueNode
   {
      float Value;

      explicit MaterialConstantFloatValueNode(const float value) : Value(value) {}

      virtual eValueType GetValueType() {
         return MaterialValueNode::eValueType::FLOAT_CONSTANT;
      }
   };

   struct MaterialPropertyValueNode
      : public MaterialValueNode
   {
      std::shared_ptr<MaterialProperty> Value;

      explicit MaterialPropertyValueNode(std::shared_ptr<MaterialProperty> value) : Value(value) {}

      virtual eValueType GetValueType() {
         return MaterialValueNode::eValueType::PROPERTY;
      }

   };

   struct MaterialUnaryOperationNode
      : public MaterialNode
   {
      virtual eMaterialNodeType GetMaterialNodeType() const {
         return MaterialNode::eMaterialNodeType::UNARY_OP;
      }

      std::shared_ptr<MaterialNode> InputOperation;

      virtual float DoOperation(const float& value) = 0;

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override {
         InputOperation = inputNode;
      }
   };

   struct MaterialUnaryIncrementNode
      : public MaterialUnaryOperationNode {

      virtual eMaterialOperationType GetMaterialOperationType() const override {
         return eMaterialOperationType::UNARY_INCREMENT;
      }

      virtual float DoOperation(const float& value) override {
         return value + 1.0f;
      }
   };

   struct MaterialUnaryDecrementNode
      : public MaterialUnaryOperationNode {

      virtual eMaterialOperationType GetMaterialOperationType() const override {
         return eMaterialOperationType::UNARY_DECREMENT;
      }

      virtual float DoOperation(const float& value) {
         return value - 1.0f;
      }
   };

   struct MaterialBinaryOperationNode
      : public MaterialNode
   {
      virtual eMaterialNodeType GetMaterialNodeType()  const {
         return MaterialNode::eMaterialNodeType::BINARY_OP;
      }

      std::shared_ptr<MaterialNode> InputOperation1 = nullptr;
      std::shared_ptr<MaterialNode> InputOperation2 = nullptr;

      virtual float DoOperation(const float& left, const float& right) = 0;

      virtual void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override 
      {
         if (!InputOperation1)
         {
            InputOperation1 = inputNode;
         }
         else if (!InputOperation2)
         {
            InputOperation2 = inputNode;
         }
      }
   };

   struct MaterialBinaryAddOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() const override {
         return MaterialNode::eMaterialOperationType::BINARY_ADD;
      }

      virtual float DoOperation(const float& left, const float& right) override {
         return left + right;
      }
   };

   struct MaterialBinarySubOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() const override {
         return MaterialNode::eMaterialOperationType::BINARY_SUB;
      }

      virtual float DoOperation(const float& left, const float& right) override {
         return left - right;
      }
   };

   struct MaterialBinaryMulOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() const override {
         return MaterialNode::eMaterialOperationType::BINARY_MUL;
      }

      virtual float DoOperation(const float& left, const float& right) override {
         return left * right;
      }
   };

   struct MaterialBinaryDivOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() const override {
         return MaterialNode::eMaterialOperationType::BINARY_DIV;
      }

      virtual float DoOperation(const float& left, const float& right) override {
         assert(right != 0.0f);
         return left / right;
      }
   };
}

