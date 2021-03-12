#pragma once
#include <string>
#include <functional>

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

   Type Value;

public:

   EngineGOProperty(const Type& value, const std::string& key, std::unique_ptr<Action_t> action = std::unique_ptr<Action_t>(nullptr))
      : EngineGOPropertyBase(key)
      , Value(value)
      , Action(std::move(action))
   {
   }

   Type* GetValuePtr() {
      return &Value;
   }

   Type GetValue() const {
      return Value;
   }

   void SetValue(const Type& value)
   {
      Value = value;

      if (Action)
      {
         (*(Action.get()))(Value);
      }
   }

   operator Type() const
   {
      return Value;
   }

   operator Type&()
   {
      return Value;
   }
};
