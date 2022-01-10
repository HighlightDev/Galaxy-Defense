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

   template <typename ValueType>
   EngineGOProperty(const ValueType& value,
      const std::string& key,
      std::unique_ptr<Action_t> action = std::unique_ptr<Action_t>(nullptr))
      : EngineGOPropertyBase(key)
      , ValuePtr(std::make_shared<Type>(value))
      , Action(std::move(action))
   {
   }

   std::shared_ptr<Type> GetValuePtr() {
      return ValuePtr;
   }

   Type GetValue() const {
      return *ValuePtr;
   }

   void SetValue(const Type& value)
   {
      *ValuePtr = value;

      if (Action)
      {
         (*(Action.get()))(value);
      }
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
