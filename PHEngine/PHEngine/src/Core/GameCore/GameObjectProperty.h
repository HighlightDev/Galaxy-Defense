#pragma once

#include <string>
#include <functional>
#include <memory>

struct EngineGOPropertyBase
{
   std::string Key;

public:

   EngineGOPropertyBase(const std::string& key)
      : Key(key)
   {
   }
};

template <typename Type>
struct EngineGOProperty : public EngineGOPropertyBase
{
  
   using Action_t = std::function<void(const Type&)>;

protected:

   std::unique_ptr<Action_t> Action;

   std::shared_ptr<Type> ValuePtr;

public:

   template <typename ValueType, typename FunctionType>
   EngineGOProperty(const ValueType& value,
      const std::string& key,
      FunctionType action)
      : EngineGOPropertyBase(key)
      , ValuePtr(std::make_shared<Type>(value))
      , Action(std::make_unique<Action_t>(action))
   {
   }

   template <typename ValueType>
   EngineGOProperty(const ValueType& value,
      const std::string& key)
      : EngineGOPropertyBase(key)
      , ValuePtr(std::make_shared<Type>(value))
      , Action()
   {
   }

   std::shared_ptr<Type> GetValuePtr() {
      return ValuePtr;
   }

   Type GetValue() const {
      return *ValuePtr;
   }

   void SetValue(const Type& value, const bool triggerAction = true)
   {
      *ValuePtr = value;

      if (triggerAction && Action)
      {
         (*(Action.get()))(value);
      }
   }

   EngineGOProperty<Type>& operator=(const Type& value) 
   {
      SetValue(value);
      return *this;
   }

   operator Type() const
   {
      return *ValuePtr;
   }

   operator Type&()
   {
      return *ValuePtr;
   }
};
