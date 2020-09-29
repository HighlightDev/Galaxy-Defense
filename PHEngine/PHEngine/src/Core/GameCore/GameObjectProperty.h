#pragma once
#include <string>

struct GameObjectProperty
{
   std::string Key;

   GameObjectProperty(const std::string& key)
      : Key(key)
   {

   }
};

template <typename Type>
struct GenericObjectProperty : public GameObjectProperty
{
   Type Value;
   
   GenericObjectProperty(const Type& value, const std::string& key)
      : GameObjectProperty(key)
      , Value(value)
   {
   }

   Type* GetValuePtr() {
      return &Value;
   }

   void operator=(const Type& value)
   {
      Value = value;
   }

   operator Type&()
   {
      return Value;
   }
};
