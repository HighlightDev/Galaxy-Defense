#pragma once

namespace Event
{

   template <typename... Args>
   struct AtomicEventPolicy
   {
      using TupleData_t = std::tuple<Args...>;

   private:

      bool bHasData;
      TupleData_t value;

   public:

      template <typename... DataTypesT>
      void EmplaceData(DataTypesT&&... data)
      {
         value = std::make_tuple(std::forward<DataTypesT>(data)...);
         bHasData = true;
      }

      TupleData_t PopData() {
         bHasData = false;
         return value;
      }

      bool HasData() const
      {
         return bHasData;
      }
   };
}