#pragma once

#include <functional>

namespace Game
{

   template <typename Type>
   struct VariableWrapper
   {
      using wrapped_type = Type;
      using callback_type = typename std::function<void(void)>;

   private:

      wrapped_type mValue;
      callback_type mCallback;

   public:

      VariableWrapper(wrapped_type initValue, callback_type callback)
         : mValue(initValue)
         , mCallback(callback)
      {
      }

      template <typename ValueType>
      VariableWrapper<wrapped_type>& operator=(ValueType&& value)
      {
         SetValue(value);
         return *this;
      }

      template <typename ValueType>
      void SetValue(ValueType&& value)
      {
         mValue = value;
         mCallback();
      }

      wrapped_type GetValue() const
      {
         return mValue;
      }
   };

}