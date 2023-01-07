#pragma once

#include <queue>

namespace Event
{
   struct NoDataEventPolicy
   {
      using TupleData_t = std::tuple<>;

   private:
      bool bHasData;
      TupleData_t value;

   public:
      template <typename... DataTypesT>
      void EmplaceData(DataTypesT &&...data)
      {
         bHasData = true;
      }

      const TupleData_t &PopData()
      {
         bHasData = false;
         return value;
      }

      bool HasData() const
      {
         return bHasData;
      }
   };

   template <typename... Args>
   struct SingleDataEventPolicy
   {
      using TupleData_t = std::tuple<Args...>;

   private:
      bool bHasData;
      TupleData_t value;

   public:
      template <typename... DataTypesT>
      void EmplaceData(DataTypesT &&...data)
      {
         value = std::make_tuple(std::forward<DataTypesT>(data)...);
         bHasData = true;
      }

      const TupleData_t &PopData()
      {
         bHasData = false;
         return value;
      }

      bool HasData() const
      {
         return bHasData;
      }
   };

   template <typename... Args>
   struct MultipleDataEventPolicy
   {
      using TupleData_t = std::tuple<Args...>;

   private:
      bool bHasData;
      std::queue<TupleData_t> values;

   public:
      template <typename... DataTypesT>
      void EmplaceData(DataTypesT &&...data)
      {
         values.emplace(std::make_tuple(std::forward<DataTypesT>(data)...));
         bHasData = true;
      }

      TupleData_t PopData()
      {
         auto result = std::move(values.front());
         values.pop();
         bHasData = values.size() > 0;
         return result;
      }

      bool HasData() const
      {
         return bHasData;
      }
   };
}